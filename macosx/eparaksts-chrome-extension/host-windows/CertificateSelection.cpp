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

#include "CertificateSelection.h"
#include "BinaryUtils.h"
#include "HostExceptions.h"
#include <Windows.h>
#include <ncrypt.h>
#include <WinCrypt.h>
#include <cryptuiapi.h>

using namespace std;

extern "C" {

	typedef BOOL(WINAPI * PFNCCERTDISPLAYPROC)(
		__in  PCCERT_CONTEXT pCertContext,
		__in  HWND hWndSelCertDlg,
		__in  void *pvCallbackData
		);

	typedef struct _CRYPTUI_SELECTCERTIFICATE_STRUCT {
		DWORD               dwSize;
		HWND                hwndParent;
		DWORD               dwFlags;
		LPCWSTR             szTitle;
		DWORD               dwDontUseColumn;
		LPCWSTR             szDisplayString;
		PFNCFILTERPROC      pFilterCallback;
		PFNCCERTDISPLAYPROC pDisplayCallback;
		void *              pvCallbackData;
		DWORD               cDisplayStores;
		HCERTSTORE *        rghDisplayStores;
		DWORD               cStores;
		HCERTSTORE *        rghStores;
		DWORD               cPropSheetPages;
		LPCPROPSHEETPAGEW   rgPropSheetPages;
		HCERTSTORE          hSelectedCertStore;
	} CRYPTUI_SELECTCERTIFICATE_STRUCT, *PCRYPTUI_SELECTCERTIFICATE_STRUCT;

	typedef const CRYPTUI_SELECTCERTIFICATE_STRUCT
		*PCCRYPTUI_SELECTCERTIFICATE_STRUCT;

	PCCERT_CONTEXT WINAPI CryptUIDlgSelectCertificateW(
		__in  PCCRYPTUI_SELECTCERTIFICATE_STRUCT pcsc
		);

#define CryptUIDlgSelectCertificate CryptUIDlgSelectCertificateW




}  // extern "C"

BOOL isCardInReader(PCCERT_CONTEXT certContext) {
	DWORD flags = CRYPT_ACQUIRE_CACHE_FLAG | CRYPT_ACQUIRE_COMPARE_KEY_FLAG | CRYPT_ACQUIRE_SILENT_FLAG;
	NCRYPT_KEY_HANDLE key = 0;
	DWORD spec = 0;
	BOOL ncrypt = FALSE;
	CryptAcquireCertificatePrivateKey(certContext, flags, 0, &key, &spec, &ncrypt);
	if (!key) {
		return FALSE;
	}
	if (ncrypt) {
		NCryptFreeObject(key);
	}
	return TRUE;
}

BOOL WINAPI isValidForSigning(PCCERT_CONTEXT certContext) {
	BYTE keyUsage;
	CertGetIntendedKeyUsage(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certContext->pCertInfo, &keyUsage, 1);
	if (!(keyUsage & CERT_NON_REPUDIATION_KEY_USAGE)) {
		return FALSE;
	}
	if (CertVerifyTimeValidity(NULL, certContext->pCertInfo) != 0) {
		return FALSE;
	}

	return isCardInReader(certContext);
}

BOOL WINAPI filter_proc(PCCERT_CONTEXT certContext, BOOL *pfInitialSelectedCert, void *pvCallbackData) {
	return isValidForSigning(certContext);
}

std::string CertificateSelection::getCert() {

	HWND parent = GetForegroundWindow();

	HCERTSTORE store = CertOpenSystemStore(0, L"MY");
	if (!store)
	{
		throw TechnicalException("Failed to open Cert Store");
	}

	PCCERT_CONTEXT pCertContextForEnumeration = nullptr;
	int certificatesCount = 0;
	while (pCertContextForEnumeration = CertEnumCertificatesInStore(store, pCertContextForEnumeration)) {
		if (isValidForSigning(pCertContextForEnumeration)) {
			certificatesCount++;
		}
	}
	if (pCertContextForEnumeration){
		CertFreeCertificateContext(pCertContextForEnumeration);
	}
	if (certificatesCount < 1) {
		CertCloseStore(store, 0);
		throw NoCertificatesException();
	}

	CRYPTUI_SELECTCERTIFICATE_STRUCT pcsc = { sizeof(pcsc) };
	pcsc.pFilterCallback = filter_proc;
	pcsc.hwndParent = parent;
	pcsc.pvCallbackData = nullptr;
	pcsc.cDisplayStores = 1;
	pcsc.rghDisplayStores = &store;
	PCCERT_CONTEXT cert_context = CryptUIDlgSelectCertificate(&pcsc);
	
	if (!cert_context)
	{
		CertCloseStore(store, 0);
		throw UserCancelledException();
	}

	vector<unsigned char> data(cert_context->pbCertEncoded, cert_context->pbCertEncoded + cert_context->cbCertEncoded);
	CertFreeCertificateContext(cert_context);
	CertCloseStore(store, 0);
	return BinaryUtils::bin2hex(data);
}