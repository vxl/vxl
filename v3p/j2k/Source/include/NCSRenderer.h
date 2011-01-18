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
** FILE:   	NCSRenderer.c
** CREATED:	12 Jan 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	C++ class wrappers for the ECW library
** EDITS:
**
** [01] 07Dec00 jmp 
** [02] 12Feb01 jmp 
*******************************************************/
/** @file NCSRenderer.h */

//#if !defined(NCSRENDERER_H)
#ifndef NCSRENDERER_H
#define NCSRENDERER_H

#define NCS_HIST_AND_LUT_SUPPORT

#if defined(WIN32) || defined(_WIN32_WCE)
	#define DIB_FIX		//only define for Release with IWS 2.0
#endif //WIN32

#include "NCSDefs.h"
#include "NCSFile.h"
#include "NCSMutex.h"

#ifdef WIN32
	#include "NCSDynamicLib.h"
#endif

#if defined(MACINTOSH)
	#include <Quickdraw.h>
	#define COLORREF DWORD
	#define RECT Rect
	#define LPRECT Rect *
	#define HDC CGrafPtr
#endif

/** @class CNCSRenderer
 *	@brief This class inherits from CNCSFile and provides display routines for a Win32 environment.
 *
 *	CNCSRenderer uses methods that take a Windows device context (HDC) argument
 *	to print and display views from an ECW file in either blocking or progressive mode.
 *	It is the easiest way to use the ECW JPEG 2000 SDK to get interactive ECW imagery 
 *	display into a native Windows C++ application.
 */
class NCS_EXPORT CNCSRenderer : public CNCSFile  
{
public:
	/**
	 *	Constructor.
	 */
	CNCSRenderer();
	/**
	 *	Destructor.
	 */
	virtual ~CNCSRenderer();

	inline bool HasAlpha()
	{
		int nAlphaBand=0;
		return HasAlpha( nAlphaBand );
	}
	bool HasAlpha( int &nAlphaBand );
	bool isUsingAlphaBand()
	{
		return m_bUsingAlphaBand;
	}

	/**	
	 *	Set the view on the open file.  This version takes world coordinates as input.
	 *
	 *	@param[in]	nBands				The number of bands to include in the view being set.
	 *	@param[in]	pBandList			An array of band indices specifying which bands to include and in which order.
	 *	@param[in]	nWidth				The width of the view to construct in dataset cells.
	 *	@param[in]	nHeight				The height of the view to construct in dataset cells.
	 *	@param[in]	dWorldTLX			The left of the view to construct in world coordinates.
	 *	@param[in]	dWorldTLY			The top of the view to construct in world coordinates.
	 *	@param[in]	dWorldBRX			The right of the view to construct in world coordinates.
	 *	@param[in]	dWorldBRY			The bottom of the view to construct in world coordinates.
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError SetView(  INT32 nBands, INT32 *pBandList, 
					   INT32 nWidth, INT32 nHeight,
					   IEEE8 dWorldTLX, IEEE8 dWorldTLY,
					   IEEE8 dWorldBRX, IEEE8 dWorldBRY );
	/**	
	 *	Set the view on the open file.  This version takes dataset coordinates as input.
	 *
	 *	@param[in]	nBands				The number of bands to include in the view being set.
	 *	@param[in]	pBandList			An array of band indices specifying which bands to include and in which order.
	 *	@param[in]	nWidth				The width of the view to construct in dataset cells.
	 *	@param[in]	nHeight				The height of the view to construct in dataset cells.
	 *	@param[in]	dDatasetTLX			The left of the view to construct in dataset coordinates.
	 *	@param[in]	dDatasetTLY			The top of the view to construct in dataset coordinates.
	 *	@param[in]	dDatasetBRX			The right of the view to construct in dataset coordinates.
	 *	@param[in]	dDatasetBRY			The bottom of the view to construct in dataset coordinates.
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError SetView(  INT32 nBands, INT32 *pBandList, 
					   INT32 nWidth, INT32 nHeight,
					   INT32 nDatasetTLX, INT32 nDatasetTLY,
					   INT32 nDatasetBRX, INT32 nDatasetBRY );
	virtual NCSEcwReadStatus ReadLineBIL (UINT8 **ppOutputLine);
	virtual NCSEcwReadStatus ReadLineRGB (UINT8 *pRGBTriplet);
	virtual NCSEcwReadStatus ReadLineBGR (UINT8 *pRGBTriplet);

#if defined( LINUX ) || defined( SOLARIS )
#else	
	NCSError ReadImage( NCSFileViewSetInfo *pViewSetInfo);
	NCSError ReadImage( INT32 nWidth, INT32 nHeight );
	NCSError ReadImage( IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY, INT32 nDatasetTLX, INT32 nDatasetTLY, INT32 nDatasetBRX, INT32 nDatasetBRY, INT32 nWidth, INT32 nHeight);

	NCSError DrawImage( HDC DeviceContext, LPRECT pClipRect, IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY );

	/** 
	 *	Sets the background color of the display area.  In non-transparent mode, this 
	 *	color will be drawn to the background before the image.
	 *
	 *	@param[in]	nBackgroundColor	COLORREF value specifying the desired color
	 */
	void SetBackgroundColor( COLORREF nBackgroundColor );
#endif

