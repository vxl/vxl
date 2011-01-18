/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
** 
** FILE:   	ComNCSRenderer.h
** CREATED:	11 Feb 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	COM class wrapper for NCSRenderer C++ class.
**			for use in visual basic apps where you can't
**			layer ActiveX controls.
** EDITS:
**
*******************************************************/

#ifndef __COMNCSRENDERER_H_
#define __COMNCSRENDERER_H_

#include "NCSRenderer.h"
#include "NCSBuildNumber.h"

#if !defined(_WIN32_WCE)
	#include "crtdbg.h"
	#include "NCSEcwCP.h"
	#include "resource.h"
#else
	#include <atlctl.h>
	#include "..\NCSEcwCE\NCSEcwCP.h"
	#include "..\NCSEcwCE\resource.h"       
#endif


typedef enum {
	NCS_READ_RGB = 0,
	NCS_READ_BGR,
	NCS_READ_BIL
}NCSReadType;

//#if defined(_WIN32_WCE)	//as idl file for CE can't have definitions
//	enum NCSCellSizeUnits
//		{	CU_INVALID	= 0,
//		CU_METERS	= 1,
//		CU_DEGREES	= 2,
//		CU_FEET	= 3
//		};
//#endif

class ATL_NO_VTABLE CComNCSRenderer : public CNCSRenderer,
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<INCSRenderer, &IID_INCSRenderer, &LIBID_NCSECWLib>,
	public CComControl<CComNCSRenderer>,
#if !defined(_WIN32_WCE)
	public IObjectSafetyImpl<CComNCSRenderer, INTERFACESAFE_FOR_UNTRUSTED_CALLER|INTERFACESAFE_FOR_UNTRUSTED_DATA>,
