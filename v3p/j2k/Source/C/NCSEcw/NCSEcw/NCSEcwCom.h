

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __NCSEcwCom_h__
#define __NCSEcwCom_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INCSRenderer_FWD_DEFINED__
#define __INCSRenderer_FWD_DEFINED__
typedef interface INCSRenderer INCSRenderer;
#endif 	/* __INCSRenderer_FWD_DEFINED__ */


#ifndef ___INCSRendererEvents_FWD_DEFINED__
#define ___INCSRendererEvents_FWD_DEFINED__
typedef interface _INCSRendererEvents _INCSRendererEvents;
#endif 	/* ___INCSRendererEvents_FWD_DEFINED__ */


#ifndef __NCSRenderer_FWD_DEFINED__
#define __NCSRenderer_FWD_DEFINED__

#ifdef __cplusplus
typedef class NCSRenderer NCSRenderer;
#else
typedef struct NCSRenderer NCSRenderer;
#endif /* __cplusplus */

#endif 	/* __NCSRenderer_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_NCSEcw_0000_0000 */
/* [local] */ 

typedef /* [uuid] */  DECLSPEC_UUID("70c70130-35af-11d3-a660-005004055c6d") 
enum tagNCSCellSizeUnits
    {	CU_INVALID	= 0,
	CU_METERS	= 1,
	CU_DEGREES	= 2,
	CU_FEET	= 3,
	CU_UNKNOWN	= 4
    } 	NCSCellSizeUnits;

typedef /* [uuid] */  DECLSPEC_UUID("70c70130-35af-11d3-a660-005004055c6c") 
enum tagNCSGeolinkMode
    {	GM_NONE	= 0,
	GM_WINDOW	= 1,
	GM_SCREEN	= 2
    } 	NCSGeolinkMode;

typedef /* [uuid] */  DECLSPEC_UUID("BD195D00-D248-11d2-88C8-006008A717FD") 
enum tagNCSPointerMode
    {	PM_ROAM	= 0,
	PM_ZOOM	= 1,
	PM_POINT	= 2,
	PM_ZOOMBOX	= 3,
	PM_NONE	= 4
    } 	NCSPointerMode;

typedef /* [uuid] */  DECLSPEC_UUID("0ff4f399-bf8f-4270-a5a3-4527832b3b14") 
enum tagNCSOverlayMode
    {	OM_WORLD	= 0,
	OM_WINRELATIVE	= 1,
	OM_WINABSOLUTE	= 2
    } 	NCSOverlayMode;

typedef /* [uuid] */  DECLSPEC_UUID("466B4405-90FD-4e77-9406-1F3C2ABCA606") 
enum tagNCSMouseVirtualKeys
    {	MVK_CONTROL	= 8,
	MVK_LBUTTON	= 1,
	MVK_MBUTTON	= 16,
	MVK_RBUTTON	= 2,
	MVK_SHIFT	= 4
    } 	NCSMouseVirtualKeys;



extern RPC_IF_HANDLE __MIDL_itf_NCSEcw_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_NCSEcw_0000_0000_v0_0_s_ifspec;


#ifndef __NCSECWLib_LIBRARY_DEFINED__
#define __NCSECWLib_LIBRARY_DEFINED__

/* library NCSECWLib */
/* [helpfile][helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_NCSECWLib;

#ifndef __INCSRenderer_INTERFACE_DEFINED__
#define __INCSRenderer_INTERFACE_DEFINED__

/* interface INCSRenderer */
/* [unique][version][helpcontext][helpstring][dual][oleautomation][uuid][object] */ 


