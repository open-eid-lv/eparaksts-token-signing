/*
 * Estonian ID card plugin for web browsers
 *
 * Copyright (C) 2010-2011 Codeborne <info@codeborne.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#endif

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/md5.h>

#include "esteid_certinfo.h"
#include "esteid_log.h"
#include "esteid_timer.h"
#include "pkcs11_errors.h"
#include "esteid_map.h"
#include "esteid_error.h"

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
#ifdef INTERNATIONAL
#define LOAD_LIBRARY \
WCHAR path[MAX_PATH], *dir; \
HMODULE hm = NULL, handle = NULL; \
GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&EstEID_loadLibrary, &hm); \
GetModuleFileNameW(hm, path, sizeof(path)); \
dir = wcsrchr(path, '\\'); \
*(++dir) = 0; \
wcscat(path, TEXT(INTERNATIONAL)); \
handle = LoadLibrary(path)
#else
#define LOAD_LIBRARY HMODULE handle = LoadLibrary(L"libeparaksts-module.dll")
#endif
#define GET_FUNCTION_PTR GetProcAddress
char error_buffer[32];

char *library_error() {
	sprintf(error_buffer, "code %lu", GetLastError());
	return error_buffer;
}
#else
#include <dlfcn.h>
#define LOAD_LIBRARY void *handle = dlopen(PKCS11_DRIVER, RTLD_NOW)
#define GET_FUNCTION_PTR dlsym
char *library_error() {
	return dlerror();
}
#endif

// on Windows it is important that openssl is included _after_ windows.h
#include <openssl/x509.h>
#include <openssl/pem.h>

#define OID_DOCUMENT_SIGNING    "1.3.6.1.4.1.311.10.3.12"

#define FAILURE 0
#define SUCCESS 1

#define FAIL_IF(failure) if (failure) { EstEID_freeCerts(); return FAILURE; }
#define FAIL_UNLESS(success) if (!success) { EstEID_freeCerts(); return FAILURE; }
#define CLOSE_SESSION_AND_RETURN(_r) { if (session) { fl->C_CloseSession(session); } return (_r); }
//#define CLOSE_ALL_SESSIONS_AND_RETURN(_r) { fl->C_CloseAllSessions(slotID); return (_r); }

CK_FUNCTION_LIST_PTR fl = NULL;

static EstEID_Certs *certs = NULL;

char EstEID_error[ESTEID_ERROR_SIZE];
int EstEID_errorCode;

void EstEID_clear_error(void) {
	EstEID_error[0] = 0;
	EstEID_errorCode = ESTEID_NO_ERROR;
}

int EstEID_CK_failure(const char *name, CK_RV result) {
	EstEID_clear_error();
	if (result == CKR_OK || result == CKR_CRYPTOKI_ALREADY_INITIALIZED) return FAILURE;
	sprintf(EstEID_error, "%s error: %s (%li)", name, pkcs11_error_message(result), result);
	EstEID_errorCode = ESTEID_PKCS11_ERROR;
	EstEID_log("cryptoki error: %s", EstEID_error);
	return SUCCESS;
}

int EstEID_dl_failure(const char *name, void *ptr) {
	EstEID_clear_error();
	if (ptr) return FALSE;
	snprintf(EstEID_error, sizeof(EstEID_error) - 1, "%s failed: %s", name, library_error());
	EstEID_errorCode = ESTEID_LIBRARY_LOAD_ERROR;
	EstEID_error[sizeof(EstEID_error) - 1] = 0;
	EstEID_log("dl error: %s", EstEID_error);
	return TRUE;
}

int EstEID_md5_failure(void *ptr) {
	EstEID_clear_error();
	if (ptr) return FALSE;
	snprintf(EstEID_error, sizeof(EstEID_error) - 1, "Cert id creation failed");
	EstEID_errorCode = ESTEID_MD5_ERROR;
	EstEID_error[sizeof(EstEID_error) - 1] = 0;
	EstEID_log("%s", EstEID_error);
	return TRUE;
}

char *EstEID_createString(CK_UTF8CHAR *padded, int length) {
	int i = 0;
	char *result = (char *)malloc(sizeof(char)* (length + 1));
	
	strncpy(result, (char *)padded, length);
	result[length] = ' ';
	for (i = length; i >= 0 && result[i] == ' '; i--) result[i] = 0;
	return result;
}

void EstEID_logInitInfo(CK_INFO info) {
	char *libraryDescription = EstEID_createString(info.libraryDescription, 32);
	char *manufacturerID = EstEID_createString(info.manufacturerID, 32);
	EstEID_log("cryptoki %i.%i, library %i.%i %s, %s", info.cryptokiVersion.major, info.cryptokiVersion.minor, info.libraryVersion.major, info.libraryVersion.minor, libraryDescription, manufacturerID);
	free(libraryDescription);
	free(manufacturerID);
}

#ifdef _WIN32
#define pthread_mutex_t HANDLE
#define pthread_cond_t HANDLE
#endif

pthread_mutex_t initialization_mutex;
pthread_cond_t initialization_condition;
//pthread_t initialization_thread;
int initialization_result;
int initialization_completed = FALSE;

THREAD_RETURN_TYPE EstEID_initializeCryptokiThread(void *v) {
	CK_C_INITIALIZE_ARGS  init_args;
	struct timeval start;
    
	LOG_LOCATION;
#ifdef _WIN32
	WaitForSingleObject(initialization_mutex, INFINITE);
#else
	pthread_mutex_lock(&initialization_mutex);
#endif
	memset( &init_args, 0x0, sizeof(init_args) );
	init_args.flags = CKF_OS_LOCKING_OK;
    
	start = EstEID_startTimer();
	initialization_result = fl->C_Initialize(&init_args);
	EstEID_log("C_Initialize: %s (%li)", pkcs11_error_message(initialization_result), initialization_result);
	EstEID_stopTimerAndLog(start, "C_Initialize:");
    
	if (initialization_result == CKR_CRYPTOKI_ALREADY_INITIALIZED) {
		EstEID_log("initialization_result == CKR_CRYPTOKI_ALREADY_INITIALIZED");
	}
    
	if (initialization_result == CKR_OK) {
		CK_INFO info;
		EstEID_log("initialization_result == CKR_OK");
		if (!EstEID_CK_failure("C_GetInfo", fl->C_GetInfo(&info))) {
			EstEID_logInitInfo(info);
		};
	}
	initialization_completed = TRUE;
#ifdef _WIN32
	ReleaseMutex(initialization_mutex);
	return TRUE;
#else
	pthread_cond_broadcast(&initialization_condition);
	pthread_mutex_unlock(&initialization_mutex);
	pthread_exit(NULL);
#endif
}

#ifdef _WIN32

int EstEID_startInitializeCryptokiThread() {
	DWORD threadId;
    
	LOG_LOCATION;
    
	initialization_result = -1;
	FAIL_IF_THREAD_ERROR("CreateMutex", (initialization_mutex = CreateMutex(NULL, FALSE, NULL)));
	EstEID_log("initialization_mutex = %p", initialization_mutex);
	FAIL_IF_THREAD_ERROR("CreateThread", CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&EstEID_initializeCryptokiThread, NULL, 0, &threadId));
	
	return SUCCESS;
}

#else

int EstEID_startInitializeCryptokiThread() {
	initialization_result = -1;
	FAIL_IF_PTHREAD_ERROR("pthread_mutex_init", pthread_mutex_init(&initialization_mutex, NULL));
	FAIL_IF_PTHREAD_ERROR("pthread_cond_init", pthread_cond_init(&initialization_condition, NULL));
	pthread_t initialization_thread;
	FAIL_IF_PTHREAD_ERROR("pthread_create", pthread_create(&initialization_thread, NULL, EstEID_initializeCryptokiThread, NULL));
	return SUCCESS;
}

#endif

int EstEID_loadLibrary() {
	CK_C_GetFunctionList GetFunctionList;
	LOAD_LIBRARY;
    
	LOG_LOCATION;
    if (fl) return SUCCESS;
	
	if (EstEID_dl_failure("dlopen", handle)) return FAILURE;
	GetFunctionList = (CK_C_GetFunctionList)GET_FUNCTION_PTR(handle, "C_GetFunctionList");
	if (EstEID_dl_failure("dlsym", GetFunctionList)) return FAILURE;
	if (EstEID_CK_failure("GetFunctionList", GetFunctionList(&fl))) return FAILURE;
	if (!EstEID_startInitializeCryptokiThread()) return FAILURE;
	
	EstEID_log("successful");
    
	return SUCCESS;
}

int EstEID_initializeCryptoki() {
	LOG_LOCATION;
	if (!EstEID_loadLibrary()) return FAILURE;
#ifdef _WIN32
	if (WAIT_OBJECT_0 != WaitForSingleObject(initialization_mutex, 0)) {
		EstEID_log("waiting for C_Initialize to complete");
		WaitForSingleObject(initialization_mutex, INFINITE);
	}
	while (!initialization_completed) {
		ReleaseMutex(initialization_mutex);
		EstEID_log("waiting for C_Initialize to complete");
		WaitForSingleObject(initialization_mutex, INFINITE);
	}
	ReleaseMutex(initialization_mutex);
#else
	if (0 != pthread_mutex_trylock(&initialization_mutex)) {
		EstEID_log("waiting for C_Initialize to complete");
		pthread_mutex_lock(&initialization_mutex);
	}
	while (!initialization_completed) {
		EstEID_log("waiting for C_Initialize to complete");
		pthread_cond_wait(&initialization_condition, &initialization_mutex);
	}
	pthread_mutex_unlock(&initialization_mutex);
#endif
	if (EstEID_CK_failure("C_Initialize", initialization_result)) return FAILURE;
	return SUCCESS;
}

int EstEID_loadSlotIDs(EstEID_Certs *certs) {
	CK_ULONG slotCount = 0;
	EstEID_log("+++++++++++++++++++++++++++++++++++++ slotCount = %i", slotCount);
	
    FAIL_IF(EstEID_CK_failure("C_GetSlotList", fl->C_GetSlotList(CK_TRUE, NULL_PTR, &slotCount)));
    
    EstEID_log("+++++++++++++++++++++++++++++++++++++ slotCount = %i", slotCount);
	certs->count = slotCount;
	certs->slotIDs = (CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID) * slotCount);
	certs->certs = (EstEID_Map *)malloc(sizeof(EstEID_Map) * slotCount);
    
	FAIL_IF(EstEID_CK_failure("C_GetSlotList", fl->C_GetSlotList(CK_TRUE, certs->slotIDs, &slotCount)));
    
	return SUCCESS;
}

//int EstEID_loadCertEntries(EstEID_Map cert, char *prefix, X509_NAME *x509Name) {
int EstEID_loadCertEntries(EstEID_Map cert, char *prefix, struct X509_name_st *x509Name) {
	unsigned int i;
	// todo: error handling of all openssl functions
	unsigned int count = X509_NAME_entry_count(x509Name);
	for (i = 0; i < count; i++) {
		char *value;
		char name[1024];
		X509_NAME_ENTRY *entry = X509_NAME_get_entry(x509Name, i);
        
		strcpy(name, prefix);
		OBJ_obj2txt(name + strlen(prefix), sizeof(name) - strlen(prefix), entry->object, 0);
        
		ASN1_STRING_to_UTF8((unsigned char **)&value, entry->value);
        
		EstEID_mapPutNoAlloc(cert, strdup(name), value);
	}
	return SUCCESS;
}

char *EstEID_getCertHash(char *certificate) {
	unsigned char certMD5[MD5_DIGEST_LENGTH];
	char result[2 * MD5_DIGEST_LENGTH + 1] = "";
	char chunk[3];
	int i;
    
	MD5((unsigned char*)certificate, strlen(certificate), certMD5);
	
	for(i = 0; i < MD5_DIGEST_LENGTH; i++){
		sprintf(chunk, "%02X", certMD5[i]);
		strcat(result, chunk);
	}
	
	EstEID_log("cert hash as HEX string: %s", result);
	return strdup(result);
}

char *EstEID_ASN1_TIME_toString(ASN1_TIME *time) {
	// ASN1_GENERALIZEDTIME.data format: yyyyMMddHHmmssZ
	ASN1_GENERALIZEDTIME *gt = ASN1_TIME_to_generalizedtime(time, NULL);
	char *data = (char *)gt->data;
	// result format dd.MM.yyyy HH:mm:ss
	// todo maybe: ASN1_GENERALIZEDTIME is in GMT -> should we adjust it?
	char *result = (char *)malloc(20);
	
	LOG_LOCATION;
    
	strncpy(result, data + 6, 2);
	strncpy(result + 3, data + 4, 2);
	strncpy(result + 6, data, 4);
	strncpy(result + 11, data + 8, 2);
	strncpy(result + 14, data + 10, 2);
	strncpy(result + 17, data + 12, 2);
	result[2] = result[5] = '.';
	result[10] = ' ';
	result[13] = result[16] = ':';
	result[19] = 0;
	ASN1_GENERALIZEDTIME_free(gt);
	return result;
}

time_t ASN1_GetTimeT(ASN1_TIME* time){
    struct tm t;
    const char* str = (const char*) time->data;
    size_t i = 0;
    
    memset(&t, 0, sizeof(t));
    
    if (time->type == V_ASN1_UTCTIME) {/* two digit year */
        t.tm_year = (str[i++] - '0') * 10;
        t.tm_year += (str[i++] - '0');
        if (t.tm_year < 70)
            t.tm_year += 100;
    } else if (time->type == V_ASN1_GENERALIZEDTIME) {/* four digit year */
        t.tm_year = (str[i++] - '0') * 1000;
        t.tm_year+= (str[i++] - '0') * 100;
        t.tm_year+= (str[i++] - '0') * 10;
        t.tm_year+= (str[i++] - '0');
        t.tm_year -= 1900;
    }
    t.tm_mon  = (str[i++] - '0') * 10;
    t.tm_mon += (str[i++] - '0') - 1; // -1 since January is 0 not 1.
    t.tm_mday = (str[i++] - '0') * 10;
    t.tm_mday+= (str[i++] - '0');
    t.tm_hour = (str[i++] - '0') * 10;
    t.tm_hour+= (str[i++] - '0');
    t.tm_min  = (str[i++] - '0') * 10;
    t.tm_min += (str[i++] - '0');
    t.tm_sec  = (str[i++] - '0') * 10;
    t.tm_sec += (str[i++] - '0');
    
    /* Note: we did not adjust the time based on time zone information */
    return mktime(&t);
}