	/**
	 *	Specifies whether the renderer is responsible for doing a background fill
	 *	before drawing imagery.  If the renderer is being used in an application 
	 *	that contains other image layers, the transparency mode should be set to 
	 *	FALSE and the application should do the work of managing the display.
	 *	If the renderer is incorporated into a single-layered control then it is 
	 *	appropriate to set the transparency mode to TRUE to reduce the amount of 
	 *	work required from the renderer's container.
	 *
	 *	@param[in]	bTransparent		BOOLEAN value specifying whether of not to draw the image transparently
	 */
	void SetTransparent( BOOLEAN bTrasnparent );
	/**
	 *	Obtain the current transparency status from the renderer.
	 *
	 *	@param[out]	pbTransparent		BOOLEAN buffer for the returned transparency status
	 */
	void GetTransparent( BOOLEAN *pbTransparent );
	/**
	 *	Writes the current view to a JPEG file.  This function can only be called successfully
	 *	if the current view has been opened on an ECW or JPEG 2000 file in non-progressive
	 *	mode.
	 *
	 *	@param[in]	pFilename			(char *) ASCII string specifying the output filename
	 *	@param[in]	nQuality			Desired quality of the output JPEG file
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError WriteJPEG(char *pFilename, INT32 nQuality);
	/**
	 *	Writes a JPEG file based on the current view, and stores it in a buffer that 
	 *	can be output to file later, or used for some other purpose.  This function can only
	 *	be called successfully if the current view has been opened on an ECW or JPEG 2000 file 
	 *	in non-progressive mode.
	 *
	 *	@param[in]	pFilename			(char *) ASCII string specifying the output filename
	 *	@param[in]	nQuality			Desired quality of the output JPEG file
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError WriteJPEG(UINT8 **ppBuffer, UINT32 *pBufferLength, INT32 nQuality);
	/**
	 *	This call is used to free the JPEG buffer returned by a call to 
	 *	CNCSRenderer::WriteJPEG(UINT8 **ppBuffer, UINT32 *pBufferLength, INT32 nQuality).
	 *
	 *	@param[in]	pBuffer				The JPEG buffer previously returned that must now be freed
	 */
	static void FreeJPEGBuffer(UINT8 *pBuffer);
	/**
	 *	This call is used to write a world file containing the georeferenceing information for 
	 *	the current view.  The world file written is given the same name as the input filename,
	 *	excepting that its extension is constructed from the first and third letters of the extension
	 *	of the input + "w".  For example, ".jpg" becomes ".jgw" and ".tif" becomes ".tfw".
	 *
	 *	@param[in]	pFilename			The filename on which to base the output world filename.
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError WriteWorldFile(char *pFilename);
	static void ShutDown( void );
#ifdef NCS_HIST_AND_LUT_SUPPORT
	BOOLEAN CalcHistograms(BOOLEAN bEnable);
	BOOLEAN	GetHistogram(INT32 nBand, UINT32 Histogram[256]);
	
	BOOLEAN ApplyLUTs(BOOLEAN bEnable);
	BOOLEAN	SetLUT(INT32 nBand, UINT8 Lut[256]);
#endif // NCS_HIST_AND_LUT_SUPPORT

protected:

#if defined( LINUX ) || defined( SOLARIS )
#else

	void DrawingExtents(LPRECT pClipRect,	
					    IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY,
						LPRECT pNewClipRect);	/**[01]**/
#if defined( DIB_FIX ) || defined(MACINTOSH) || defined(MACOSX)
	BOOLEAN	m_bCreateNewDIB;	// need to recreate DIB	/**[02]**/
#endif
private:	
#ifdef WIN32	
	BOOLEAN CreateDIBAndPallete( HDC hDeviceContext, INT32 nWidth, INT32 nHeight );
	BOOLEAN DestroyDibAndPalette( void );
#elif defined(MACINTOSH) || defined(MACOSX)
	BOOLEAN CreatePixMapAndPallete( GrafPtr pGPtr, INT32 nWidth, INT32 nHeight );
	BOOLEAN DestroyPixMapAndPallete( void );
	GDHandle GetWindowDevice (WindowPtr pWindow);
#else
	#error unknown machine type
#endif

#endif //LINUX

