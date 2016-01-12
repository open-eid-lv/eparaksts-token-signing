

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IeParakstsIEPluginBHO,0x713951C8,0xAF83,0x4612,0x9E,0xEB,0x9E,0x47,0x53,0x99,0xA9,0x8B);


MIDL_DEFINE_GUID(IID, IID_IeParakstsCertificate,0x0E3620BC,0xDB00,0x4885,0xB8,0xE6,0x0F,0x3D,0xA9,0x98,0x7F,0xCC);


MIDL_DEFINE_GUID(IID, LIBID_eparakstspluginieLib,0x3055A8D3,0xCBEB,0x4BD9,0xB8,0xC7,0xFC,0x76,0x07,0x80,0xE2,0x6B);


MIDL_DEFINE_GUID(CLSID, CLSID_eParakstsIEPluginBHO,0xD458D6D1,0x73E8,0x4D92,0xA1,0xB1,0xE4,0x8F,0xC4,0xF6,0xCC,0xF0);


MIDL_DEFINE_GUID(CLSID, CLSID_eParakstsCertificate,0xF1DB7C78,0xEEF3,0x4509,0xB9,0xFB,0xB5,0xDD,0x35,0x28,0xEA,0xC4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