int isSigningCertificate(X509 *certificate) {
    time_t currentTime, certificateEndTime;
    ASN1_TIME *notAfter;
    
    time(&currentTime);
    X509_check_ca(certificate);
    
    notAfter = X509_get_notAfter(certificate);
    certificateEndTime = ASN1_GetTimeT(notAfter);
    
    if (difftime(currentTime, certificateEndTime) > 0) {
        EstEID_log("Certificate expired at: %s", EstEID_ASN1_TIME_toString(notAfter));
        return FALSE;
    }
    
    unsigned long keyUsage = certificate->ex_kusage;
    if ((keyUsage & X509v3_KU_NON_REPUDIATION) || (keyUsage & X509v3_KU_DIGITAL_SIGNATURE)) {
        
        EXTENDED_KEY_USAGE *extendedKeyUsage = (EXTENDED_KEY_USAGE*)X509_get_ext_d2i(certificate, NID_ext_key_usage, NULL, NULL);
        if (extendedKeyUsage) {
            for(int i = 0; i < sk_ASN1_OBJECT_num(extendedKeyUsage); i++) {
                ASN1_OBJECT *object = sk_ASN1_OBJECT_value(extendedKeyUsage, i);
                int nid = OBJ_obj2nid(object);
                EstEID_log("NID: %i", nid);
                if (nid == NID_undef) {
                    char obj_tmp[80];
                    i2t_ASN1_OBJECT(obj_tmp, 80, object);
                    char* oid = obj_tmp;
                    EstEID_log("Checking OID \"%s\"", oid);
                    if (strstr(oid, OID_DOCUMENT_SIGNING) != NULL) {
                        EstEID_log("Signing cert with correct extended key usage");
                        return TRUE;
                    }
                }
            }
            EstEID_log("Signing cert with wrong extended key usage");
            return FALSE;
        }
        EstEID_log("Signing cert with correct key usage");
        return TRUE;
    }
    EstEID_log("Signing cert with wrong key usage");
    return FALSE;
}