#endif
	public IPersistStreamInitImpl<CComNCSRenderer>,
	public IOleControlImpl<CComNCSRenderer>,
	public IOleObjectImpl<CComNCSRenderer>,
	public IOleInPlaceActiveObjectImpl<CComNCSRenderer>,
	public IViewObjectExImpl<CComNCSRenderer>,
	public IOleInPlaceObjectWindowlessImpl<CComNCSRenderer>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CComNCSRenderer>,
	public IPersistStorageImpl<CComNCSRenderer>,
	public ISpecifyPropertyPagesImpl<CComNCSRenderer>,
	public IQuickActivateImpl<CComNCSRenderer>,
	public IDataObjectImpl<CComNCSRenderer>,
	public IProvideClassInfo2Impl<&CLSID_NCSRenderer, &DIID__INCSRendererEvents, &LIBID_NCSECWLib>,
	public IPropertyNotifySinkCP<CComNCSRenderer>,
	public CComCoClass<CComNCSRenderer, &CLSID_NCSRenderer>,
	public CProxy_INCSRendererEvents<CComNCSRenderer>
{
public:
	CComNCSRenderer();
	~CComNCSRenderer();

DECLARE_REGISTRY_RESOURCEID(IDR_NCSRENDERER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComNCSRenderer)
	COM_INTERFACE_ENTRY(INCSRenderer)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
#if !defined(_WIN32_WCE)
	COM_INTERFACE_ENTRY(IObjectSafety)
#endif
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_PROP_MAP(CComNCSRenderer)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CComNCSRenderer)
//	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(DIID__INCSRendererEvents)
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CComNCSRenderer)
	CHAIN_MSG_MAP(CComControl<CComNCSRenderer>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_INCSRenderer,
		};
		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (::IsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// INCSRenderer
public:
	STDMETHOD(get_ImageWidth)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ImageHeight)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBandList)(/*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(get_ViewSetInfoWorldRightY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_ViewSetInfoWorldBottomX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_ViewSetInfoWorldLeftY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_ViewSetInfoWorldTopX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_ViewSetInfoMissedBlocksDuringRead)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBlocksAvailableAtSetView)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBlocksAvailable)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBlocksInView)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoWindowSizeY)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoWindowSizeX)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoRightY)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBottomX)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoLeftY)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoTopX)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ViewSetInfoBands)(/*[out, retval]*/ long *pVal);
	
	STDMETHOD(FormatErrorText)(/*[in]*/ long nErrorNumber, /*[out]*/ BSTR *pVal);
	STDMETHOD(ConvertDatasetToWorld)(/*[in]*/ long nDatasetX, /*[in]*/ long nDatasetY, /*[out]*/ double *pWorldX, /*[out]*/ double *pWorldY);
	STDMETHOD(ConvertDatasetToWorldVB)(/*[in]*/ long nDatasetX, /*[in]*/ long nDatasetY, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(ConvertWorldToDataset)(/*[in]*/ double dWorldX, /*[in]*/ double dWorldY, /*[out]*/ long *pDatasetX, /*[out]*/ long *pDatasetY);
	STDMETHOD(ConvertWorldToDatasetVB)(/*[in]*/ double dWorldX, /*[in]*/ double dWorldY, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(GetPercentComplete)(/*[out, retval]*/ long *pVal);
	
	STDMETHOD(WriteJPG)(/*[in]*/ BSTR OutputFile, /*[in]*/ long Quality, /*[in]*/ VARIANT_BOOL bWriteWorldFile);
	STDMETHOD(GetCellUnits)(/*[out, retval]*/ NCSCellSizeUnits *pVal);
	STDMETHOD(get_Progressive)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(ReadLineBIL)(/*[in,out]*/ VARIANTARG* pLineArray);
	STDMETHOD(ReadLineBGR)(/*[in,out]*/ VARIANTARG* pRGBTriplets);
	STDMETHOD(ReadLineRGB)(/*[in,out]*/ VARIANTARG* pRGBTriplets);
	STDMETHOD(FireRefresh)(double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY, long nWidth, long nHeight);
	STDMETHOD(FinalConstruct)(void);
	STDMETHOD(DrawImage)(/*[in]*/ OLE_HANDLE hDeviceContext, long nRectTLX, long nRectTLY ,long nRectBRX, long nRectBRY, double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY );
	STDMETHOD(ReadImage)(/*[in]*/ double dWorldTLX, /*[in]*/ double dWorldTLY, /*[in]*/ double dWorldBRX, /*[in]*/ double dWorldBRY, /*[in]*/ long nWidth, /*[in]*/ long nHeight);
	STDMETHOD(SetView)(/*[in]*/ long NumberOfBands, /*[in]*/ VARIANT* pBandList, /*[in]*/ long Width, /*[in]*/ long Height, /*[in]*/ double WorldTLX, /*[in]*/ double WorldTLY, /*[in]*/ double WorldBRX, /*[in]*/ double WorldBRY);
	STDMETHOD(get_Datum)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Projection)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_OriginY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_OriginX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_CellIncrementY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_CellIncrementX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_CompressionRatio)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_NumberOfBands)(/*[out]*/ long *pVal);
	STDMETHOD(get_Height)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Width)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Transparent)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_Transparent)(/*[in]*/ BOOL newVal);
	STDMETHOD(Open)(/*[in]*/ BSTR Filename, /*[in]*/ VARIANT_BOOL Progressive);
	STDMETHOD(Close)(/*[in]*/ VARIANT_BOOL bFreeCache);
	STDMETHOD(SetBackgroundColor)(/*[in]*/ short Red, /*[in]*/ short Green, /*[in]*/ short Blue);

	virtual void RefreshUpdate(NCSFileViewSetInfo *pViewSetInfo);

	STDMETHOD(GetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
	{
		// This sets the default size of the design time control
		dwDrawAspect = DVASPECT_CONTENT;
		psizel->cx = 3000;
		psizel->cy = 1500;
		return S_OK;
	}
	STDMETHOD(SetExtent)(DWORD dwDrawAspect, SIZEL *psizel)
	{
		dwDrawAspect;psizel;//Keep compiler happy
		// We wont listen to the containers resize event.
		return E_FAIL;
	}

	HRESULT OnDraw(ATL_DRAWINFO& di);

private:
	STDMETHOD(ReadLine)(VARIANTARG* pLineArray, NCSReadType nType);
	LPSTREAM m_pStream;
	BOOLEAN m_bHaveStreamInit;
	HBITMAP m_hECWIcon;
	void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart, short yStart, COLORREF cTransparentColor);

};

#endif //__COMNCSRENDERER_H_