	NCSError AdjustExtents( INT32 nWidth, INT32 nHeight,
						    INT32 nDatasetTLX, INT32 nDatasetTLY, INT32 nDatatasetBRX, INT32 DatasetBRY,
						    INT32 *pnAdjustedDatasetTLX, INT32 *pnAdjustedDatasetTLY, INT32 *pnAdjustedDatatasetBRX, INT32 *pnAdjustedDatasetBRY,
						    INT32 *pnAdjustedDeviceTLX, INT32 *pnAdjustedDeviceTLY, INT32 *pnAdjustedDeviceBRX, INT32 *pnAdjustedDeviceBRY );
	NCSError AdjustExtents( INT32 nWidth, INT32 nHeight, 
						    IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY, 
						    IEEE8 *pdAdjustedWorldTLX, IEEE8 *pdAdjustedWorldTLY, IEEE8 *pdAdjustedWorldBRX, IEEE8 *pdAdjustedWorldBRY,
						    INT32 *pnAdjustedDeviceTLX, INT32 *pnAdjustedDeviceTLY, INT32 *pnAdjustedDeviceBRX, INT32 *pnAdjustedDeviceBRY );

	void CalculateDeviceCoords(INT32 nTopLeftX, INT32 nTopLeftY,INT32 nBottomRightX, INT32 nBottomRightY,
						IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY,
						IEEE8 dNewWorldTLX, IEEE8 dNewWorldTLY, IEEE8 dNewWorldBRX, IEEE8 dNewWorldBRY,
						INT32 *pnDeviceTLX, INT32 *pnDeviceTLY,	INT32 *pnDeviceBRX, INT32 *pnDeviceBRY);

	BOOLEAN CalcStretchBltCoordinates(INT32 nViewWidth, INT32 nViewHeight,
									  IEEE8 dTLX, IEEE8 dTLY, IEEE8 dBRX, IEEE8 dBRY,
									  IEEE8 outputDeviceCoords[4], IEEE8 outputImageCoords[4]);
////
	void calculateDeviceCoords(int nDeviceTLX, int nDeviceTLY,
							   int nDeviceBRX, int nDeviceBRY,
							   double dWorldTLX, double dWorldTLY,
							   double dWorldBRX, double dWorldBRY,
							   double outputDeviceCoords[4],
							   double dRendererWorldTLX, double dRendererWorldTLY,
							   double dRendererWorldBRX, double dRendererWorldBRY);
	void calculateImageCoords(double dDevice1TLX, double dDevice1TLY, double dDevice1BRX, double dDevice1BRY,
                              double dImageWidth, double dImageHeight,
                              double dDevice2TLX, double dDevice2TLY, double dDevice2BRX, double dDevice2BRY,
							  double outputImageCoords[4]);
////