int EstEID_loadCertInfoEntries(EstEID_Certs *certs, int index) {
	EstEID_Map cert = certs->certs[index];
	CK_SLOT_ID slotID = certs->slotIDs[index];
	CK_SESSION_HANDLE session = 0;
	CK_OBJECT_CLASS objectClass = CKO_CERTIFICATE;
	CK_OBJECT_HANDLE objectHandle[10];
	CK_ULONG objectCount;
	CK_ATTRIBUTE searchAttribute ={CKA_CLASS, &objectClass, sizeof(objectClass)};
	//CK_ATTRIBUTE valueAttribute = {CKA_VALUE, NULL_PTR, 0};
	//CK_ULONG certificateLength;
	//CK_BYTE_PTR certificate;
	char *certMD5, *certSerialNumber, *b, *pem;
	const unsigned char *p;
	X509 *x509;
	//unsigned long keyUsage;
	//ASN1_BIT_STRING *usage;
	BIO *bio;
	int len, count, i;
    
	LOG_LOCATION;
    
	FAIL_IF(EstEID_CK_failure("C_OpenSession", fl->C_OpenSession(slotID, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session)));
    
	if (EstEID_CK_failure("C_FindObjectsInit", fl->C_FindObjectsInit(session, &searchAttribute, 1))) CLOSE_SESSION_AND_RETURN(FAILURE);
    
    count = 0;
    
    while(TRUE) {
        if (EstEID_CK_failure("C_FindObjects", fl->C_FindObjects(session, &objectHandle[count], 1, &objectCount))) {
            break;
        }
        
        EstEID_log("Found certificate handle %lu objectCount %lu ", objectHandle[count], objectCount);
        
        if (objectCount == 0) {
            EstEID_log("Found %i certificates on token", count);
            break;
        }
        count++;
    }
    
    if (EstEID_CK_failure("C_FindObjectsFinal", fl->C_FindObjectsFinal(session))) CLOSE_SESSION_AND_RETURN(FAILURE);

    //FAIL_IF(EstEID_CK_failure("C_FindObjectsFinal", fl->C_FindObjectsFinal(session)));
    
    for(i = 0; i < count; i++) {
        
        CK_ATTRIBUTE valueAttribute = {CKA_VALUE, NULL_PTR, 0};
        
        EstEID_log("Checking certificate %i certificate with handle %lu", i, objectHandle[i]);
        
        if (EstEID_CK_failure("C_GetAttributeValue", fl->C_GetAttributeValue(session, objectHandle[i], &valueAttribute, 1))) CLOSE_SESSION_AND_RETURN(FAILURE);
        
        CK_ULONG certificateLength  = valueAttribute.ulValueLen;
        CK_BYTE_PTR certificate = (CK_BYTE_PTR)malloc(certificateLength);
        valueAttribute.pValue = certificate;
        
        if (EstEID_CK_failure("C_GetAttributeValue", fl->C_GetAttributeValue(session, objectHandle[i], &valueAttribute, 1))) CLOSE_SESSION_AND_RETURN(FAILURE);
        
        // ID Attribute
        CK_ATTRIBUTE idAttribute = { CKA_ID, NULL_PTR, 0 };
        
        if (EstEID_CK_failure("C_GetAttributeValue", fl->C_GetAttributeValue(session, objectHandle[i], &idAttribute, 1))) CLOSE_SESSION_AND_RETURN(FAILURE);
        
        CK_ULONG certIdLength  = idAttribute.ulValueLen;
        CK_BYTE_PTR certId = (CK_BYTE_PTR)malloc(certIdLength);
        idAttribute.pValue = certId;
        
        
        if (EstEID_CK_failure("C_GetAttributeValue", fl->C_GetAttributeValue(session, objectHandle[i], &idAttribute, 1))) CLOSE_SESSION_AND_RETURN(FAILURE);
        
        char* certificateId = EstEID_createString(certId, certIdLength);
        
        p = certificate;
        x509 = d2i_X509(NULL, &p, certificateLength);
        
        if (isSigningCertificate(x509)) {
        
            EstEID_log("cert = %p, certificate = %p, certificateLength = %i", cert, certificate, certificateLength);
            EstEID_mapPutNoAlloc(cert, strdup("certificateAsHex"), EstEID_bin2hex((char *)certificate, certificateLength));
            
            EstEID_mapPut(cert, strdup("certificateIdHex"), EstEID_bin2hex(certificateId, certIdLength));
        
            certMD5 = EstEID_getCertHash((char*)certificate);
            FAIL_IF(EstEID_md5_failure(certMD5));
            EstEID_mapPutNoAlloc(cert, strdup("certHash"), certMD5);
        
            EstEID_mapPutNoAlloc(cert, strdup("validTo"), EstEID_ASN1_TIME_toString(X509_get_notAfter(x509)));
            EstEID_mapPutNoAlloc(cert, strdup("validFrom"), EstEID_ASN1_TIME_toString(X509_get_notBefore(x509)));
            
            EstEID_mapPut(cert, "usageDigitalSignature", "TRUE");
            EstEID_mapPut(cert, "usageNonRepudiation", "TRUE");
            EstEID_mapPut(cert, "keyUsage", "Non-Repudiation");  // for compatibility with older plugin
            EstEID_mapPut(cert, "pinName", count == 1 ? "PIN2" : "PIN");
            
            EstEID_loadCertEntries(cert, "", X509_get_subject_name(x509));
        
            certSerialNumber = (char*)malloc(33);
            snprintf(certSerialNumber, 32, "%lX", ASN1_INTEGER_get(X509_get_serialNumber(x509)));
            EstEID_mapPutNoAlloc(cert, strdup("certSerialNumber"), certSerialNumber);
            EstEID_loadCertEntries(cert, "issuer.", X509_get_issuer_name(x509));
        
            bio = BIO_new(BIO_s_mem());
            if (!PEM_write_bio_X509(bio, x509)) printf("Cannot create PEM\n");
            len = BIO_get_mem_data(bio, &b);
            pem = (char *)malloc(len + 1);
            strncpy(pem, b, len);
            pem[len] = 0;
            BIO_free(bio);
            EstEID_mapPutNoAlloc(cert, strdup("certificateAsPEM"), pem);
            free(certId);
            free(certificate);
            X509_free(x509);
            break;
        
       }
       free(certId);
       free(certificate);
       X509_free(x509);
    }
    
    CLOSE_SESSION_AND_RETURN(SUCCESS);
}

