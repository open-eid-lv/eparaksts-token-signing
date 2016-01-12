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

#pragma once
#include "EstEIDHelper.h"
#include "esteidpluginie_i.h"
#include "HostExceptions.h"
#include <windows.h>
#include <Wincrypt.h>
#include <comutil.h>


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// http://www.askyb.com/bho/step-by-step-guide-to-making-you-bho-epm-compatible-with-ie11-on-windows-8/
// https://msdn.microsoft.com/library/dn519894%28v=vs.85%29.aspx
DEFINE_GUID(CATID_AppContainerCompatible, 0x59fb2056, 0xd625, 0x48d0, 0xa9, 0x44, 0x1a, 0x85, 0xb5, 0xab, 0x26, 0x40);

using namespace ATL;

class ATL_NO_VTABLE CeParakstsIEPluginBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CeParakstsIEPluginBHO, &CLSID_eParakstsIEPluginBHO>,
	public IObjectWithSiteImpl<CeParakstsIEPluginBHO>,
	public IObjectSafetyImpl<CeParakstsIEPluginBHO, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA >,
	public IDispatchImpl<IeParakstsIEPluginBHO, &IID_IeParakstsIEPluginBHO, &LIBID_eparakstspluginieLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CeParakstsIEPluginBHO(){}

DECLARE_REGISTRY_RESOURCEID(IDR_ESTEIDIEPLUGINBHO)

DECLARE_NOT_AGGREGATABLE(CeParakstsIEPluginBHO)

BEGIN_COM_MAP(CeParakstsIEPluginBHO)
	COM_INTERFACE_ENTRY(IeParakstsIEPluginBHO)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		this->errorCode = 0;
		this->language = NULL;
		return S_OK;
	}

	void FinalRelease()
	{
		webBrowser.Release();
	}

private:
	CComPtr<IWebBrowser2> webBrowser;
	CComPtr<IeParakstsCertificate> certificate;
	BSTR language;
	int errorCode;
	std::string errorMessage;

	void clearErrors();
	void setError(BaseException &exception);
	void mapInternalErrorCodes(unsigned int code);
	BOOL isSiteAllowed();
	BOOL isSameCardInReader(CComPtr<IeParakstsCertificate> _cert);
	BOOL CeParakstsIEPluginBHO::certificateMatchesId(PCCERT_CONTEXT certContext, BSTR id);

public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(get_version)(BSTR *result);
	STDMETHOD(get_pluginLanguage)(BSTR *result);
	STDMETHOD(put_pluginLanguage)(BSTR language);
	STDMETHOD(get_errorMessage)(BSTR *result);
	STDMETHOD(get_errorCode)(BSTR *result);
	STDMETHOD(getCertificate)(IDispatch **_certificate);
	STDMETHOD(sign)(BSTR id, BSTR hash, BSTR language, BSTR *signature);	
};

OBJECT_ENTRY_AUTO(__uuidof(eParakstsIEPluginBHO), CeParakstsIEPluginBHO)
