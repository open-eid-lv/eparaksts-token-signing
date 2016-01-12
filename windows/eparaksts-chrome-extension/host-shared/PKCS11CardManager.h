/*
* Chrome Token Signing Native Host
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

//TODO: use polymorphism for Windows/Linux/OSX specifics. This is such an ifndef mess

#include "pkcs11.h"
#include "Logger.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <afx.h> //Using afx.h instead of windows.h because of MFC
#else
#include <dlfcn.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#endif

#define BINARY_SHA1_LENGTH 20
#define BINARY_SHA224_LENGTH 28
#define BINARY_SHA256_LENGTH 32
#define BINARY_SHA384_LENGTH 48
#define BINARY_SHA512_LENGTH 64

#define OID_DOCUMENT_SIGNING    "1.3.6.1.4.1.311.10.3.12"

#ifndef PKCS11_MODULE
#ifdef _WIN32
#define PKCS11_MODULE "libeparaksts-module.dll"
#elif defined(__APPLE__)
#define PKCS11_MODULE "/Library/eparaksts/lib/libeparaksts-module.dylib"
#else
#define PKCS11_MODULE "/usr/lib/pkcs11/libeparaksts-module.so"
#endif
#endif

#define C(API, ...) Call(__FILE__, __LINE__, "C_"#API, fl->C_##API, __VA_ARGS__)

class UserCanceledError : public std::runtime_error {
public:
	UserCanceledError() : std::runtime_error("User canceled"){}
};

class AuthenticationError : public std::runtime_error {
public:
	AuthenticationError() : std::runtime_error("Authentication error"){}
};

class AuthenticationBadInput : public std::runtime_error {
public:
	AuthenticationBadInput() : std::runtime_error("Authentication Bad Input"){}
};

class PKCS11CardManager {
private:
#ifdef _WIN32
	HINSTANCE library = 0;
#else
	void *library = nullptr;
	X509 *cert = nullptr;
	std::vector<CK_BYTE> signCertId;
	//char *signTokenLabel;
#endif
	CK_FUNCTION_LIST_PTR fl = nullptr;
	CK_SLOT_ID managerSlotID;
	CK_TOKEN_INFO tokenInfo;
	CK_SESSION_HANDLE session = 0;
	std::vector<unsigned char> signCert;
    bool haveCert = false;
    char* pinName;

	template <typename Func, typename... Args>
	void Call(const char *file, int line, const char *function, Func func, Args... args) const
	{
		CK_RV rv = func(args...);
		Logger::writeLog(function, file, line, "return value %u", rv);
		switch (rv) {
		case CKR_OK:
			break;
		case CKR_FUNCTION_CANCELED:
			throw UserCanceledError();
		case CKR_PIN_INCORRECT:
			throw AuthenticationError();
		case CKR_PIN_LEN_RANGE:
			throw AuthenticationBadInput();
		default:
			throw std::runtime_error("PKCS11 method failed.");
		}
	}

	std::string bin2hex(const std::vector<unsigned char> &bin) const {
		std::string hex(bin.size() * 2, 0);
		for (size_t j = 0; j < bin.size(); ++j)
			sprintf(&hex[j * 2], "%02X", (unsigned char)bin.at(j));
		return hex;
	}

	std::vector<CK_OBJECT_HANDLE> findPrivateKey() const {
		if (!fl) {
			throw std::runtime_error("PKCS11 is not loaded");
		}

		std::vector<CK_OBJECT_HANDLE> objectHandle(10);
#ifndef _WIN32
		std::string certIdHex = bin2hex(signCertId);
		_log("selected = %i, session = %i, certificateId = %s, ", fl, session, certIdHex.c_str());

		CK_BYTE *objectId = (CK_BYTE *)signCertId.data();
		CK_OBJECT_CLASS keyClass = CKO_PRIVATE_KEY;
		CK_OBJECT_CLASS keyType = CKK_RSA;

		CK_ATTRIBUTE searchTemplate[] = {
			{ CKA_CLASS, &keyClass, sizeof(keyClass) },
			{ CKA_KEY_TYPE, &keyType, sizeof(keyType) },
			{ CKA_ID, objectId, signCertId.size() * sizeof(CK_BYTE) }
		};
		//{ CKA_ID, objectId, sizeof(objectId) / sizeof(objectId[0])}};

		CK_ULONG objectCount = 0;
		CK_ULONG count = 0;

		try {
			C(FindObjectsInit, session, searchTemplate, 3);
			while (true) {
				C(FindObjects, session, &objectHandle[count], 1, &objectCount);
				if (objectCount == 0 && !objectHandle[count]) {
					_log("Exit find loop, count = %i, objectCount = %i, handle = %i", count, objectCount, objectHandle[count]);
					break;
				}
				_log("Found private key %i for certificate id %s", objectHandle[count], certIdHex.c_str());
				count++;
			}
			C(FindObjectsFinal, session);
			_log("Found %i private keys for certificate id %s", count, certIdHex.c_str());
			if (count != 1) {
				_log("Wrong count of private keys found %i", count);
				objectHandle.resize(0);
			}
		}
		catch (const std::runtime_error &) {
			_log("Error finding private key for certificate id %s", certIdHex.c_str());
			objectHandle.resize(0);
		}
#endif
		objectHandle.resize(1);
		return objectHandle;
	}


	std::vector<CK_OBJECT_HANDLE> findCertificate() {
		if (!fl) {
			throw std::runtime_error("PKCS11 is not loaded");
		}

		std::vector<CK_OBJECT_HANDLE> objectHandle(10);

		CK_OBJECT_CLASS objectClass = CKO_CERTIFICATE;
		CK_ATTRIBUTE searchAttribute = { CKA_CLASS, &objectClass, sizeof(objectClass) };
		CK_ULONG objectCount;
		CK_ULONG count = 0;

		C(FindObjectsInit, session, &searchAttribute, 1);

		while (true) {
			try {
				C(FindObjects, session, &objectHandle[count], 1, &objectCount);
				if (objectCount == 0 && !objectHandle[count]) {
					break;
				}
				count++;
			}
			catch (const std::runtime_error &) {
				break;
			}
		}
		C(FindObjectsFinal, session);
		_log("findCertificate read %i objects", count);

		bool found = false;
#ifndef _WIN32
		for (unsigned int i = 0; i < count; i++) {

			CK_ATTRIBUTE valueAttribute = { CKA_VALUE, nullptr, 0 };
			C(GetAttributeValue, session, objectHandle[i], &valueAttribute, 1);
			signCert.resize(valueAttribute.ulValueLen, 0);
			valueAttribute.pValue = &signCert[0];
			C(GetAttributeValue, session, objectHandle[i], &valueAttribute, 1);

			const unsigned char *p = signCert.data();
			cert = d2i_X509(NULL, &p, signCert.size());

			if (isSigningCertificate(cert)) {
				found = true;

				CK_ATTRIBUTE idAttribute = { CKA_ID, nullptr, 0 };
				C(GetAttributeValue, session, objectHandle[i], &idAttribute, 1);
				signCertId.resize(idAttribute.ulValueLen, 0);
				idAttribute.pValue = &signCertId[0];

				_log("findCertificate: certificate id length: %i", idAttribute.ulValueLen);

				C(GetAttributeValue, session, objectHandle[i], &idAttribute, 1);

				_log("findCertificate: certificate id: %s", bin2hex(signCertId).c_str());
				objectHandle[0] = objectHandle[i];
                pinName = (char*) (count == 1 ? "PIN2" : "PIN");
                
				break;
			} else {
				_log("findCertificate: not a signing certificate %i", objectHandle[i]);
				signCert.resize(0);
				X509_free(cert);
			}
		}
#endif
		if (found) {
			objectHandle.resize(1);
			_log("findCertificate: found signing certificate %i", objectHandle[0]);
		} else {
			objectHandle.resize(0);
			_log("findCertificate: signing certificate not found ");
		}

		return objectHandle;
	}

#ifndef _WIN32    
	bool isSigningCertificate(X509 *certificate) const {
		if (!certificate) {
			throw std::runtime_error("Could not parse cert");
		}
		X509_check_ca(certificate);
		unsigned long keyUsage = certificate->ex_kusage;
		if ((keyUsage & X509v3_KU_NON_REPUDIATION) || (keyUsage & X509v3_KU_DIGITAL_SIGNATURE)) {

			EXTENDED_KEY_USAGE *extendedKeyUsage = (EXTENDED_KEY_USAGE*)X509_get_ext_d2i(certificate, NID_ext_key_usage, NULL, NULL);
			if (extendedKeyUsage) {
				for (int i = 0; i < sk_ASN1_OBJECT_num(extendedKeyUsage); i++) {
					ASN1_OBJECT *object = sk_ASN1_OBJECT_value(extendedKeyUsage, i);
					int nid = OBJ_obj2nid(object);
					if (nid == NID_undef) {
						char obj_tmp[80];
						i2t_ASN1_OBJECT(obj_tmp, 80, object);
						char* oid = obj_tmp;
						if (std::strstr(oid, OID_DOCUMENT_SIGNING) != NULL) {
							_log("Signing cert with correct extended key usage");
							return TRUE;
						}
					}
				}
				_log("Signing cert with wrong extended key usage");
				return FALSE;
			}
			_log("Signing cert with correct key usage");
			return TRUE;
		}
		_log("Signing cert with wrong key usage");
		return FALSE;
	}
#endif

	PKCS11CardManager(CK_SLOT_ID slotID, CK_FUNCTION_LIST_PTR fl)
		: fl(fl)
	{
		managerSlotID = slotID;
		C(GetTokenInfo, slotID, &tokenInfo);
		C(OpenSession, slotID, CKF_SERIAL_SESSION, nullptr, nullptr, &session);
		std::vector<CK_OBJECT_HANDLE> objectHandle = findCertificate();
        haveCert = !objectHandle.empty();
		_log("Create instance with slot %lu", managerSlotID);
	}

	PKCS11CardManager(const std::string &module) {
		CK_C_GetFunctionList C_GetFunctionList = nullptr;
#ifdef _WIN32
		library = LoadLibraryA(module.c_str());
		if (library)
			C_GetFunctionList = CK_C_GetFunctionList(GetProcAddress(library, "C_GetFunctionList"));
#else
		library = dlopen(module.c_str(), RTLD_LOCAL | RTLD_NOW);
		if (library)
			C_GetFunctionList = CK_C_GetFunctionList(dlsym(library, "C_GetFunctionList"));
#endif

		if (!C_GetFunctionList) {
			throw std::runtime_error("PKCS11 is not loaded");
		}
		Call(__FILE__, __LINE__, "C_GetFunctionList", C_GetFunctionList, &fl);
		C(Initialize, nullptr);
	}

public:

	static PKCS11CardManager* instance(const std::string &module = PKCS11_MODULE) {
		static PKCS11CardManager instance(module);
		return &instance;
	}

	~PKCS11CardManager() {
		_log("Delete instance with slot %lu", managerSlotID);

		if (!library) {
			_log("Delete close all sessions with slot %lu", managerSlotID);
			C(CloseAllSessions, managerSlotID);

#ifndef _WIN32
			if (haveCert) {
				X509_free(cert);
			}
#endif
		} else {
			_log("Delete finalize with slot %lu", managerSlotID);
			C(Finalize, nullptr);

			_log("Delete dlclose with slot %lu", managerSlotID);
#ifndef _WIN32
			dlclose(library);
#else
			FreeLibrary(library);
#endif
		}

		_log("Delete completed with slot %lu", managerSlotID);
	}

	std::vector<CK_SLOT_ID> getAvailableTokens() const {
		if (!fl) {
			throw std::runtime_error("PKCS11 is not loaded");
		}
		CK_ULONG slotCount = 0;
		CK_TOKEN_INFO tInfo;

		C(GetSlotList, CK_TRUE, nullptr, &slotCount);
		_log("slotCount = %i", slotCount);
		std::vector<CK_SLOT_ID> slotIDs(slotCount, 0);
		C(GetSlotList, CK_TRUE, slotIDs.data(), &slotCount);

		std::vector<CK_SLOT_ID> signingSlotIDs;
		for (CK_ULONG i = 0; i < slotCount; ++i) {
			_log("slotID: %i", slotIDs[i]);
			C(GetTokenInfo, slotIDs[i], &tInfo);
            signingSlotIDs.push_back(slotIDs[i]);
		}
		return signingSlotIDs;
	}

	PKCS11CardManager *getManagerForReader(CK_SLOT_ID slotId) {
		if (!fl) {
			throw std::runtime_error("PKCS11 is not loaded");
		}
		return new PKCS11CardManager(slotId, fl);
	}
    
    bool haveSigningCertificate() {
        return haveCert;
    }
    
	char *getPINName() {
        return pinName;
	}

	std::vector<unsigned char> sign(const std::vector<unsigned char> &hash, const char *pin) const {
		if (!fl) {
			throw std::runtime_error("PKCS11 is not loaded");
		}
		C(Login, session, CKU_USER, (unsigned char*)pin, pin ? strlen(pin) : 0);
		std::vector<CK_OBJECT_HANDLE> privateKeyHandle = findPrivateKey();
		if (privateKeyHandle.empty()) {
			throw std::runtime_error("Could not read private key");
		}

		CK_MECHANISM mechanism = { CKM_RSA_PKCS, 0, 0 };
		C(SignInit, session, &mechanism, privateKeyHandle[0]);
		std::vector<unsigned char> hashWithPadding;
		switch (hash.size()) {
		case BINARY_SHA1_LENGTH:
			hashWithPadding = { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
			break;
		case BINARY_SHA224_LENGTH:
			hashWithPadding = { 0x30, 0x2d, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04, 0x05, 0x00, 0x04, 0x1c };
			break;
		case BINARY_SHA256_LENGTH:
			hashWithPadding = { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
			break;
		case BINARY_SHA384_LENGTH:
			hashWithPadding = { 0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30 };
			break;
		case BINARY_SHA512_LENGTH:
			hashWithPadding = { 0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40 };
			break;
		default:
			_log("incorrect digest length, dropping padding");
		}
		hashWithPadding.insert(hashWithPadding.end(), hash.begin(), hash.end());
		CK_ULONG signatureLength = 0;
		C(Sign, session, hashWithPadding.data(), hashWithPadding.size(), nullptr, &signatureLength);
		std::vector<unsigned char> signature(signatureLength, 0);
		C(Sign, session, hashWithPadding.data(), hashWithPadding.size(), signature.data(), &signatureLength);
		C(Logout, session);

		return signature;
	}

	bool isPinpad() const {
		return tokenInfo.flags & CKF_PROTECTED_AUTHENTICATION_PATH;
	}

	int getPIN2RetryCount() const {
		if (tokenInfo.flags & CKF_USER_PIN_LOCKED) return 0;
		if (tokenInfo.flags & CKF_USER_PIN_FINAL_TRY) return 1;
		if (tokenInfo.flags & CKF_USER_PIN_COUNT_LOW) return 2;
		return 3;
	}

	std::vector<unsigned char> getSignCert() const {
		return signCert;
	}

#ifndef _WIN32
	std::string getSubjectX509Name(const std::string &object) const {
		if (!cert) {
			throw std::runtime_error("Could not parse cert");
		}
		std::string X509Value(1024, 0);
		int length = X509_NAME_get_text_by_NID(X509_get_subject_name(cert), OBJ_txt2nid(object.c_str()), &X509Value[0], int(X509Value.size()));
		X509Value.resize(std::max(0, length));
		_log("%s length is %i, %s", object.c_str(), length, X509Value.c_str());
		return X509Value;
	}

	std::string getCN() const {
		return getSubjectX509Name("commonName");
	}

	std::string getType() const {
		return getSubjectX509Name("organizationName");
	}

	std::string getCardName() const {
		return getSubjectX509Name("givenName") + ", " + getSubjectX509Name("surname");
	}

	std::string getPersonalCode() const {
		return getSubjectX509Name("serialNumber");
	}

	std::string getValidTo() const {
		if (!cert) {
			throw std::runtime_error("Could not parse cert");
		}
		ASN1_GENERALIZEDTIME *gt = ASN1_TIME_to_generalizedtime(X509_get_notAfter(cert), nullptr);
		std::string timeAsString((const char *)gt->data, gt->length);
		ASN1_GENERALIZEDTIME_free(gt);
		return timeAsString;
	}
#endif
};
