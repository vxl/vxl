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
** FILE:   	ComNCSRenderer.cpp
** CREATED:	11 Feb 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	COM class wrapper for NCSRenderer C++ class.
**			for use in Visual Basic apps where you can't
**			layer ActiveX controls.
**			Inside VB, the user needs to get a device 
**			context as a stdole.OLE_HANDLE which it 
**			passes to the Draw() method of this object.
** EDITS:
** [01] rdh	26Jun00	Changed MessageBox Errors to return
**					Errors or compile only in Debug.
** [02] jmp 12Jun01 Marshalling the stream prevents the object from closing when all references removed.
**					Moved marshalling code to Open call and UnMarshalling code to Close call.
**					Unmarshalling doesn't destroy all references to the marshalled stream, this causes
**					refcounts to remain so object is not destroyed, using CoDisconnectObject to remove all
**					refcounts.
** [03] jmp 21Jun01 Added ImageWidth and ImageHeight properties, same as Width and Height, needed for Delphi
**					as delphi has standard container properties of height and width.
*******************************************************/

#ifdef _WIN32_WCE
#include "comutil.h"
#define _INC_COMIP
#endif
	#include "stdafx.h"
	#include "stdio.h"
	#include "NCSEcwCom.h"
	#include "../NCSEcw/ComNCSRenderer.h"
	#include "COMDef.h"
	#include "NCSMalloc.h"
	#include "NCSUtil.h"
//#else
//	#include "..\NCSEcwCE\stdafx.h"
//	#include "..\NCSEcwCE\NCSEcw.h"
//	#include "../NCSEcwCE/ComNCSRenderer.h"
//	#include "COMDef.h"
//	#include "NCSMalloc.h"
//	#include "NCSUtil.h"
//#endif

#define FILE_NOT_OPEN_MESSAGE "File not open."
#define SET_VIEW_FAIL_BANDLIST "SetView() failed, due to an error accessing an element of the BandList array."
#define SAFEARRAY_NOT_ONE_DIMENSION "An array of more than one dimension was passed to SetView(), ignoring all others except the first one."
#define SAFEARRAY_DIM_NOTEQUAL_BANDS "Dimension of array does not match input number of bands. Continuing anyway."

CComNCSRenderer::CComNCSRenderer()
{
	m_pStream = NULL;
	m_bHaveStreamInit = FALSE;

	m_hECWIcon = (HBITMAP)LoadImage(_Module.GetModuleInstance(),
							MAKEINTRESOURCE(IDB_NCSRENDERER32),
							IMAGE_BITMAP,
							0,
							0,
							LR_DEFAULTCOLOR);
}

CComNCSRenderer::~CComNCSRenderer()
{
#if !defined(_WIN32_WCE)
	if(m_pStream) {									/**[02]**/
		HRESULT hr = E_FAIL;
		INCSRenderer *pRend;

		// Get the marshalled interface pointer from the stream
		LARGE_INTEGER     bZero = {0,0};
		hr = m_pStream->Seek(bZero, STREAM_SEEK_SET, NULL);
		hr = CoUnmarshalInterface(m_pStream, IID_INCSRenderer, (void**)&pRend);
		hr = CoReleaseMarshalData(m_pStream);

		if (pRend)
		{
#if (_WIN32_WCE >= 300)
			hr = CoDisconnectObject(pRend, 0);
#endif
			pRend->Release();
		}
		m_pStream->Release();
		m_pStream = NULL;
		m_bHaveStreamInit = FALSE;
	}
#endif

//	if(m_pStream) {									/**[02]**/
//		m_pStream->Release();
//	}
	if (m_hECWIcon){
		DeleteObject(m_hECWIcon);
	}
}

STDMETHODIMP CComNCSRenderer::FinalConstruct(void)
{
//	HRESULT hr;											/**[02]**/
/*	INCSRenderer *pRend = this;

	// Marshal the interface to a stream, so that we can acces the interface pointer in other threads.	
	hr = CreateStreamOnHGlobal(0, TRUE, &m_pStream);
	if (SUCCEEDED(hr)) {
		hr = CoMarshalInterface(m_pStream, IID_INCSRenderer, (IUnknown *)pRend, 
								MSHCTX_INPROC, 0, MSHLFLAGS_TABLESTRONG);
	}
	if(!SUCCEEDED(hr)) {
		m_pStream = NULL; 
	}
	else {
		m_bHaveStreamInit = TRUE;
	}
*/
	return S_OK;
}

/**
 * Open an ecw image.
 * Open a local file path or a ecwp protocol URL image.
 * @param Filename A BString representing the input filename URL or local path
 * @param Progressive A boolean value, specifying wheather to open the image in progressive decoding mode.
 * @see Close()
 * @return HRESULT
 */
