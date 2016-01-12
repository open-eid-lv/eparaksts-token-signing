

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Dec 21 12:02:26 2015
 */
/* Compiler settings for esteidpluginie.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __esteidpluginie_i_h__
#define __esteidpluginie_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IeParakstsIEPluginBHO_FWD_DEFINED__
#define __IeParakstsIEPluginBHO_FWD_DEFINED__
typedef interface IeParakstsIEPluginBHO IeParakstsIEPluginBHO;
#endif 	/* __IeParakstsIEPluginBHO_FWD_DEFINED__ */


#ifndef __IeParakstsCertificate_FWD_DEFINED__
#define __IeParakstsCertificate_FWD_DEFINED__
typedef interface IeParakstsCertificate IeParakstsCertificate;
#endif 	/* __IeParakstsCertificate_FWD_DEFINED__ */


#ifndef __eParakstsIEPluginBHO_FWD_DEFINED__
#define __eParakstsIEPluginBHO_FWD_DEFINED__

#ifdef __cplusplus
typedef class eParakstsIEPluginBHO eParakstsIEPluginBHO;
#else
typedef struct eParakstsIEPluginBHO eParakstsIEPluginBHO;
#endif /* __cplusplus */

#endif 	/* __eParakstsIEPluginBHO_FWD_DEFINED__ */


#ifndef __eParakstsCertificate_FWD_DEFINED__
#define __eParakstsCertificate_FWD_DEFINED__

#ifdef __cplusplus
typedef class eParakstsCertificate eParakstsCertificate;
#else
typedef struct eParakstsCertificate eParakstsCertificate;
#endif /* __cplusplus */

#endif 	/* __eParakstsCertificate_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IeParakstsIEPluginBHO_INTERFACE_DEFINED__
#define __IeParakstsIEPluginBHO_INTERFACE_DEFINED__