EstEID_Map EstEID_createCertMap(CK_TOKEN_INFO tokenInfo) {
	char *label = EstEID_createString(tokenInfo.label, sizeof(tokenInfo.label));
	EstEID_Map cert = EstEID_mapPutNoAlloc(NULL, strdup("label"), label);
    
	char pinLen[8];
	memset(pinLen, 0x0, 8);
	sprintf(pinLen, "%lu", tokenInfo.ulMinPinLen);
	EstEID_mapPut(cert, "minPinLen", pinLen);
    
	return cert;
}

int EstEID_loadCertInfo(EstEID_Certs *certs, int index) {
	CK_TOKEN_INFO tokenInfo;
	CK_SLOT_ID slotID = certs->slotIDs[index];
	CK_SLOT_INFO slotInfo;
    
	LOG_LOCATION;
    EstEID_log("---------------------- index = %i", index);
    
	FAIL_IF(EstEID_CK_failure("C_GetSlotInfo", fl->C_GetSlotInfo(slotID, &slotInfo)));
    
	if (!(slotInfo.flags & CKF_TOKEN_PRESENT)) return SUCCESS;
    
	FAIL_IF(EstEID_CK_failure("C_GetTokenInfo", fl->C_GetTokenInfo(slotID, &tokenInfo)));
    
	certs->certs[index] = EstEID_createCertMap(tokenInfo);
    
	FAIL_UNLESS(EstEID_loadCertInfoEntries(certs, index));
    
	return SUCCESS;
}