STDMETHODIMP CComNCSRenderer::Open(BSTR Filename, VARIANT_BOOL Progressive)
{
	NCSError nResult;
	USES_CONVERSION;
	char *pfname;
	
	pfname = OLE2A(Filename);
	if (pfname == NULL) {
		return E_INVALIDARG;
	}

	if (m_bIsOpen) {
		Close(VARIANT_FALSE);
	}

	nResult = CNCSRenderer::Open(pfname, (BOOLEAN)((Progressive == VARIANT_FALSE) ? FALSE : TRUE));
	if (nResult != NCS_SUCCESS) {
#if !defined(_WIN32_WCE)
		return Error(A2OLE(NCSGetLastErrorText(nResult)));
#else
		return Error(OS_STRING(NCSGetLastErrorText(nResult)));
#endif
	}
	else
	{
#if !defined(_WIN32_WCE)
		if (!m_pStream)								/**[02]**/
		{
			HRESULT hr;
			INCSRenderer *pRend = this;

			// Marshal the interface to a stream, so that we can acces the interface pointer in other threads.	
			hr = CreateStreamOnHGlobal(0, TRUE, &m_pStream);
			if (SUCCEEDED(hr)) {
				hr = CoMarshalInterface(m_pStream, IID_INCSRenderer, (IUnknown *)pRend, 
										MSHCTX_INPROC, 0, MSHLFLAGS_TABLESTRONG);
			}
			if(!SUCCEEDED(hr)) {
				m_pStream = NULL; 
			}
			else {
				m_bHaveStreamInit = TRUE;
			}
		}
#endif
	}
	return S_OK;
}

/**
 * Close an ecw image.
 * Close a previously opened ecw image. Closing the image will free the cache and reclaim any memory allocated to the renderer object.
 * @param Filename A BString representing the input filename URL or local path
 * @param Progressive A boolean value, specifying wheather to open the image in progressive decoding mode.
 * @see Close()
 * @return HRESULT
 */
