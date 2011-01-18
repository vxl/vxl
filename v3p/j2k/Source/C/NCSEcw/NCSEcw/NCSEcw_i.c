

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Jan 20 18:00:02 2010
 */
/* Compiler settings for .\NCSEcw.idl:
    Oicf, W3, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

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

MIDL_DEFINE_GUID(IID, LIBID_NCSECWLib,0x1BA90D24,0xDF6F,0x11D3,0xA6,0xC3,0x00,0x50,0x04,0x05,0x5C,0x6C);


MIDL_DEFINE_GUID(IID, IID_INCSRenderer,0xD63891F0,0xE026,0x11D3,0xA6,0xC3,0x00,0x50,0x04,0x05,0x5C,0x6C);


MIDL_DEFINE_GUID(IID, DIID__INCSRendererEvents,0xD63891F2,0xE026,0x11D3,0xA6,0xC3,0x00,0x50,0x04,0x05,0x5C,0x6C);


MIDL_DEFINE_GUID(CLSID, CLSID_NCSRenderer,0xD63891F1,0xE026,0x11D3,0xA6,0xC3,0x00,0x50,0x04,0x05,0x5C,0x6C);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