void EstEID_waitForSlotEvent() {
	CK_SLOT_ID slotID = 0;
	CK_RV result = fl->C_WaitForSlotEvent(0, &slotID, NULL_PTR);
	printf("result: %s, slotID: %lu\n", pkcs11_error_message(result), slotID);
}

int EstEID_tokensChanged() {
	CK_SLOT_ID slotID;
	int changed = FALSE;
    
	LOG_LOCATION;
    
	while (fl->C_WaitForSlotEvent(CKF_DONT_BLOCK, &slotID, NULL_PTR) == CKR_OK) {
        EstEID_log("C_WaitForSlotEvent() pass cycle 1");
		changed = TRUE;
	}
	if (!changed) {
		while (fl->C_WaitForSlotEvent(CKF_DONT_BLOCK, &slotID, NULL_PTR) == CKR_OK) {
			EstEID_log("C_WaitForSlotEvent() pass cycle 2");
			changed = TRUE;
		}
	}
	EstEID_log("tokens change %sdetected", changed ? "" : "not ");
	return changed;
}

EstEID_Certs *EstEID_loadCerts() {
	unsigned int i;
	LOG_LOCATION;
	if (!EstEID_initializeCryptoki()) {
		EstEID_log("cryptoki initialization result forces to return NULL");
		return NULL;
	}
	if (certs && !EstEID_tokensChanged()){
		EstEID_log("tokens not changed, returning existing certs");
		return certs;
	}
	if (certs) EstEID_freeCerts();
    
	certs = (EstEID_Certs *)malloc(sizeof(EstEID_Certs));
	if (!EstEID_loadSlotIDs(certs)) return NULL;
	EstEID_log("Certs count=%lu", certs->count);
	for (i = 0; i < certs->count; i++) {
		if (!EstEID_loadCertInfo(certs, i)) return NULL;
		EstEID_log("slotID=%lu", certs->slotIDs[i]);
		EstEID_logMap(certs->certs[i]);
	}
	EstEID_log("returning %u fresh cert(s)", certs->count);
	return certs;
}