	NCSError WriteJPEG(char *pFilename, UINT8 **ppBuffer, UINT32 *pBufferLength, INT32 nQuality);
#ifdef NCS_HIST_AND_LUT_SUPPORT
	BOOLEAN SetupHistograms(void);
#endif // NCS_HIST_AND_LUT_SUPPORT
	// These are the adjusted extents for when the set view is outside the range of the data
	INT32 m_nAdjustedViewWidth;
	INT32 m_nAdjustedViewHeight;
	INT32 m_nAdjustedXOffset;
	INT32 m_nAdjustedYOffset;
	IEEE8 m_dAdjustedWorldTLX;
	IEEE8 m_dAdjustedWorldTLY;
	IEEE8 m_dAdjustedWorldBRX;
	IEEE8 m_dAdjustedWorldBRY;

	// This is the actual extents of the renderer.
	INT32 m_nRendererWidth;
	INT32 m_nRendererHeight;
	INT32 m_nRendererXOffset;
	INT32 m_nRendererYOffset;	
	INT32 m_nRendererDatasetTLX;
	INT32 m_nRendererDatasetTLY;
	INT32 m_nRendererDatasetBRX;
	INT32 m_nRendererDatasetBRY;
	IEEE8 m_dRendererWorldTLX;
	IEEE8 m_dRendererWorldTLY;
	IEEE8 m_dRendererWorldBRX;
	IEEE8 m_dRendererWorldBRY;

#ifdef WIN32
	HBITMAP m_hBitmap;
	BITMAPINFO *m_pbmInfo;
	UINT8 *m_pColorTable;
	PALETTEENTRY *m_pPaletteEntries;
	HINSTANCE	m_hOpenGLDLL;	
	UINT8 *m_pBitmapImage;
#elif defined(MACINTOSH) || defined(MACOSX)
	//PixMapHandle m_hPixMap;
	GWorldPtr m_hLocalMemDC;
	//CTabHandle m_hCTable;
#endif
	UINT8 *m_pRGBA;
	UINT8 *m_pRGBALocal;
	bool m_bUsingAlphaBand;
	bool UsingAlphaBand();
	NCS_FUNCADDR m_pAlphaBlend;
	NCS_DLHANDLE m_hMSImg32DLL;
	
	bool StretchBlt(
		HDC hdcDest,      // handle to destination DC
		int nXOriginDest, // x-coord of destination upper-left corner
		int nYOriginDest, // y-coord of destination upper-left corner
		int nWidthDest,   // width of destination rectangle
		int nHeightDest,  // height of destination rectangle
		HDC hdcSrc,       // handle to source DC
		int nXOriginSrc,  // x-coord of source upper-left corner
		int nYOriginSrc,  // y-coord of source upper-left corner
		int nWidthSrc,    // width of source rectangle
		int nHeightSrc   // height of source rectangle
	);

	INT32 m_nDCWidth;
	INT32 m_nDCHeight;
	INT32 m_nDCBitDepth;
	INT32 m_nBytesPerLine;
	BOOLEAN m_bHaveInit;
#if defined( LINUX ) || defined( SOLARIS )
#else	
	COLORREF m_nBackgroundColor;
#endif
	BOOLEAN  m_bIsTransparent;
	NCSMutex m_DrawMutex;
	NCSFileViewSetInfo *pCurrentViewSetInfo;
	BOOLEAN m_bHaveReadImage;
	BOOLEAN m_bAlternateDraw;
#ifdef NCS_HIST_AND_LUT_SUPPORT
	BOOLEAN m_bCalcHistograms;
	INT32	m_nReadLine;
	typedef UINT32 Histogram[256];
	Histogram *m_pHistograms;
	NCSMutex m_HistogramMutex;
	
	BOOLEAN m_bApplyLUTs;
	BOOLEAN m_bLutChanged;
	UINT8 m_LUTs[3][256];
#endif //NCS_HIST_AND_LUT_SUPPORT
};

#endif // !defined(NCSRENDERER_H)