EXTERN_C const IID IID_INCSRenderer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D63891F0-E026-11D3-A6C3-005004055C6C")
    INCSRenderer : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR Filename,
            /* [in] */ VARIANT_BOOL bProgressive) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Transparent( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Transparent( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetBackgroundColor( 
            /* [in] */ short Red,
            /* [in] */ short Green,
            /* [in] */ short Blue) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ VARIANT_BOOL bFreeCache) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Filename( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_NumberOfBands( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CompressionRatio( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CellIncrementX( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CellIncrementY( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_OriginX( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_OriginY( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Projection( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Datum( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetView( 
            /* [in] */ long NumberOfBands,
            /* [in] */ VARIANT *pBandList,
            /* [in] */ long Width,
            /* [in] */ long Height,
            /* [in] */ double WorldTLX,
            /* [in] */ double WorldTLY,
            /* [in] */ double WorldBRX,
            /* [in] */ double WorldBRY) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReadImage( 
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE DrawImage( 
            /* [in] */ /* external definition not present */ OLE_HANDLE hOutputDC,
            /* [in] */ long nRectTLX,
            /* [in] */ long nRectTLY,
            /* [in] */ long nRectBRX,
            /* [in] */ long nRectBRY,
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE FireRefresh( 
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReadLineRGB( 
            /* [out][in] */ VARIANTARG *pRGBTriplets) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReadLineBGR( 
            /* [out][in] */ VARIANTARG *pRGBTriplets) = 0;
        
        virtual /* [helpstring][hidden][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReadLineBIL( 
            /* [out][in] */ VARIANTARG *pRGBLineArray) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Progressive( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetCellUnits( 
            /* [retval][out] */ NCSCellSizeUnits *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE WriteJPG( 
            /* [in] */ BSTR OutputFile,
            /* [in] */ long Quality,
            /* [in] */ VARIANT_BOOL bWriteWorldFile) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetPercentComplete( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConvertWorldToDataset( 
            /* [in] */ double dWorldX,
            /* [in] */ double dWorldY,
            /* [out] */ long *pDatasetX,
            /* [out] */ long *pDatasetY) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ConvertDatasetToWorld( 
            /* [in] */ long nDatasetX,
            /* [in] */ long nDatasetY,
            /* [out] */ double *pWorldX,
            /* [out] */ double *pWorldY) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE FormatErrorText( 
            /* [in] */ long nErrorNumber,
            /* [out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBands( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoTopX( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoLeftY( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBottomX( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoRightY( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWindowSizeX( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWindowSizeY( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBlocksInView( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBlocksAvailable( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBlocksAvailableAtSetView( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoMissedBlocksDuringRead( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWorldTopX( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWorldLeftY( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWorldBottomX( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoWorldRightY( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewSetInfoBandList( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertDatasetToWorldVB( 
            /* [in] */ long nDatasetX,
            /* [in] */ long nDatasetY,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConvertWorldToDatasetVB( 
            /* [in] */ double dWorldX,
            /* [in] */ double dWorldY,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageHeight( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageWidth( 
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INCSRendererVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INCSRenderer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INCSRenderer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INCSRenderer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INCSRenderer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INCSRenderer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INCSRenderer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INCSRenderer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            INCSRenderer * This,
            /* [in] */ BSTR Filename,
            /* [in] */ VARIANT_BOOL bProgressive);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Transparent )( 
            INCSRenderer * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Transparent )( 
            INCSRenderer * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *SetBackgroundColor )( 
            INCSRenderer * This,
            /* [in] */ short Red,
            /* [in] */ short Green,
            /* [in] */ short Blue);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            INCSRenderer * This,
            /* [in] */ VARIANT_BOOL bFreeCache);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Filename )( 
            INCSRenderer * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NumberOfBands )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CompressionRatio )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CellIncrementX )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CellIncrementY )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OriginX )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OriginY )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Projection )( 
            INCSRenderer * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Datum )( 
            INCSRenderer * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *SetView )( 
            INCSRenderer * This,
            /* [in] */ long NumberOfBands,
            /* [in] */ VARIANT *pBandList,
            /* [in] */ long Width,
            /* [in] */ long Height,
            /* [in] */ double WorldTLX,
            /* [in] */ double WorldTLY,
            /* [in] */ double WorldBRX,
            /* [in] */ double WorldBRY);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ReadImage )( 
            INCSRenderer * This,
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *DrawImage )( 
            INCSRenderer * This,
            /* [in] */ /* external definition not present */ OLE_HANDLE hOutputDC,
            /* [in] */ long nRectTLX,
            /* [in] */ long nRectTLY,
            /* [in] */ long nRectBRX,
            /* [in] */ long nRectBRY,
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *FireRefresh )( 
            INCSRenderer * This,
            /* [in] */ double dWorldTLX,
            /* [in] */ double dWorldTLY,
            /* [in] */ double dWorldBRX,
            /* [in] */ double dWorldBRY,
            /* [in] */ long nWidth,
            /* [in] */ long nHeight);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLineRGB )( 
            INCSRenderer * This,
            /* [out][in] */ VARIANTARG *pRGBTriplets);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLineBGR )( 
            INCSRenderer * This,
            /* [out][in] */ VARIANTARG *pRGBTriplets);
        
        /* [helpstring][hidden][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLineBIL )( 
            INCSRenderer * This,
            /* [out][in] */ VARIANTARG *pRGBLineArray);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Progressive )( 
            INCSRenderer * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *GetCellUnits )( 
            INCSRenderer * This,
            /* [retval][out] */ NCSCellSizeUnits *pVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *WriteJPG )( 
            INCSRenderer * This,
            /* [in] */ BSTR OutputFile,
            /* [in] */ long Quality,
            /* [in] */ VARIANT_BOOL bWriteWorldFile);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *GetPercentComplete )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertWorldToDataset )( 
            INCSRenderer * This,
            /* [in] */ double dWorldX,
            /* [in] */ double dWorldY,
            /* [out] */ long *pDatasetX,
            /* [out] */ long *pDatasetY);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertDatasetToWorld )( 
            INCSRenderer * This,
            /* [in] */ long nDatasetX,
            /* [in] */ long nDatasetY,
            /* [out] */ double *pWorldX,
            /* [out] */ double *pWorldY);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE *FormatErrorText )( 
            INCSRenderer * This,
            /* [in] */ long nErrorNumber,
            /* [out] */ BSTR *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBands )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoTopX )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoLeftY )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBottomX )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoRightY )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWindowSizeX )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWindowSizeY )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBlocksInView )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBlocksAvailable )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBlocksAvailableAtSetView )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoMissedBlocksDuringRead )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWorldTopX )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWorldLeftY )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWorldBottomX )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoWorldRightY )( 
            INCSRenderer * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewSetInfoBandList )( 
            INCSRenderer * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertDatasetToWorldVB )( 
            INCSRenderer * This,
            /* [in] */ long nDatasetX,
            /* [in] */ long nDatasetY,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConvertWorldToDatasetVB )( 
            INCSRenderer * This,
            /* [in] */ double dWorldX,
            /* [in] */ double dWorldY,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageHeight )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageWidth )( 
            INCSRenderer * This,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } INCSRendererVtbl;

    interface INCSRenderer
    {
        CONST_VTBL struct INCSRendererVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INCSRenderer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INCSRenderer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INCSRenderer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INCSRenderer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INCSRenderer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INCSRenderer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INCSRenderer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INCSRenderer_Open(This,Filename,bProgressive)	\
    ( (This)->lpVtbl -> Open(This,Filename,bProgressive) ) 

#define INCSRenderer_get_Transparent(This,pVal)	\
    ( (This)->lpVtbl -> get_Transparent(This,pVal) ) 

#define INCSRenderer_put_Transparent(This,newVal)	\
    ( (This)->lpVtbl -> put_Transparent(This,newVal) ) 

#define INCSRenderer_SetBackgroundColor(This,Red,Green,Blue)	\
    ( (This)->lpVtbl -> SetBackgroundColor(This,Red,Green,Blue) ) 

#define INCSRenderer_Close(This,bFreeCache)	\
    ( (This)->lpVtbl -> Close(This,bFreeCache) ) 

#define INCSRenderer_get_Filename(This,pVal)	\
    ( (This)->lpVtbl -> get_Filename(This,pVal) ) 

#define INCSRenderer_get_Width(This,pVal)	\
    ( (This)->lpVtbl -> get_Width(This,pVal) ) 

#define INCSRenderer_get_Height(This,pVal)	\
    ( (This)->lpVtbl -> get_Height(This,pVal) ) 

#define INCSRenderer_get_NumberOfBands(This,pVal)	\
    ( (This)->lpVtbl -> get_NumberOfBands(This,pVal) ) 

#define INCSRenderer_get_CompressionRatio(This,pVal)	\
    ( (This)->lpVtbl -> get_CompressionRatio(This,pVal) ) 

#define INCSRenderer_get_CellIncrementX(This,pVal)	\
    ( (This)->lpVtbl -> get_CellIncrementX(This,pVal) ) 

#define INCSRenderer_get_CellIncrementY(This,pVal)	\
    ( (This)->lpVtbl -> get_CellIncrementY(This,pVal) ) 

#define INCSRenderer_get_OriginX(This,pVal)	\
    ( (This)->lpVtbl -> get_OriginX(This,pVal) ) 

#define INCSRenderer_get_OriginY(This,pVal)	\
    ( (This)->lpVtbl -> get_OriginY(This,pVal) ) 

#define INCSRenderer_get_Projection(This,pVal)	\
    ( (This)->lpVtbl -> get_Projection(This,pVal) ) 

#define INCSRenderer_get_Datum(This,pVal)	\
    ( (This)->lpVtbl -> get_Datum(This,pVal) ) 

#define INCSRenderer_SetView(This,NumberOfBands,pBandList,Width,Height,WorldTLX,WorldTLY,WorldBRX,WorldBRY)	\
    ( (This)->lpVtbl -> SetView(This,NumberOfBands,pBandList,Width,Height,WorldTLX,WorldTLY,WorldBRX,WorldBRY) ) 

#define INCSRenderer_ReadImage(This,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY,nWidth,nHeight)	\
    ( (This)->lpVtbl -> ReadImage(This,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY,nWidth,nHeight) ) 

#define INCSRenderer_DrawImage(This,hOutputDC,nRectTLX,nRectTLY,nRectBRX,nRectBRY,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY)	\
    ( (This)->lpVtbl -> DrawImage(This,hOutputDC,nRectTLX,nRectTLY,nRectBRX,nRectBRY,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY) ) 

#define INCSRenderer_FireRefresh(This,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY,nWidth,nHeight)	\
    ( (This)->lpVtbl -> FireRefresh(This,dWorldTLX,dWorldTLY,dWorldBRX,dWorldBRY,nWidth,nHeight) ) 

#define INCSRenderer_ReadLineRGB(This,pRGBTriplets)	\
    ( (This)->lpVtbl -> ReadLineRGB(This,pRGBTriplets) ) 

#define INCSRenderer_ReadLineBGR(This,pRGBTriplets)	\
    ( (This)->lpVtbl -> ReadLineBGR(This,pRGBTriplets) ) 

#define INCSRenderer_ReadLineBIL(This,pRGBLineArray)	\
    ( (This)->lpVtbl -> ReadLineBIL(This,pRGBLineArray) ) 

#define INCSRenderer_get_Progressive(This,pVal)	\
    ( (This)->lpVtbl -> get_Progressive(This,pVal) ) 

#define INCSRenderer_GetCellUnits(This,pVal)	\
    ( (This)->lpVtbl -> GetCellUnits(This,pVal) ) 

#define INCSRenderer_WriteJPG(This,OutputFile,Quality,bWriteWorldFile)	\
    ( (This)->lpVtbl -> WriteJPG(This,OutputFile,Quality,bWriteWorldFile) ) 

#define INCSRenderer_GetPercentComplete(This,pVal)	\
    ( (This)->lpVtbl -> GetPercentComplete(This,pVal) ) 

#define INCSRenderer_ConvertWorldToDataset(This,dWorldX,dWorldY,pDatasetX,pDatasetY)	\
    ( (This)->lpVtbl -> ConvertWorldToDataset(This,dWorldX,dWorldY,pDatasetX,pDatasetY) ) 

#define INCSRenderer_ConvertDatasetToWorld(This,nDatasetX,nDatasetY,pWorldX,pWorldY)	\
    ( (This)->lpVtbl -> ConvertDatasetToWorld(This,nDatasetX,nDatasetY,pWorldX,pWorldY) ) 

#define INCSRenderer_FormatErrorText(This,nErrorNumber,pVal)	\
    ( (This)->lpVtbl -> FormatErrorText(This,nErrorNumber,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBands(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBands(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoTopX(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoTopX(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoLeftY(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoLeftY(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBottomX(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBottomX(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoRightY(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoRightY(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWindowSizeX(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWindowSizeX(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWindowSizeY(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWindowSizeY(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBlocksInView(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBlocksInView(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBlocksAvailable(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBlocksAvailable(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBlocksAvailableAtSetView(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBlocksAvailableAtSetView(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoMissedBlocksDuringRead(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoMissedBlocksDuringRead(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWorldTopX(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWorldTopX(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWorldLeftY(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWorldLeftY(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWorldBottomX(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWorldBottomX(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoWorldRightY(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoWorldRightY(This,pVal) ) 

#define INCSRenderer_get_ViewSetInfoBandList(This,pVal)	\
    ( (This)->lpVtbl -> get_ViewSetInfoBandList(This,pVal) ) 

#define INCSRenderer_ConvertDatasetToWorldVB(This,nDatasetX,nDatasetY,pVal)	\
    ( (This)->lpVtbl -> ConvertDatasetToWorldVB(This,nDatasetX,nDatasetY,pVal) ) 

#define INCSRenderer_ConvertWorldToDatasetVB(This,dWorldX,dWorldY,pVal)	\
    ( (This)->lpVtbl -> ConvertWorldToDatasetVB(This,dWorldX,dWorldY,pVal) ) 

#define INCSRenderer_get_ImageHeight(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageHeight(This,pVal) ) 

#define INCSRenderer_get_ImageWidth(This,pVal)	\
    ( (This)->lpVtbl -> get_ImageWidth(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INCSRenderer_INTERFACE_DEFINED__ */


#ifndef ___INCSRendererEvents_DISPINTERFACE_DEFINED__
#define ___INCSRendererEvents_DISPINTERFACE_DEFINED__

/* dispinterface _INCSRendererEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__INCSRendererEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D63891F2-E026-11D3-A6C3-005004055C6C")
    _INCSRendererEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _INCSRendererEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _INCSRendererEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _INCSRendererEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _INCSRendererEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _INCSRendererEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _INCSRendererEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _INCSRendererEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _INCSRendererEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _INCSRendererEventsVtbl;

    interface _INCSRendererEvents
    {
        CONST_VTBL struct _INCSRendererEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _INCSRendererEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _INCSRendererEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _INCSRendererEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _INCSRendererEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _INCSRendererEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _INCSRendererEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _INCSRendererEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___INCSRendererEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NCSRenderer;

#ifdef __cplusplus

class DECLSPEC_UUID("D63891F1-E026-11D3-A6C3-005004055C6C")
NCSRenderer;
#endif
#endif /* __NCSECWLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