void EstEID_printCerts() {
	unsigned int i;
	if (!EstEID_loadCerts()) {
		printf("%s\n", EstEID_error);
		return;
	}
	for (i = 0; i < certs->count; i++) {
		EstEID_Map cert = certs->certs[i];
		fprintf(stdout, "Slot: %lu\n", certs->slotIDs[i]);
		EstEID_mapPrint(stdout, cert);
	}
}

void EstEID_freeCerts() {
	unsigned int i;
	if (certs == NULL) return;
	for (i = 0; i < certs->count; i++) {
		EstEID_mapFree(certs->certs[i]);
	}
	if (certs->certs) free(certs->certs);
	if (certs->slotIDs) free(certs->slotIDs);
	free(certs);
	certs = NULL;
}

EstEID_Map EstEID_getNonRepudiationCert() {
	unsigned int i;
	if (!EstEID_loadCerts()) return NULL;
	for (i = 0; i < certs->count; i++) {
		EstEID_Map cert = certs->certs[i];
		if (EstEID_mapGet(cert, "usageNonRepudiation")) return cert;
	}
	sprintf(EstEID_error, "non-repudiation certificate not found");
	EstEID_errorCode = ESTEID_CERT_NOT_FOUND_ERROR;
	return NULL;
}

EstEID_Map EstEID_getNonRepudiationCertById(char* certID) {
	unsigned int i;
	if (!EstEID_loadCerts()) return NULL;
	for (i = 0; i < certs->count; i++) {
		EstEID_Map cert = certs->certs[i];
		if(EstEID_mapGet(cert, "certHash") && !strcmp(certID, EstEID_mapGet(cert, "certHash"))) {
			return cert;
		}
	}
	sprintf(EstEID_error, "non-repudiation certificate not found");
	EstEID_errorCode = ESTEID_CERT_NOT_FOUND_ERROR;
	return NULL;
}

const char *EstEID_getCertPropertyName(const char *name) {
	char *realName = (char *)name;
	if (!strcmp("CN", name)) realName = "commonName";
	else if (!strcmp("id", name)) realName = "certHash";
	else if (!strcmp("issuerCN", name)) realName = "issuer.commonName";
	else if (!strcmp("cert", name)) realName = "certificateAsHex";
	return realName;
}

char *EstEID_bin2hex(const char *bin, const int binLength) {
	int j;
	char *hex = (char *)malloc(binLength * 2 + 1);
	LOG_LOCATION;
	for (j = 0; j < binLength; j++) sprintf(hex + (j * 2), "%02X", (unsigned char)bin[j]);
	hex[binLength * 2] = '\0';
	return hex;
}

