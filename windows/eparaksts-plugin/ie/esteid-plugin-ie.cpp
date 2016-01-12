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

// esteid-plugin-ie.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "esteidpluginie_i.h"
#include "dllmain.h"

//Type Library GUID
const GUID CDECL BASED_CODE _tlid = { 0x3055A8D3, 0xCBEB, 0x4BD9, { 0xB8, 0xC7, 0xFC, 0x76, 0x07, 0x80, 0xE2, 0x6B } };
//eParakstsIEPluginBHO GUID
const CLSID CLSID_SafeItem = { 0xD458D6D1, 0x73E8, 0x4D92, { 0xA1, 0xB1, 0xE4, 0x8F, 0xC4, 0xF6, 0xCC, 0xF0 } };

HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription) {
	ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english

	int len = wcslen(catDescription);
	if (len>127)
		len = 127;
	wcsncpy_s(catinfo.szDescription, 127, catDescription, len);
	catinfo.szDescription[len] = '\0';

	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}

HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid) {
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr)) {
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid) {
    ICatRegister *pcr = NULL ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (SUCCEEDED(hr)) {
       // Unregister this category as being "implemented" by the class.
       CATID rgcatid[1] ;
       rgcatid[0] = catid;
       hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
    }

    if (pcr != NULL)
        pcr->Release();

    return hr;
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
	HRESULT  hr;    // return for safety functions

	// _AtlModule.DllRegisterServer required to register *.rgs resources
	_AtlModule.UpdateRegistryAppId(TRUE);
	hr = _AtlModule.RegisterServer(TRUE);
	//hr = _AtlModule.DllRegisterServer();
	if (FAILED(hr))
		return hr;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid)) 
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
        return ResultFromScode(SELFREG_E_CLASS);

    // Mark the control as safe for initializing.
    // https://support.microsoft.com/en-us/kb/161873

    hr = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");
    if (FAILED(hr))
        return hr;
	
    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    // Mark the control as safe for scripting.

    hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");
    if (FAILED(hr))
        return hr;

    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

    return NOERROR;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr;    // HResult used by Safety Functions

	// _AtlModule.DllUnregisterServer required to unregister *.rgs resources
	_AtlModule.UpdateRegistryAppId(FALSE);
	hr = _AtlModule.UnregisterServer(TRUE);
	//hr = _AtlModule.DllUnregisterServer();
	if (FAILED(hr))
		return hr;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!AfxOleUnregisterTypeLib(_tlid))
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
        return ResultFromScode(SELFREG_E_CLASS);

    // Remove entries from the registry.

    hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

    return NOERROR;
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    HRESULT hr = E_FAIL;
    static const wchar_t szUserSwitch[] = _T("user");

    if (pszCmdLine != NULL) {
    	if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0) {
			ATL::AtlSetPerUserRegistration(true);
    	}
    }
    if (bInstall) {	
    	hr = DllRegisterServer();
    	if (FAILED(hr)) {	
    		DllUnregisterServer();
    	}
    }
    else {
    	hr = DllUnregisterServer();
    }
    return hr;
}