/* interface IeParakstsIEPluginBHO */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IeParakstsIEPluginBHO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("713951C8-AF83-4612-9EEB-9E475399A98B")
    IeParakstsIEPluginBHO : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_version( 
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_errorMessage( 
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_errorCode( 
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_pluginLanguage( 
            /* [retval][out] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_pluginLanguage( 
            /* [in] */ BSTR language) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCertificate( 
            /* [retval][out] */ IDispatch **_certificate) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE sign( 
            /* [in] */ BSTR id,
            /* [in] */ BSTR hash,
            /* [in] */ BSTR language,
            /* [retval][out] */ BSTR *signature) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IeParakstsIEPluginBHOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IeParakstsIEPluginBHO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IeParakstsIEPluginBHO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IeParakstsIEPluginBHO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_version )( 
            IeParakstsIEPluginBHO * This,
            /* [retval][out] */ BSTR *result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_errorMessage )( 
            IeParakstsIEPluginBHO * This,
            /* [retval][out] */ BSTR *result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_errorCode )( 
            IeParakstsIEPluginBHO * This,
            /* [retval][out] */ BSTR *result);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_pluginLanguage )( 
            IeParakstsIEPluginBHO * This,
            /* [retval][out] */ BSTR *result);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_pluginLanguage )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ BSTR language);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCertificate )( 
            IeParakstsIEPluginBHO * This,
            /* [retval][out] */ IDispatch **_certificate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *sign )( 
            IeParakstsIEPluginBHO * This,
            /* [in] */ BSTR id,
            /* [in] */ BSTR hash,
            /* [in] */ BSTR language,
            /* [retval][out] */ BSTR *signature);
        
        END_INTERFACE
    } IeParakstsIEPluginBHOVtbl;

    interface IeParakstsIEPluginBHO
    {
        CONST_VTBL struct IeParakstsIEPluginBHOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IeParakstsIEPluginBHO_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IeParakstsIEPluginBHO_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IeParakstsIEPluginBHO_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IeParakstsIEPluginBHO_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IeParakstsIEPluginBHO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IeParakstsIEPluginBHO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IeParakstsIEPluginBHO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IeParakstsIEPluginBHO_get_version(This,result)	\
    ( (This)->lpVtbl -> get_version(This,result) ) 

#define IeParakstsIEPluginBHO_get_errorMessage(This,result)	\
    ( (This)->lpVtbl -> get_errorMessage(This,result) ) 

#define IeParakstsIEPluginBHO_get_errorCode(This,result)	\
    ( (This)->lpVtbl -> get_errorCode(This,result) ) 

#define IeParakstsIEPluginBHO_get_pluginLanguage(This,result)	\
    ( (This)->lpVtbl -> get_pluginLanguage(This,result) ) 

#define IeParakstsIEPluginBHO_put_pluginLanguage(This,language)	\
    ( (This)->lpVtbl -> put_pluginLanguage(This,language) ) 

#define IeParakstsIEPluginBHO_getCertificate(This,_certificate)	\
    ( (This)->lpVtbl -> getCertificate(This,_certificate) ) 

#define IeParakstsIEPluginBHO_sign(This,id,hash,language,signature)	\
    ( (This)->lpVtbl -> sign(This,id,hash,language,signature) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IeParakstsIEPluginBHO_INTERFACE_DEFINED__ */


#ifndef __IeParakstsCertificate_INTERFACE_DEFINED__
#define __IeParakstsCertificate_INTERFACE_DEFINED__

/* interface IeParakstsCertificate */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IeParakstsCertificate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0E3620BC-DB00-4885-B8E6-0F3DA9987FCC")
    IeParakstsCertificate : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_id( 
            /* [retval][out] */ BSTR *id) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_certificateAsHex( 
            /* [retval][out] */ BSTR *certificate) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_cert( 
            /* [retval][out] */ BSTR *certificate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IeParakstsCertificateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IeParakstsCertificate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IeParakstsCertificate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IeParakstsCertificate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IeParakstsCertificate * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IeParakstsCertificate * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IeParakstsCertificate * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IeParakstsCertificate * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_id )( 
            IeParakstsCertificate * This,
            /* [retval][out] */ BSTR *id);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_certificateAsHex )( 
            IeParakstsCertificate * This,
            /* [retval][out] */ BSTR *certificate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_cert )( 
            IeParakstsCertificate * This,
            /* [retval][out] */ BSTR *certificate);
        
        END_INTERFACE
    } IeParakstsCertificateVtbl;

    interface IeParakstsCertificate
    {
        CONST_VTBL struct IeParakstsCertificateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IeParakstsCertificate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IeParakstsCertificate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IeParakstsCertificate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IeParakstsCertificate_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IeParakstsCertificate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IeParakstsCertificate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IeParakstsCertificate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IeParakstsCertificate_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 

#define IeParakstsCertificate_get_certificateAsHex(This,certificate)	\
    ( (This)->lpVtbl -> get_certificateAsHex(This,certificate) ) 

#define IeParakstsCertificate_get_cert(This,certificate)	\
    ( (This)->lpVtbl -> get_cert(This,certificate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IeParakstsCertificate_INTERFACE_DEFINED__ */



#ifndef __eparakstspluginieLib_LIBRARY_DEFINED__
#define __eparakstspluginieLib_LIBRARY_DEFINED__

/* library eparakstspluginieLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_eparakstspluginieLib;

EXTERN_C const CLSID CLSID_eParakstsIEPluginBHO;

#ifdef __cplusplus

class DECLSPEC_UUID("D458D6D1-73E8-4D92-A1B1-E48FC4F6CCF0")
eParakstsIEPluginBHO;
#endif

EXTERN_C const CLSID CLSID_eParakstsCertificate;

#ifdef __cplusplus

class DECLSPEC_UUID("F1DB7C78-EEF3-4509-B9FB-B5DD3528EAC4")
eParakstsCertificate;
#endif
#endif /* __eparakstspluginieLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