STDMETHODIMP CComNCSRenderer::Close(VARIANT_BOOL bFreeCache)
{
	CNCSRenderer::Close((BOOLEAN)((bFreeCache == VARIANT_FALSE) ? FALSE : TRUE));

	//destroy the marshalled object referenced in from the stream
#if !defined(_WIN32_WCE)
	if(m_pStream) {										/**[02]**/
		HRESULT hr = E_FAIL;
		INCSRenderer *pRend;

		// Get the marshalled interface pointer from the stream
		LARGE_INTEGER     bZero = {0,0};
		hr = m_pStream->Seek(bZero, STREAM_SEEK_SET, NULL);
		hr = CoUnmarshalInterface(m_pStream, IID_INCSRenderer, (void**)&pRend);
		hr = CoReleaseMarshalData(m_pStream);

		if (pRend)
		{
			hr = CoDisconnectObject(pRend, 0);
			pRend->Release();
		}
		m_pStream->Release();
		m_pStream = NULL;
		m_bHaveStreamInit = FALSE;
	}
#endif

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_Transparent(BOOL *pVal)
{
	CNCSRenderer::GetTransparent((BOOLEAN *)pVal);
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::put_Transparent(BOOL newVal)
{
	CNCSRenderer::SetTransparent(newVal);
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::SetBackgroundColor(short Red, short Green, short Blue)
{
	CNCSRenderer::SetBackgroundColor(RGB(Red, Green, Blue));
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_Filename(BSTR *pVal)
{
	USES_CONVERSION;
	if (m_bIsOpen) {
		*pVal = ::SysAllocString(A2OLE(CNCSFile::m_pFilename));
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_Width(long *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_nWidth;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_Height(long *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_nHeight;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_NumberOfBands(long *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_nNumberOfBands;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_CompressionRatio(double *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_dCompressionRate;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_CellIncrementX(double *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_dCellIncrementX;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_CellIncrementY(double *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_dCellIncrementY;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_OriginX(double *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_dOriginX;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_OriginY(double *pVal)
{
	if (m_bIsOpen) {
		*pVal = CNCSFile::m_dOriginY;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_Projection(BSTR *pVal)
{
	USES_CONVERSION;
	if (m_bIsOpen) {
		*pVal = ::SysAllocString(A2OLE(CNCSFile::m_pProjection));
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_Datum(BSTR *pVal)
{
	USES_CONVERSION;
	if (m_bIsOpen) {
		*pVal = ::SysAllocString(A2OLE(CNCSFile::m_pDatum));
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::get_Progressive(VARIANT_BOOL *pVal)
{
	if (m_bIsOpen) {
		*pVal = (VARIANT_BOOL)CNCSRenderer::m_bIsProgressive;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::GetCellUnits(NCSCellSizeUnits *pVal)
{
	if (m_bIsOpen) {
		*pVal = (NCSCellSizeUnits)CNCSFile::m_CellSizeUnits;
		return S_OK;
	}
	else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}
}

STDMETHODIMP CComNCSRenderer::SetView(long NumberOfBands, VARIANT *pBandList, long Width, long Height, double WorldTLX, double WorldTLY, double WorldBRX, double WorldBRY)
{
	USES_CONVERSION;
	HRESULT hr = S_OK;
	
	if (m_bIsOpen) {
		long nIndex;
		INT32 *pBandArray;

		pBandArray = (INT32 *)NCSMalloc(sizeof(INT32)*NumberOfBands, 1);		

		if (pBandList->vt & VT_ARRAY) {
			// if ARRAY, process as SAFEARRAY
			SAFEARRAY *pIntArray;
			long nSize;
			long nDimension;
			long nLBound = 0;
			INT32 nNumber;

			if(pBandList->vt & VT_BYREF) {
				pIntArray = *(pBandList->pparray);
			} else {
				pIntArray = pBandList->parray;
			}
			nSize = pIntArray->rgsabound->cElements;
			nDimension = SafeArrayGetDim(pIntArray);

			
#ifdef DEBUG
			if (nDimension != 1) {
				::MessageBox(NULL, TEXT(SAFEARRAY_NOT_ONE_DIMENSION), TEXT("NCS Debug"), MB_OK|MB_TOPMOST);
			}
			if (nSize < NumberOfBands) {
				::MessageBox(NULL, TEXT(SAFEARRAY_DIM_NOTEQUAL_BANDS), TEXT("NCS Debug"), MB_OK|MB_TOPMOST);
			}
#endif
#if !defined(_WIN32_WCE)
			if(pBandList->vt & VT_VARIANT) {
				// Array of Variants
				VARIANT *pArrayData;

				hr = SafeArrayAccessData(pIntArray, (void **)&pArrayData);

				if(SUCCEEDED(hr)) {
					for (nIndex=0; nIndex<MIN(nSize, NumberOfBands); nIndex++) {
						_variant_t MyVariant(pArrayData[nIndex]);

						pBandArray[nIndex] = (long)MyVariant;
					}
					SafeArrayUnaccessData(pIntArray);
				}
			} else {
#endif
				// SafeArray
				SafeArrayLock(pIntArray);
				
				hr = SafeArrayGetLBound(pIntArray, 1, &nLBound);
				if(SUCCEEDED(hr)) {
					for (nIndex=0; nIndex<MIN(nSize, NumberOfBands); nIndex++) {
						long nThisIndex = nIndex + nLBound;

#if defined(_WIN32_WCE)
						VARIANT FAR rgvar[1];
						VariantInit(&rgvar[0]);
						hr = SafeArrayGetElement(pIntArray, &nThisIndex, &rgvar[0]);
						nNumber = rgvar[0].iVal;
						VariantClear(&rgvar[0]);
#else
						hr = SafeArrayGetElement(pIntArray, &nThisIndex, (void *)&nNumber);
#endif 
						if (SUCCEEDED(hr)) {
							pBandArray[nIndex] = nNumber;
						} else {
							break;
						}
					}
				}
				SafeArrayUnlock(pIntArray);
#if !defined(_WIN32_WCE)
			}
#endif
			
#if !defined(_WIN32_WCE)
		} else if (pBandList->vt & VT_BSTR) {
			// JavaScript passes arrays as a comma separated BSTR
			INT32 i = 0;

			_bstr_t pBStr (pBandList);
			char *pString = (char*)pBStr;
			char *pTmp = pString;

			for (nIndex=0; nIndex<NumberOfBands; nIndex++) {
				pBandArray[nIndex] = atoi(pTmp);
				while((*pTmp != '\0') && (*pTmp != ',')) {
					pTmp++;
				}
			}
#endif
		} else {
			  return E_INVALIDARG;
		}

		if (SUCCEEDED(hr)){
			NCSError eError;

			eError = CNCSRenderer::SetView(NumberOfBands, 
										   pBandArray, 
										   Width, Height,
										   WorldTLX, WorldTLY, 
										   WorldBRX, WorldBRY);
			NCSFree(pBandArray);
			if(eError != NCS_SUCCESS) {
				// Dont generate an error if the region is outside the file.
				// This should be handled in the lower classes to draw nada.
				if (eError != NCS_REGION_OUTSIDE_FILE) {
#if !defined(_WIN32_WCE)
					return Error(A2OLE(CNCSFile::FormatErrorText(eError)));
#else
					return Error(OS_STRING(CNCSFile::FormatErrorText(eError)));
#endif
				}
			}
		}
		else {
			return Error(_T(SET_VIEW_FAIL_BANDLIST));
		}

	} else {
		return Error(_T(FILE_NOT_OPEN_MESSAGE));
	}

	return hr;
}

const LARGE_INTEGER     bZero = {0,0};

void CComNCSRenderer::RefreshUpdate(NCSFileViewSetInfo *pViewSetInfo)
{
	HRESULT hr = E_FAIL;

	// Make sure COM is initialized for this thread (its a callback from ECW lib).
	// We could make this faster in the future by using a free threaded model, COINIT_MULTITHREADED 
//	CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY|COINIT_APARTMENTTHREADED); - not supported before IE4.01 SP2 - hangs install!
#ifdef _WIN32_WCE
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	CoInitialize(NULL);
#endif
	
#if !defined(_WIN32_WCE)
	INCSRenderer *pRend = NULL;
	if(m_pStream) {
		// Get the marshalled interface pointer from the stream
		hr = m_pStream->Seek(bZero, STREAM_SEEK_SET, NULL);
		hr = CoUnmarshalInterface(m_pStream, IID_INCSRenderer, (void**)&pRend);
	}
	if(pRend != NULL) {
		if(m_pStream && SUCCEEDED(hr)) {
			if (m_bSetViewModeIsWorld) {
				// Call the marshalled interface to fire a refresh event
				pRend->FireRefresh(pViewSetInfo->fTopX, pViewSetInfo->fLeftY, 
									pViewSetInfo->fBottomX, pViewSetInfo->fRightY, 
									pViewSetInfo->nSizeX, pViewSetInfo->nSizeY);	
			}
			else {
				Error(_T("Sorry, only world coordinates are acceptable when using VisualBasic and NCSRenderer events."));
			}
		}

		// Clean up
		pRend->Release();
	} else {
		Error(_T("Failed to create INCSRenderer."));
	}
#else
	FireRefresh(pViewSetInfo->fTopX, pViewSetInfo->fLeftY, 
				pViewSetInfo->fBottomX, pViewSetInfo->fRightY, 
				pViewSetInfo->nSizeX, pViewSetInfo->nSizeY);	
#endif
	CoUninitialize();
	
	return;
}

STDMETHODIMP CComNCSRenderer::ReadImage(double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY, long nWidth, long nHeight)
{
	if (m_bIsProgressive) {
		INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;
		
		CNCSFile::ConvertWorldToDataset(dWorldTLX, dWorldTLY, &nDatasetTLX, &nDatasetTLY);
		CNCSFile::ConvertWorldToDataset(dWorldBRX, dWorldBRY, &nDatasetBRX, &nDatasetBRY);
		
		CNCSRenderer::ReadImage(dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY, 
								nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY,
								nWidth, nHeight);
	}
	else {
		CNCSRenderer::ReadImage(nWidth, nHeight);
	}
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::DrawImage( OLE_HANDLE hDeviceContext, long nRectTLX, long nRectTLY ,long nRectBRX, long nRectBRY, double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY )
{	
	RECT ClipRect;

	ClipRect.left   = nRectTLX;
	ClipRect.top    = nRectTLY;
	ClipRect.right  = nRectBRX;
	ClipRect.bottom = nRectBRY;
#pragma warning(disable: 4312)
	HDC hDC = (HDC)hDeviceContext;
#pragma warning(default: 4312)
	CNCSRenderer::DrawImage(hDC, &ClipRect, dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY);

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::FireRefresh(double dWorldTLX, double dWorldTLY, double dWorldBRX, double dWorldBRY, long nWidth, long nHeight)
{
	Fire_RefreshUpdate(dWorldTLX,  dWorldTLY,  dWorldBRX,  dWorldBRY,  nWidth,  nHeight);
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::ReadLine(VARIANTARG* pRGBTriplets, NCSReadType nType)
{
	UINT8 HUGEP *pArrayData;
	HRESULT hr = S_OK;

	if(pRGBTriplets->vt & VT_ARRAY) {
		SAFEARRAY *pRGBTripletArray;
		long nSize;
		long nDimension;
//		UINT nDataSize;

		if(pRGBTriplets->vt & VT_BYREF) {
			pRGBTripletArray = *(pRGBTriplets->pparray);
		} else {
			pRGBTripletArray = pRGBTriplets->parray;
		}
		nSize = pRGBTripletArray->rgsabound->cElements;
		nDimension = SafeArrayGetDim(pRGBTripletArray);
//		nDataSize = SafeArrayGetElemsize(pRGBTripletArray);


			// Lock the array for quick access
		hr = SafeArrayAccessData(pRGBTripletArray, (void **)&pArrayData);
	
		// The datatype (for RGB/BGR) must be a byte, it must be a one dimensional array and the width must equal the current set view width.
		if ((nType == NCS_READ_RGB)||(nType == NCS_READ_BGR)) {

			if(((pRGBTriplets->vt & VT_VARIANT) && (((VARIANT*)pArrayData)[0].vt != VT_UI1)) || 
			   (!(pRGBTriplets->vt & VT_VARIANT) && !(pRGBTriplets->vt & VT_UI1))) {
				// Unlock the array
				SafeArrayUnaccessData(pRGBTripletArray);
				return Error(_T("ReadLineRGB(): The data type of the array passed, is not a Byte."));											/**[01]**/
			}
		}
		if (nSize < m_nSetViewWidth) {
			// Unlock the array
			SafeArrayUnaccessData(pRGBTripletArray);
			return Error(_T("ReadLineRGB(): Width of input array is less than the current width passed to SetView(). Aborting read...."));		/**[01]**/
		}


		if (!FAILED(hr)) {

			NCSEcwReadStatus nStatus = NCSECW_READ_OK;
			UINT8 *pByteArrayData = NULL;

			if(pRGBTriplets->vt & VT_VARIANT) {
				//pByteArrayData = (UINT8*)NCSMalloc(m_nSetViewWidth * sizeof(UINT8) * m_nNumberOfBands, FALSE);
				pByteArrayData = (UINT8*)NCSMalloc(m_nSetViewWidth * (sizeof(UINT8)*3) * m_nNumberOfBands, FALSE);
			}

			switch (nType) {
				case NCS_READ_RGB : 
#ifdef DEBUG																																	/**[01]**/
					if (nDimension != 1) {
						::MessageBox(NULL, TEXT("ReadLineRGB(): An array of more than one dimension was input, ignoring all other dimensions except the first one."), TEXT("NCS Debug"), MB_OK|MB_TOPMOST);
					}
#endif																																			/**[01]**/
					nStatus = CNCSRenderer::ReadLineRGB(pByteArrayData ? pByteArrayData : pArrayData);
					break;
				case NCS_READ_BGR:
#ifdef DEBUG																																	/**[01]**/
					if (nDimension != 1) {
						::MessageBox(NULL, TEXT("ReadLineRGB(): An array of more than one dimension was input, ignoring all other dimensions except the first one."), TEXT("NCS Debug"), MB_OK|MB_TOPMOST);
					}
#endif																																			/**[01]**/
					nStatus = CNCSRenderer::ReadLineBGR(pByteArrayData ? pByteArrayData : pArrayData);
					break;
				case NCS_READ_BIL:
					hr = E_NOTIMPL;
					break;
				default:
					hr = E_NOTIMPL;
					break;
			}

			if(pByteArrayData) {
				INT32 i;
				for(i = 0; i < MIN(m_nSetViewWidth * 3 * m_nNumberOfBands, nSize); i++) {
					((VARIANT*)pArrayData)[i].bVal = pByteArrayData[i];
				}
				NCSFree(pByteArrayData);
			}

			// Unlock the array
			SafeArrayUnaccessData(pRGBTripletArray);

			if (nStatus != NCSECW_READ_OK) {
				hr = E_FAIL;
			}
		}
		else {
			if (hr == E_INVALIDARG) {
				return Error(_T("ReadLineRGB(): The argument passed was not a valid SAFEARRAY descriptor."));		/**[01]**/
			}
			else if (hr == E_UNEXPECTED) {
				return Error(_T("ReadLineRGB(): The array could not be locked."));									/**[01]**/
			}
			else {
				return Error(_T("ReadLineRGB(): Unkown error."));													/**[01]**/
			}
		}
	} else if(pRGBTriplets->vt & VT_BSTR) {
		// JavaScript passes arrays as a comma separated BSTR
//		_variant_t vtArray (pRGBTriplets);
//		_bstr_t pBStr (vtArray);
		//char *pString = (char*)pBStr;
		
		pArrayData = (UINT8*)NCSMalloc(m_nSetViewWidth * sizeof(UINT8) * m_nNumberOfBands, FALSE);

		if (pArrayData) {

			NCSEcwReadStatus nStatus = NCSECW_READ_OK;

			switch (nType) {
				case NCS_READ_RGB : 
					nStatus = CNCSRenderer::ReadLineRGB(pArrayData);
					break;
				case NCS_READ_BGR:
					nStatus = CNCSRenderer::ReadLineBGR(pArrayData);
					break;
				case NCS_READ_BIL:
					hr = E_NOTIMPL;
					break;
				default:
					hr = E_NOTIMPL;
					break;
			}
			if (nStatus != NCSECW_READ_OK) {
				hr = E_FAIL;
			} else {
				USES_CONVERSION;
				_bstr_t	bStr (OS_STRING(""));
				UINT32 w;

				for(w = 0; w < m_nSetViewWidth * sizeof(UINT8) * m_nNumberOfBands; w++) {
					char buf[1024];

					sprintf(buf, "%3ld", (UINT32)pArrayData[w]);

					if(w > 0) {
						bStr += OS_STRING(",");
					}
					bStr += OS_STRING(buf);
				}

				if(!SysReAllocString(&(pRGBTriplets->bstrVal), (wchar_t*)bStr)) {
					hr = E_FAIL;
				}
			}
			NCSFree(pArrayData);
		}
	} else {
		hr = E_INVALIDARG;
	}
	return hr;
}

STDMETHODIMP CComNCSRenderer::ReadLineRGB(VARIANTARG* pRGBTriplets)
{
	return ReadLine(pRGBTriplets, NCS_READ_RGB);
}

STDMETHODIMP CComNCSRenderer::ReadLineBGR(VARIANTARG* pRGBTriplets)
{
	return ReadLine(pRGBTriplets, NCS_READ_BGR);
}

STDMETHODIMP CComNCSRenderer::ReadLineBIL(VARIANTARG* pLineArray)
{
	return ReadLine(pLineArray, NCS_READ_BIL);
}

STDMETHODIMP CComNCSRenderer::WriteJPG(BSTR OutputFile, long Quality, VARIANT_BOOL bWriteWorldFile)
{
	NCSError nError;
	USES_CONVERSION;

#if defined(_WIN32_WCE)
	return Error(_T("WriteJPG(): Not supported."));
	return S_OK;
#endif

	nError = WriteJPEG(OLE2A(OutputFile), Quality);
	if (nError != NCS_SUCCESS) {
		const char *pError = NCSGetLastErrorText(nError);
#if !defined(_WIN32_WCE)
		return Error(A2OLE(pError));
#else
		return Error(OS_STRING(pError));
#endif
	}
	if (bWriteWorldFile) {
		if (WriteWorldFile(OLE2A(OutputFile)) != NCS_SUCCESS){
#if !defined(_WIN32_WCE)
			return Error(A2OLE(NCSGetLastErrorText(NCSGetLastErrorNum())));
#else
			return Error(OS_STRING(NCSGetLastErrorText(NCSGetLastErrorNum())));
#endif
		}
	}
	return S_OK;
}

/**
 * Draw the NCSRenderer ActiveX control.
 * The NCSRenderer control is a design time ActiveX control that is invisible at runtime.
 * This function is only ever called at design time, for example in developer studio or visual basic.
 * @param ATL_DRAWINFO A structure containing the dc and clip rect
 * @see DrawTransparentBitmap() 
 * @return HRESULT
 */
HRESULT CComNCSRenderer::OnDraw(ATL_DRAWINFO& di)
{
	USES_CONVERSION;
	RECT& rc = *(RECT*)di.prcBounds;
	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
#if !defined(_WIN32_WCE)
	HFONT hOldFont = (HFONT)SelectObject(di.hdcDraw, GetStockObject(ANSI_VAR_FONT));
#else
	HFONT hOldFont = (HFONT)SelectObject(di.hdcDraw, GetStockObject(SYSTEM_FONT));
#endif

	LPCTSTR pRenderString = TEXT("NCSRenderer v") TEXT(NCS_SERVER_VERSION_STRING);

#if !defined(_WIN32_WCE)
	SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
//	pszText = _TEXT(pRenderString);
	TextOut(di.hdcDraw, 
		(rc.left + rc.right) / 2, 
		//(rc.top + rc.bottom) /2, 
		rc.top + 20,
		pRenderString, 
		(int)_tcslen(pRenderString));
#else
//	pszText = OS_STRING(pRenderString);
	ExtTextOut(di.hdcDraw,
			   (rc.left + rc.right) / 2,
			   rc.top + 20,
			   ETO_CLIPPED | ETO_OPAQUE,
			   NULL,
			   pRenderString,
			   _tcslen(pRenderString),
			   NULL);
#endif

	if (m_hECWIcon) {
		// Draw the bitmap with transparent color as white, the bitmap is 32x32.
		DrawTransparentBitmap(di.hdcDraw, m_hECWIcon, 
			(short)((rc.left + rc.right) / 2) - 16, 
			(short)((rc.top + rc.bottom) / 2) - 4,
			RGB(189, 189, 198));			/*0x00FFFFFF*/
	}

	SelectObject(di.hdcDraw, hOldFont);

	// Draw a rectangle around the plugin
#if !defined(_WIN32_WCE)
	LOGBRUSH brush;
	brush.lbStyle = BS_HOLLOW;
	HBRUSH hBrush = CreateBrushIndirect(&brush);
#else
	HBRUSH hBrush = (HBRUSH) GetStockObject(HOLLOW_BRUSH); 
#endif
	HPEN hPen = CreatePen( PS_SOLID, 1, RGB(0,0,0));
	HPEN hOldPen = (HPEN)SelectObject(di.hdcDraw, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(di.hdcDraw, hBrush);

	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(di.hdcDraw, hOldBrush);
	SelectObject(di.hdcDraw, hOldPen);
	DeleteObject(hOldBrush);
	DeleteObject(hPen);

	return S_OK;
}

/**
 * Draw a transparent bitmap to the device context.
 * This in a private function, the source was ripped directly from MSDN.
 * @param hdc The destination device context to draw to.
 * @param hBitmap A valid HBITMAP, created for example from LoadImage(), or CreateBitmap().
 * @param xStart X offset to start drawing
 * @param yStart Y offset to start drawing
 * @param cTransparentColor A color ref describing the transparent color.
 * @see OnDraw() 
 * @return void
 */
void CComNCSRenderer::DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Select the bitmap

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Get width of bitmap
	ptSize.y = bm.bmHeight;           // Get height of bitmap
#if !defined(_WIN32_WCE)	//In CE window and viewport origins or extents can't be changed
	DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device
#endif									 // to logical points

	// Create some DCs to hold temporary data.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);

	// Create a bitmap for each DC. DCs are required for a number of
	// GDI functions.

	// Monochrome DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	// Monochrome DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// Each DC must select a bitmap object to store pixel data.
	bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);

#if !defined(_WIN32_WCE) 
	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));
#endif
	// Save the bitmap sent here, because it will be overwritten.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC to the color.
	// contained in the parts of the bitmap that should be transparent
	cColor = SetBkColor(hdcTemp, cTransparentColor);

	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
		  SRCCOPY);

	// Set the background color of the source DC back to the original
	// color.
	SetBkColor(hdcTemp, cColor);

	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
		  NOTSRCCOPY);

	// Copy the background of the main DC to the destination.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
		  SRCCOPY);

	// Mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// Mask out the transparent colored pixels on the bitmap.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// XOR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	// Copy the destination to the screen.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
		  SRCCOPY);

	// Place the original bitmap back into the bitmap sent here.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Delete the memory bitmaps.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	// Delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}



STDMETHODIMP CComNCSRenderer::GetPercentComplete(long *pVal)
{
	*pVal = CNCSFile::GetPercentComplete();
	return S_OK;
}

STDMETHODIMP CComNCSRenderer::ConvertWorldToDataset(double dWorldX, double dWorldY, long *pDatasetX, long *pDatasetY)
{
	USES_CONVERSION;
	NCSError err = CNCSFile::ConvertWorldToDataset(dWorldX, dWorldY, (int*)pDatasetX, (int*)pDatasetY);
	if (err != NCS_SUCCESS)
#if !defined(_WIN32_WCE)
		return Error(A2OLE(CNCSFile::FormatErrorText(err)));
#else
		return Error(OS_STRING(CNCSFile::FormatErrorText(err)));
#endif

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::ConvertWorldToDatasetVB(double dWorldX, double dWorldY, VARIANT *pVal)
{
	USES_CONVERSION;
	INT32 nDatasetX, nDatasetY;
	NCSError err = CNCSFile::ConvertWorldToDataset(dWorldX, dWorldY, &nDatasetX, &nDatasetY);
	if (err != NCS_SUCCESS)
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_I4; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {0, 0};
		SAFEARRAY *psa = SafeArrayCreate(VT_I4, 1, bounds);
		pVal->parray = psa;

#if !defined(_WIN32_WCE)
		return Error(A2OLE(CNCSFile::FormatErrorText(err)));
#else
		return Error(OS_STRING(CNCSFile::FormatErrorText(err)));
#endif
	}
	else
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_I4; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {2, 0};

		SAFEARRAY *psa = SafeArrayCreate(VT_I4, 1, bounds);
		if (psa)
		{
			 long *data;
			 SafeArrayAccessData(psa, (void**)&data);

			 data[0] = nDatasetX;
			 data[1] = nDatasetY;

			 SafeArrayUnaccessData(psa);
		}
		pVal->parray = psa;
	}

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::ConvertDatasetToWorld(long nDatasetX, long nDatasetY, double *pWorldX, double *pWorldY)
{
	USES_CONVERSION;
	NCSError err = CNCSFile::ConvertDatasetToWorld(nDatasetX, nDatasetY, pWorldX, pWorldY);
	if (err != NCS_SUCCESS)
#if !defined(_WIN32_WCE)
		return Error(A2OLE(CNCSFile::FormatErrorText(err)));
#else
		return Error(OS_STRING(CNCSFile::FormatErrorText(err)));
#endif

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::ConvertDatasetToWorldVB(long nDatasetX, long nDatasetY, VARIANT *pVal)
{
	USES_CONVERSION;
	IEEE8 dWorldX, dWorldY;
	NCSError err = CNCSFile::ConvertDatasetToWorld(nDatasetX, nDatasetY, &dWorldX, &dWorldY);
	if (err != NCS_SUCCESS)
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_R8; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {0, 0};
		SAFEARRAY *psa = SafeArrayCreate(VT_R8, 1, bounds);
		pVal->parray = psa;

#if !defined(_WIN32_WCE)
		return Error(A2OLE(CNCSFile::FormatErrorText(err)));
#else
		return Error(OS_STRING(CNCSFile::FormatErrorText(err)));
#endif
	}
	else
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_R8; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {2, 0};

		SAFEARRAY *psa = SafeArrayCreate(VT_R8, 1, bounds);
		if (psa)
		{
			 IEEE8 *data;
			 SafeArrayAccessData(psa, (void**)&data);

			 data[0] = dWorldX;
			 data[1] = dWorldY;

			 SafeArrayUnaccessData(psa);
		}
		pVal->parray = psa;
	}

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::FormatErrorText(long nErrorNumber, BSTR *pVal)
{
	USES_CONVERSION;
	*pVal = NULL;
	const char * pError = NCSGetLastErrorText((NCSError)nErrorNumber);
	
	if (pError)
	{
		*pVal = ::SysAllocString(A2OLE(pError));
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBands(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nBands;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoTopX(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nTopX;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoLeftY(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nLeftY;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBottomX(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nBottomX;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoRightY(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nRightY;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWindowSizeX(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nSizeX;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWindowSizeY(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nSizeY;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBlocksInView(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nBlocksInView;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBlocksAvailable(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nBlocksAvailable;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBlocksAvailableAtSetView(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nBlocksAvailableAtSetView;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoMissedBlocksDuringRead(long *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->nMissedBlocksDuringRead;
	else
		*pVal = 0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWorldTopX(double *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->fTopX;
	else
		*pVal = 0.0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWorldLeftY(double *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->fLeftY;
	else
		*pVal = 0.0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWorldBottomX(double *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->fBottomX;
	else
		*pVal = 0.0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoWorldRightY(double *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
		*pVal = pNCSFileViewSetInfo->fRightY;
	else
		*pVal = 0.0;

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ViewSetInfoBandList(VARIANT *pVal)
{
	NCSFileViewSetInfo *pNCSFileViewSetInfo = GetFileViewSetInfo();
	if (pNCSFileViewSetInfo)
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_I4; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {pNCSFileViewSetInfo->nBands, 0};

		SAFEARRAY *psa = SafeArrayCreate(VT_I4, 1, bounds);
		if (psa)
		{
			 long *data;
			 SafeArrayAccessData(psa, (void**)&data);

			 for (int i = 0; i < (int) pNCSFileViewSetInfo->nBands; i++)
			   data[i] = pNCSFileViewSetInfo->pBandList[i];

			 SafeArrayUnaccessData(psa);
		}
		pVal->parray = psa;
	}
	else
	{
		VariantInit(pVal);
		pVal->vt = VT_ARRAY | VT_I4; //variant contains an array of integers
		SAFEARRAYBOUND bounds[] = {0, 0};
		SAFEARRAY *psa = SafeArrayCreate(VT_I4, 1, bounds);
		pVal->parray = psa;
	}

	return S_OK;
}

STDMETHODIMP CComNCSRenderer::get_ImageHeight(long *pVal)			/**[03]**/
{
	return get_Height(pVal);
}

STDMETHODIMP CComNCSRenderer::get_ImageWidth(long *pVal)			/**[03]**/
{
	return get_Width(pVal);
}


