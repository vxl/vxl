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
** [01] 14Jan00 mjs DIB code ripped from NCSDisplayEngineRWin32.cpp
** [02] 13Jun00 sjc Fixed crash with <8 bit DCs
** [03] 05Jul00 sjc Fixed OpenGL preload hack
** [04] 08Jul00 sjc Fixed memory leak
** [05] 08Sep00 sjc Fixed potential error problem - FormatErrorText() takes varargs, we don't have them...
** [06] 06Oct00 sjc Added fallback case for bad Blt/Stretch in drivers
** [07] 22Nov00 jmp Fix: m_nDCWidth, m_nDCHeight & m_nDCBitDepth where not being initialized, causing strange things to happen...
** [08] 22Nov00 jmp Fixed calculation for an adjusted world Y coord in the AdjustExtents function.
** [09] 07Dec00 jmp Added DrawingExtents function, used by NCSView in OnErasebackground function.
** [10] 12Feb01 jmp Check if need to recreate DIB	//will include for IWS V2.0
** [11] 13Feb01 jmp Returning an error from ReadImage and DrawImage, orig return was NCS_SUCCESS
** [12] 13Feb01 jmp Replacing MessageBox's with NCS errors, MessageBox's only in _DEBUG build. 
** [13] 16May01 jmp width and height passed to CNCSFile::SetView out by 1, as Dataset units calculated use inclusive points.
** [14] 05Jun01 jmp non-transparent case now working, exclude clip extents where previously wrong
** [15] 24Jan02 sjc Fixed UMR from uninitialised members
** [16] 17Jun02 jmp Added code to do a Blt instead of a Stretch where possible.
** [17] 17Jun02 rar Changed SetView to pass the error NCS_FILEIO_ERROR on.
** [18] 15May04 rar Removed changed 13 above. Don't know why it was initially done.
*******************************************************/


#ifdef WIN32
	//#define DIB_FIX		//only define for Release with IWS 2.0
	#define _WIN32_WINNT 0x0500	// For MB_SERVICE_NOTIFICATION
#elif defined(macintosh) // must be lower-case macintosh as this is defined by the compiler
//	#include "CarbonPrefix.h"
#endif //WIN32

#include "NCSRenderer.h"
#include "NCSFile.h"
#include "NCSMalloc.h"
#include "NCSMutex.h"
#include "NCSThread.h"
#include "NCSPrefs.h"
#include <stdio.h>

#if defined(MACINTOSH)
	#include <string.h>
	#include <Quickdraw.h>
	#include <MacMemory.h>
	
#if TARGET_API_MAC_CARBON
#else
#	define GetPortPixMap( _A ) ((CGrafPtr)_A)->portPixMap
#	define GetPortBounds( _A, _B ) (Rect *)memcpy( _B, (&((CGrafPtr)_A)->portRect), sizeof(RECT) )
#	define GetPortBitMapForCopyBits( _A ) &((GrafPtr)_A)->portBits
#endif

#endif //MACINTOSH


/*! \class CNCSRenderer
 *  \brief An object that renders ecw imagery to a device context.
 *
 *  This class directly inherits from NCSFile class to provide a
 *  mechanism to render ecw imagery into a device context. It takes
 *	a simple SetExtents() call and adjusts the extents accordingly and
 *  draws imagery. The extents do not have to lie within the boundary
 *  of the dataset (as in the NCSFile class). It will clip and draw
 *  intersection regions accordingly. It can be transparent or opaque.
 *  In opaque mode you can set the background color.
 *  \author Mark Sheridan
 *  \date    12 Jan 2000
 *  \par Edits:
 *
 */

#define NCS_WIDTH_BYTES(bytes) (bytes+((bytes%4)?(4-bytes%4):0))

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

#define IS_BETWEEN(a, b, c) (((a>=b)&&(a<=c))||((a>=c)&&(a<=b)))

#ifndef NCSDISPLAYENGINERWIN32_H
/*
** 4x4 Kernel Dither Macros & Defines
*/
#define _MR  32
#define _MG  32
#define _MB  32

#define _DX 4
#define _DY 4
#define _D  (_DX*_DY)
#define MAXC    256

#define _MIX(r,g,b) (((r)*_MG+(g))*_MB+(b))
#define _DITH(C,c,d)    (((unsigned)((_D*(C-1)+1)*c+d)) >> 12)

static UINT16 kernel8[_DY*_DX] = {
	0 * MAXC,  8 * MAXC,  2 * MAXC, 10 * MAXC,
	12 * MAXC,  4 * MAXC, 14 * MAXC,  6 * MAXC,
	3 * MAXC, 11 * MAXC,  1 * MAXC,  9 * MAXC,
	15 * MAXC,  7 * MAXC, 13 * MAXC,  5 * MAXC,
};
static UINT16 __d;

#define KERNEL_DITHER( X, Y, R, G, B )                     \
		(__d = kernel8[(((Y)&3)<<2) | ((X)&3)],             \
			m_pColorTable[_MIX(_DITH(_MR, (R), __d),    \
			_DITH(_MG, (G), __d),    \
			_DITH(_MB, (B), __d))])

#endif // !defined(NCSDISPLAYENGINERWIN32_H)


//BOOLEAN inline IntersectRects(INT32 tlx1, INT32 tly1, INT32 brx1, INT32 bry1, INT32 tlx2, INT32 tly2, INT32 brx2, INT32 bry2)
BOOLEAN IntersectRects(INT32 tlx1, INT32 tly1, INT32 brx1, INT32 bry1, INT32 tlx2, INT32 tly2, INT32 brx2, INT32 bry2)
{
#ifdef WIN32
	RECT DestRect;
	RECT Rect1;
	RECT Rect2;

	Rect1.left = tlx1;
	Rect1.right = brx1;
	Rect1.top = tly1;
	Rect1.bottom = bry1;
	Rect2.left = tlx2;
	Rect2.right = brx2;
	Rect2.top = tly2;
	Rect2.bottom = bry2;

	return (BOOLEAN)IntersectRect(&DestRect, &Rect1, &Rect2);
#elif defined(POSIX)
	if( (tlx2 >= tlx1) && (tlx2 <= brx1) && (tly2 >= tly1) && (tly2 <= bry1) ) {
		return TRUE;
	}

	if( (brx2 >= tlx1) && (brx2 <= brx1) && (tly2 >= tly1) && (tly2 <= bry1) ) {
		return TRUE;
	}

	if( (brx2 >= tlx1) && (brx2 <= brx1) && (bry2 >= tly1) && (bry2 <= bry1) ) {
		return TRUE;
	}

	if( (tlx2 >= tlx1) && (tlx2 <= brx1) && (bry2 >= tly1) && (bry2 <= bry1) ) {
		return TRUE;
	}

	if( (tlx1 >= tlx2) && (brx1 <= brx2) && (bry1 <= bry2) && (tly1 >= tly2) ) {
		return TRUE;
	}


	return FALSE;
#else
#	error IntersectRects not implemented for platform
#endif //WIN32
}

/**
 * Constructor.
 * Create a non transparent NCSRenderer object, set the background color to the system default.
 * @see ~CNCSRenderer()
 */
CNCSRenderer::CNCSRenderer()
{
#ifdef WIN32
	m_hBitmap = NULL;
	m_pbmInfo = (BITMAPINFO*)NULL;
	m_pPaletteEntries = NULL;
	m_pColorTable = NULL;
	m_nBackgroundColor = GetSysColor(COLOR_3DFACE);
	m_hOpenGLDLL = NULL;		/**[03]**/
#elif defined(MACINTOSH) || defined(MACOSX)
	//m_hPixMap = NULL;
	m_hLocalMemDC = NULL;
	//m_hCTable = NULL;
#endif //WIN32
	m_pRGBA = NULL;
	m_pRGBALocal = NULL;
	m_bUsingAlphaBand = false;
	m_pAlphaBlend = (NCS_FUNCADDR)NULL;
#ifdef WIN32
	m_hMSImg32DLL = NCSDlOpen("Msimg32.DLL");
	if(m_hMSImg32DLL) {
		m_pAlphaBlend = (NCS_FUNCADDR)NCSDlGetFuncAddress(m_hMSImg32DLL, "AlphaBlend");
	}
#endif

	m_nDCWidth = 0;														/**[07]**/
	m_nDCHeight = 0;													/**[07]**/
	m_nDCBitDepth = 0;													/**[07]**/
	m_bHaveInit = FALSE;
	m_bIsTransparent = TRUE;
	pCurrentViewSetInfo = NULL;
	m_bHaveReadImage = FALSE;
#if defined(DIB_FIX)||defined(MACINTOSH) || defined(MACOSX)
	m_bCreateNewDIB = TRUE;												/**[10]**/
#endif

	m_nRendererWidth = 0;
	m_nRendererHeight = 0;
	m_nRendererDatasetTLX = 0;
	m_nRendererDatasetTLY = 0;
	m_nRendererDatasetBRX = 0;
	m_nRendererDatasetBRY = 0;
	m_dRendererWorldTLX = 0.0;	/**[15]**/
	m_dRendererWorldTLY = 0.0;	/**[15]**/
	m_dRendererWorldBRX = 0.0;	/**[15]**/
	m_dRendererWorldBRY = 0.0;	/**[15]**/
	m_dAdjustedWorldTLX = 0;
	m_dAdjustedWorldTLY = 0;
	m_dAdjustedWorldBRX = 0;
	m_dAdjustedWorldBRY = 0;


	m_bAlternateDraw = FALSE;															//[06]

#ifdef WIN32
#if !defined(_WIN32_WCE)
	BOOLEAN bAlternateDraw;																//[06]
	if (NCSPrefGetBoolean("UseAlternateDrawCode", &bAlternateDraw) == NCS_SUCCESS) {	//[06]
		m_bAlternateDraw = bAlternateDraw;												//[06]
	}

	//Under Win98 using IE 5.5 SetDIBBitsToDevice doesn't work properly
	//instead need to use BitBlt. (Use wininet.dll to get the browser version).
	char m_pPath[MAX_PATH + 1 + sizeof("\\wininet.dll")];														
    UINT16 nMajor, nMinor, nRevision, nBuild;									
	if(::GetSystemDirectoryA(m_pPath, MAX_PATH))
	{								
		strcat(m_pPath, "\\wininet.dll");										
		NCSFileGetVersion(m_pPath, &nMajor, &nMinor, &nRevision, &nBuild);		
		if ((nMajor >=5 && nMinor >= 50) && (NCSGetPlatform() == NCS_WINDOWS_9X))
			m_bAlternateDraw = TRUE;

		if ((nMajor >= 6) && (NCSGetPlatform() == NCS_WINDOWS_9X))
			m_bAlternateDraw = TRUE;
	}	
#endif
#endif //WIN32

	NCSMutexInit(&m_DrawMutex);
	NCSMutexInit(&m_HistogramMutex);
#ifdef NCS_HIST_AND_LUT_SUPPORT
	m_bCalcHistograms = false;
	m_nReadLine = 0;
	m_pHistograms = NULL;
	m_bApplyLUTs = false;
	m_bLutChanged = false;
	for(int b = 0; b < 3; b++) {
		m_LUTs[0][b] = b;
		m_LUTs[1][b] = b;
		m_LUTs[2][b] = b;
	}
#endif // NCS_HIST_AND_LUT_SUPPORT
}

/**
 * Destructor.
 * Destroy a NCSRenderer object.
 * @see CNCSRenderer()
 */
CNCSRenderer::~CNCSRenderer()
{
	if (m_pRGBA)
	{
		NCSFree(m_pRGBA);
		m_pRGBA = NULL;
	}

	if (m_pRGBALocal)/**[04]**/
	{
		NCSFree(m_pRGBALocal);	/**[04]**/
		m_pRGBALocal = NULL;
	}
#ifdef WIN32
	DestroyDibAndPalette();

	if(m_hOpenGLDLL) {				/**[03]**/
		FreeLibrary(m_hOpenGLDLL);	/**[03]**/
	}								/**[03]**/
#elif defined(MACINTOSH) || defined(MACOSX)
	DestroyPixMapAndPallete();
#endif
#ifdef NCS_HIST_AND_LUT_SUPPORT
	NCSFree(m_pHistograms);
#endif // NCS_HIST_AND_LUT_SUPPORTf
	NCSMutexFini(&m_DrawMutex);
	NCSMutexFini(&m_HistogramMutex);

#ifdef WIN32
	if(m_hMSImg32DLL)
	{
		NCSDlClose(m_hMSImg32DLL);
		m_hMSImg32DLL = NULL;
	}
#endif //WIN32
}

bool CNCSRenderer::UsingAlphaBand()
{
	if( m_pAlphaBlend && m_pnSetViewBandList && (m_nSetViewNrBands > 2) ) {
		if( strcmp( NCS_BANDDESC_AllOpacity, m_Bands[ m_pnSetViewBandList[m_nSetViewNrBands-1] ].szDesc)==0 ) {
			m_bUsingAlphaBand = true;
			return true;
		}
	}
	m_bUsingAlphaBand = false;
	return false;
}

bool CNCSRenderer::HasAlpha( int &nAlphaBand ) {
	if( (GetFileType() == NCS_FILE_JP2) ) {
		int nNumBands = m_Bands.size();
		for( int i=0; i < nNumBands; i++ ) {
			if( strcmp( NCS_BANDDESC_AllOpacity, m_Bands[i].szDesc)==0 ) {
				nAlphaBand = i;
				return true;
			}
		}
	}
	return false;
}

/**
 * Adjust the extents, using input dataset extents.
 * The renderer object attemps to draw to a bitmap, irrespective of whether the extents
 * are outside the actual dataset extents or not. This method adjusts the extents to be
 * within the dataset and returns the relevant adjustments.
 * @param nWidth The output device view width
 * @param nHeight The output device view height
 * @param nInputDatasetTLX The unadjusted top left X input dataset coordinate
 * @param nInputDatasetTLY The unadjusted top left Y input dataset coordinate
 * @param nInputDatasetBRX The unadjusted bottom right X input dataset coordinate
 * @param nInputDatasetBRY The unadjusted bottom right Y input dataset coordinate
 * @param *pnAdjustedDatasetTLX [OUT] The output adjusted top left X dataset coordinate
 * @param *pnAdjustedDatasetTLY [OUT] The output adjusted top left Y dataset coordinate
 * @param *pnAdjustedDatasetBRX [OUT] The output adjusted bottom right X dataset coordinate
 * @param *pnAdjustedDatasetBRY [OUT] The output adjusted bottom right Y dataset coordinate
 * @param *pnAdjustedDeviceTLX [OUT] The output adjusted top left X device coordinate
 * @param *pnAdjustedDeviceTLY [OUT] The output adjusted top left Y device coordinate
 * @param *pnAdjustedDeviceBRX [OUT] The output adjusted bottom right X device coordinate
 * @param *pnAdjustedDeviceBRY [OUT] The output adjusted bottom right Y device coordinate
 * @see SetView()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::AdjustExtents(INT32 nWidth, INT32 nHeight,
//NCSError inline CNCSRenderer::AdjustExtents(INT32 nWidth, INT32 nHeight,
									INT32 nInputDatasetTLX, INT32 nInputDatasetTLY, INT32 nInputDatasetBRX, INT32 nInputDatasetBRY,
									INT32 *pnAdjustedDatasetTLX, INT32 *pnAdjustedDatasetTLY, INT32 *pnAdjustedDatasetBRX, INT32 *pnAdjustedDatasetBRY,
									INT32 *pnAdjustedDeviceTLX, INT32 *pnAdjustedDeviceTLY, INT32 *pnAdjustedDeviceBRX, INT32 *pnAdjustedDeviceBRY)
{
	INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;
	NCSError nError = NCS_SUCCESS;
	
	// The dataset extents and the required view extents don't intersect!
	if (!IntersectRects(nInputDatasetTLX, nInputDatasetTLY, 
						nInputDatasetBRX, nInputDatasetBRY, 
						0, 0, m_nWidth, m_nHeight) ){
		return NCS_FILE_INVALID_SETVIEW;
	}

	nDatasetTLX = nInputDatasetTLX;
	nDatasetTLY = nInputDatasetTLY;
	nDatasetBRX = nInputDatasetBRX;
	nDatasetBRY = nInputDatasetBRY;

	if ((nDatasetTLX < 0) || (nDatasetTLY < 0) ||
		(nDatasetBRX > m_nWidth-1) || (nDatasetBRY > m_nHeight-1)){

		INT32 nAdjustedDeviceTLX = 0;
		INT32 nAdjustedDeviceTLY = 0;
		INT32 nAdjustedDeviceBRX = nWidth;
		INT32 nAdjustedDeviceBRY = nHeight;
		INT32 nDeviceTLX = 0;
		INT32 nDeviceTLY = 0;
		INT32 nDeviceBRX = nWidth;
		INT32 nDeviceBRY = nHeight;
		INT32 nAdjustedDatasetTLX = nDatasetTLX;
		INT32 nAdjustedDatasetTLY = nDatasetTLY;
		INT32 nAdjustedDatasetBRX = nDatasetBRX;
		INT32 nAdjustedDatasetBRY = nDatasetBRY;

		// 4 sets of data, nAdjustedDevice, nDevice, nAdjustedDataset, nDataset
		if (nDatasetTLX < 0) {
			nAdjustedDatasetTLX = 0;
			nAdjustedDeviceTLX = (INT32)(((IEEE8)((nDeviceBRX - nDeviceTLX)*(nAdjustedDatasetTLX-nDatasetTLX))/(IEEE8)(nDatasetBRX - nDatasetTLX)) + (IEEE8)nDeviceTLX);
		}
		if (nDatasetTLY < 0) {
			nAdjustedDatasetTLY = 0;
			nAdjustedDeviceTLY = (INT32)(((IEEE8)((nDeviceBRY - nDeviceTLY)*(nAdjustedDatasetTLY-nDatasetTLY))/(IEEE8)(nDatasetBRY - nDatasetTLY)) + (IEEE8)nDeviceTLY);
		}
		if (nDatasetBRX > m_nWidth) {
			nAdjustedDatasetBRX = m_nWidth;
			nAdjustedDeviceBRX = nDeviceBRX - (INT32)((IEEE8)(nDeviceBRX - nDeviceTLX)*(IEEE8)(nDatasetBRX - nAdjustedDatasetBRX)/(IEEE8)(nDatasetBRX - nDatasetTLX));
		}
		if (nDatasetBRY > m_nHeight) {
			nAdjustedDatasetBRY = m_nHeight;
			nAdjustedDeviceBRY = nDeviceBRY - (INT32)((IEEE8)(nDeviceBRY - nDeviceTLY)*(IEEE8)(nDatasetBRY - nAdjustedDatasetBRY)/(IEEE8)(nDatasetBRY - nDatasetTLY));
		}

		*pnAdjustedDeviceTLX = nAdjustedDeviceTLX;
		*pnAdjustedDeviceTLY = nAdjustedDeviceTLY;
		*pnAdjustedDeviceBRX = nAdjustedDeviceBRX;
		*pnAdjustedDeviceBRY = nAdjustedDeviceBRY;

		*pnAdjustedDatasetTLX= nAdjustedDatasetTLX;
		*pnAdjustedDatasetTLY= nAdjustedDatasetTLY;
		*pnAdjustedDatasetBRX= nAdjustedDatasetBRX;
		*pnAdjustedDatasetBRY= nAdjustedDatasetBRY;
	}
	else {
		*pnAdjustedDeviceTLX = 0;
		*pnAdjustedDeviceTLY = 0;
		*pnAdjustedDeviceBRX = nWidth;
		*pnAdjustedDeviceBRY = nHeight;

		*pnAdjustedDatasetTLX= nDatasetTLX;
		*pnAdjustedDatasetTLY= nDatasetTLY;
		*pnAdjustedDatasetBRX= nDatasetBRX;
		*pnAdjustedDatasetBRY= nDatasetBRY;
	}
	return NCS_SUCCESS;
}

/**
 * Adjust the extents, using input world coordinates.
 * The renderer object attemps to draw to a bitmap, irrespective of whether the extents
 * are outside the actual dataset extents or not. This method adjusts the extents to be
 * within the dataset and returns the relevant adjustments.
 * @param nWidth The output device view width
 * @param nHeight The output device view height
 * @param dInputWorldTLX The unadjusted top left X world coordinate
 * @param dInputWorldTLY The unadjusted top left Y world coordinate
 * @param dInputWorldBRX The unadjusted bottom right X world coordinate
 * @param dInputWorldBRY The unadjusted bottom right Y world coordinate
 * @param *pdAdjustedWorldTLX [OUT] The output adjusted top left X world coordinate
 * @param *pdAdjustedWorldTLY [OUT] The output adjusted top left Y world coordinate
 * @param *pdAdjustedWorldBRX [OUT] The output adjusted bottom right X world coordinate
 * @param *pdAdjustedWorldBRY [OUT] The output adjusted bottom right Y world coordinate
 * @param *pnAdjustedDeviceTLX [OUT] The output adjusted top left X device coordinate
 * @param *pnAdjustedDeviceTLY [OUT] The output adjusted top left Y device coordinate
 * @param *pnAdjustedDeviceBRX [OUT] The output adjusted bottom right X device coordinate
 * @param *pnAdjustedDeviceBRY [OUT] The output adjusted bottom right Y device coordinate
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
//NCSError inline CNCSRenderer::AdjustExtents(INT32 nWidth, INT32 nHeight, 
NCSError CNCSRenderer::AdjustExtents(INT32 nWidth, INT32 nHeight, 
									IEEE8 dInputWorldTLX, IEEE8 dInputWorldTLY, IEEE8 dInputWorldBRX, IEEE8 dInputWorldBRY, 
									IEEE8 *pdAdjustedWorldTLX, IEEE8 *pdAdjustedWorldTLY, IEEE8 *pdAdjustedWorldBRX, IEEE8 *pdAdjustedWorldBRY,
									INT32 *pnAdjustedDeviceTLX, INT32 *pnAdjustedDeviceTLY, INT32 *pnAdjustedDeviceBRX, INT32 *pnAdjustedDeviceBRY)
{
	IEEE8 dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY;
	NCSError nError = NCS_SUCCESS;

#ifdef NOTDEF
	// Need a double version of this!
	if (!IntersectRects(dInputWorldTLX, dInputWorldTLY, 
						dInputWorldBRX, dInputWorldBRY, 
						m_fOriginX, m_fOriginY, 
						m_fOriginX + (IEEE8)m_nWidth*m_fCellIncrementX, 
						m_fOriginY + (IEEE8)m_nHeight*m_fCellIncrementY) ){
		return NCS_FILE_INVALID_SETVIEW;
	}
#endif

	dWorldTLX = dInputWorldTLX;
	dWorldTLY = dInputWorldTLY;
	dWorldBRX = dInputWorldBRX;
	dWorldBRY = dInputWorldBRY;

	if (!IS_BETWEEN(dWorldTLX, m_dOriginX, m_dOriginX + ((IEEE8)m_nWidth)*m_dCellIncrementX)  ||
		!IS_BETWEEN(dWorldTLY, m_dOriginY, m_dOriginY + ((IEEE8)m_nHeight)*m_dCellIncrementY) ||
		!IS_BETWEEN(dWorldBRX, m_dOriginX, m_dOriginX + ((IEEE8)m_nWidth)*m_dCellIncrementX)  ||
		!IS_BETWEEN(dWorldBRY, m_dOriginY, m_dOriginY + ((IEEE8)m_nHeight)*m_dCellIncrementY) ) {

		INT32 nAdjustedDeviceTLX = 0;
		INT32 nAdjustedDeviceTLY = 0;
		INT32 nAdjustedDeviceBRX = nWidth;
		INT32 nAdjustedDeviceBRY = nHeight;
		INT32 nDeviceTLX = 0;
		INT32 nDeviceTLY = 0;
		INT32 nDeviceBRX = nWidth;
		INT32 nDeviceBRY = nHeight;
		IEEE8 dAdjustedWorldTLX = dWorldTLX;
		IEEE8 dAdjustedWorldTLY = dWorldTLY;
		IEEE8 dAdjustedWorldBRX = dWorldBRX;
		IEEE8 dAdjustedWorldBRY = dWorldBRY;

		// Lat long inversion check
		if (m_dCellIncrementY < 0.0) {
			if (dWorldTLY > m_dOriginY) {
				dAdjustedWorldTLY = m_dOriginY;
				nAdjustedDeviceTLY = (INT32)(((IEEE8)((nDeviceBRY - nDeviceTLY)*(dAdjustedWorldTLY-dWorldTLY))/(IEEE8)(dWorldBRY - dWorldTLY)) + (IEEE8)nDeviceTLY);
			}
			if (dWorldBRY < (m_dOriginY + (m_nHeight)*m_dCellIncrementY)) {
				dAdjustedWorldBRY = (m_dOriginY + (m_nHeight)*m_dCellIncrementY);
				nAdjustedDeviceBRY = nDeviceBRY - (INT32)((IEEE8)(nDeviceBRY - nDeviceTLY)*(IEEE8)(dWorldBRY - dAdjustedWorldBRY)/(IEEE8)(dWorldBRY - dWorldTLY));
			}
		} else {
			if (dWorldTLY < m_dOriginY) {
				dAdjustedWorldTLY = m_dOriginY;
				nAdjustedDeviceTLY = (INT32)(((IEEE8)((nDeviceBRY - nDeviceTLY)*(dAdjustedWorldTLY-dWorldTLY))/(IEEE8)(dWorldBRY - dWorldTLY)) + (IEEE8)nDeviceTLY);
			}
			if (dWorldBRY > (m_dOriginY + (m_nHeight)*m_dCellIncrementY)) {
				//dAdjustedWorldBRY = (m_dOriginX + (m_nWidth)*m_dCellIncrementX);
				dAdjustedWorldBRY = (m_dOriginY + (m_nHeight)*m_dCellIncrementY);		/**[08]**/
				nAdjustedDeviceBRY = nDeviceBRY - (INT32)((IEEE8)(nDeviceBRY - nDeviceTLY)*(IEEE8)(dWorldBRY - dAdjustedWorldBRY)/(IEEE8)(dWorldBRY - dWorldTLY));
			}
		}
		// X is same for all coord systems
		if (dWorldBRX > (m_dOriginX + (m_nWidth)*m_dCellIncrementX)) {
			dAdjustedWorldBRX = (m_dOriginX + (m_nWidth)*m_dCellIncrementX);
			nAdjustedDeviceBRX = nDeviceBRX - (INT32)((IEEE8)(nDeviceBRX - nDeviceTLX)*(IEEE8)(dWorldBRX - dAdjustedWorldBRX)/(IEEE8)(dWorldBRX - dWorldTLX));
		}
		if (dWorldTLX < m_dOriginX) {
			dAdjustedWorldTLX = m_dOriginX;
			nAdjustedDeviceTLX = (INT32)(((IEEE8)((nDeviceBRX - nDeviceTLX)*(dAdjustedWorldTLX-dWorldTLX))/(IEEE8)(dWorldBRX - dWorldTLX)) + (IEEE8)nDeviceTLX);
		}

		*pnAdjustedDeviceTLX = nAdjustedDeviceTLX;
		*pnAdjustedDeviceTLY = nAdjustedDeviceTLY;
		*pnAdjustedDeviceBRX = nAdjustedDeviceBRX;
		*pnAdjustedDeviceBRY = nAdjustedDeviceBRY;

		*pdAdjustedWorldTLX= dAdjustedWorldTLX;
		*pdAdjustedWorldTLY= dAdjustedWorldTLY;
		*pdAdjustedWorldBRX= dAdjustedWorldBRX;
		*pdAdjustedWorldBRY= dAdjustedWorldBRY;
	}
	else {
		*pnAdjustedDeviceTLX = 0;
		*pnAdjustedDeviceTLY = 0;
		*pnAdjustedDeviceBRX = nWidth;
		*pnAdjustedDeviceBRY = nHeight;

		*pdAdjustedWorldTLX= dWorldTLX;
		*pdAdjustedWorldTLY= dWorldTLY;
		*pdAdjustedWorldBRX= dWorldBRX;
		*pdAdjustedWorldBRY= dWorldBRY;
	}

	return NCS_SUCCESS;
}

/**
 * Set the view extents.
 * Set the view extents of the renderer object, in world coordinates. If the
 * view extents are out side the dataset extents, it adjusts the extents and draws
 * accordingly. After adjusting the extents, this makes a call to the underlying
 * SetView() in the base NCSFile class.
 * @param nBands The number of bands to view
 * @param pBandList A pointer to a list of band numbers
 * @param nWidth The output device view width
 * @param nHeight The output device view height
 * @param dWorldTLX The world top left X coordinate
 * @param dWorldTLY The world top left Y coordinate
 * @param dWorldBRX The world bottom right X coordinate
 * @param dWorldBRY The world bottom right Y coordinate
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::SetView ( INT32 nBands, INT32 *pBandList, 
				   INT32 nWidth, INT32 nHeight,
				   IEEE8 dWorldTLX, IEEE8 dWorldTLY,
				   IEEE8 dWorldBRX, IEEE8 dWorldBRY )
{
	NCSError nError = NCS_SUCCESS;

	if (m_bIsOpen) {	
		INT32 nAdjustedDeviceTLX, nAdjustedDeviceTLY, nAdjustedDeviceBRX, nAdjustedDeviceBRY;
		IEEE8 dAdjustedWorldTLX, dAdjustedWorldTLY, dAdjustedWorldBRX, dAdjustedWorldBRY;

		m_bSetViewModeIsWorld = TRUE;

		if (nWidth <= 0 || nHeight <= 0) {
			return NCS_FILE_INVALID_SETVIEW;
		}
		
		// Need this because of rounding errors when stretching subsampled images.
		IEEE8 dWTLX, dWTLY, dWBRX, dWBRY;
		INT32 dTLX = (INT32)floor((dWorldTLX - m_dOriginX)/m_dCellIncrementX);
		INT32 dTLY = (INT32)floor((dWorldTLY - m_dOriginY)/m_dCellIncrementY);
		INT32 dBRX = (INT32)ceil((dWorldBRX - m_dOriginX)/m_dCellIncrementX);
		INT32 dBRY = (INT32)ceil((dWorldBRY - m_dOriginY)/m_dCellIncrementY);
		ConvertDatasetToWorld(dTLX, dTLY, &dWTLX, &dWTLY);
		ConvertDatasetToWorld(dBRX, dBRY, &dWBRX, &dWBRY);

		nError = AdjustExtents(nWidth, nHeight, dWTLX, dWTLY, dWBRX, dWBRY,
					 &dAdjustedWorldTLX, &dAdjustedWorldTLY, &dAdjustedWorldBRX, &dAdjustedWorldBRY,
					 &nAdjustedDeviceTLX, &nAdjustedDeviceTLY, &nAdjustedDeviceBRX, &nAdjustedDeviceBRY);

		/*nError = AdjustExtents(nWidth, nHeight, dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY,
					 &dAdjustedWorldTLX, &dAdjustedWorldTLY, &dAdjustedWorldBRX, &dAdjustedWorldBRY,
					 &nAdjustedDeviceTLX, &nAdjustedDeviceTLY, &nAdjustedDeviceBRX, &nAdjustedDeviceBRY);
		*/
		if (nError == NCS_SUCCESS) {
//			m_bHaveValidSetView = TRUE;
			m_nAdjustedViewWidth = nAdjustedDeviceBRX - nAdjustedDeviceTLX;
			m_nAdjustedViewHeight = nAdjustedDeviceBRY - nAdjustedDeviceTLY;
//			m_nSetViewWidth = m_nAdjustedViewWidth;
//			m_nSetViewHeight = m_nAdjustedViewHeight;
			m_nAdjustedXOffset = nAdjustedDeviceTLX;
			m_nAdjustedYOffset = nAdjustedDeviceTLY;

			// The ECW library does not subsample, so fix it here!
			INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;
			ConvertWorldToDataset(dAdjustedWorldTLX, dAdjustedWorldTLY, &nDatasetTLX, &nDatasetTLY);
			ConvertWorldToDataset(dAdjustedWorldBRX, dAdjustedWorldBRY, &nDatasetBRX, &nDatasetBRY);
			if ((nDatasetBRX - nDatasetTLX) < m_nAdjustedViewWidth ) {
				m_nAdjustedViewWidth = nDatasetBRX - nDatasetTLX;	//[13][18]
			}
			if ((nDatasetBRY - nDatasetTLY) < m_nAdjustedViewHeight) {
				m_nAdjustedViewHeight = nDatasetBRY - nDatasetTLY;	//[13][18]
			}

			// This is for the output file class (JPEG) so that it can write a "world" file from the adjusted extents.
			m_dAdjustedWorldTLX = dAdjustedWorldTLX;
			m_dAdjustedWorldTLY = dAdjustedWorldTLY;
			m_dAdjustedWorldBRX = dAdjustedWorldBRX;
			m_dAdjustedWorldBRY = dAdjustedWorldBRY;

			nError = CNCSFile::SetView(nBands, pBandList, m_nAdjustedViewWidth, m_nAdjustedViewHeight, 
									   dAdjustedWorldTLX, dAdjustedWorldTLY, dAdjustedWorldBRX, dAdjustedWorldBRY);		
			UsingAlphaBand();

			char *str = (char*)NCSGetLastErrorText(nError);
			m_bHaveReadImage = FALSE;
		}
		else
		{
			nError = NCS_ECW_ERROR;							/**[12]**/
		}

		if (nError != NCS_SUCCESS) {
	//		m_bHaveValidSetView = FALSE;
		} else {
#ifdef NOTDEF
			//NCS_HIST_AND_LUT_SUPPORT
			m_bHaveValidSetView = m_bCalcHistograms ? SetupHistograms() : TRUE;
			if(!m_bHaveValidSetView) {
				nError = NCS_COULDNT_ALLOC_MEMORY;
			}
#else // NCS_HIST_AND_LUT_SUPPORT
//			m_bHaveValidSetView = TRUE;
#endif // NCS_HIST_AND_LUT_SUPPORT
		}
	}
	else {
		nError = NCS_FILE_NOT_OPEN;
	}

	return nError;
}

/**
 * Set the view extents.
 * Set the view extents of the renderer object, in dataset units. If the
 * view extents are out side the dataset extents, it adjusts the extents and draws
 * accordingly. After adjusting the extents, this makes a call to the underlying
 * SetView() in the base NCSFile class.
 * @param nBands The number of bands to view
 * @param pBandList A pointer to a list of band numbers
 * @param nWidth The output device view width
 * @param nHeight The output device view height
 * @param nDatasetTLX The dataset top left X coordinate
 * @param nDatasetTLY The dataset top left Y coordinate
 * @param nDatasetBRX The dataset bottom right X coordinate
 * @param nDatasetBRY The dataset bottom right Y coordinate
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::SetView ( INT32 nBands, INT32 *pBandList, 
				   INT32 nWidth, INT32 nHeight,
				   INT32 nDatasetTLX, INT32 nDatasetTLY,
				   INT32 nDatasetBRX, INT32 nDatasetBRY )
{
	NCSError nError = NCS_SUCCESS;

	if (m_bIsOpen) {
		INT32 nAdjustedDatasetTLX, nAdjustedDatasetTLY, nAdjustedDatasetBRX, nAdjustedDatasetBRY;
		INT32 nAdjustedDeviceTLX, nAdjustedDeviceTLY, nAdjustedDeviceBRX, nAdjustedDeviceBRY;

		m_bSetViewModeIsWorld = FALSE;

		if (nWidth <= 0 || nHeight <= 0) {
			return NCS_FILE_INVALID_SETVIEW;
		}
		
		if (AdjustExtents(nWidth, nHeight, nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY,
					 &nAdjustedDatasetTLX, &nAdjustedDatasetTLY, &nAdjustedDatasetBRX, &nAdjustedDatasetBRY,
					 &nAdjustedDeviceTLX, &nAdjustedDeviceTLY, &nAdjustedDeviceBRX, &nAdjustedDeviceBRY) == NCS_SUCCESS) {

//			m_bHaveValidSetView = TRUE;
			m_nAdjustedViewWidth = nAdjustedDeviceBRX - nAdjustedDeviceTLX;
			m_nAdjustedViewHeight = nAdjustedDeviceBRY - nAdjustedDeviceTLY;
//			m_nSetViewWidth = m_nAdjustedViewWidth;
//			m_nSetViewHeight = m_nAdjustedViewHeight;
			m_nAdjustedXOffset = nAdjustedDeviceTLX;
			m_nAdjustedYOffset = nAdjustedDeviceTLY;

			// The ECW library does not subsample, so fix it here!
			if ((nAdjustedDatasetBRX - nAdjustedDatasetTLX) < m_nAdjustedViewWidth) {
				m_nAdjustedViewWidth = nAdjustedDatasetBRX - nAdjustedDatasetTLX;
			}
			if ((nAdjustedDatasetBRY - nAdjustedDatasetTLY) < m_nAdjustedViewHeight) {
				m_nAdjustedViewHeight = nAdjustedDatasetBRY - nAdjustedDatasetTLY;
			}

			nError = CNCSFile::SetView(nBands, pBandList, m_nAdjustedViewWidth, m_nAdjustedViewHeight, 
									  nAdjustedDatasetTLX, nAdjustedDatasetTLY, nAdjustedDatasetBRX, nAdjustedDatasetBRY);
			m_bHaveReadImage = FALSE;
			UsingAlphaBand();
		}

		if (nError != NCS_SUCCESS && nError != NCS_FILEIO_ERROR && nError != NCS_CONNECTION_LOST) {	//[17]
			char buf[1024];														//[05]
			sprintf(buf, "SetView() Error - %s", NCSGetErrorText(nError));		//[05]
			nError = NCS_ECW_ERROR;												//[05]
			NCSFormatErrorText(nError, buf);									//[05]
#ifdef NOTDEF
			//NCS_HIST_AND_LUT_SUPPORT
		} else if(m_bCalcHistograms) {
			if(SetupHistograms() == FALSE) {
				nError = NCS_COULDNT_ALLOC_MEMORY;
			}
#endif
		}
	}
	else {
		nError = NCS_FILE_NOT_OPEN;
	}

	return nError;
}

/**
 * Read a line in BIL format.
 * Read a line of data, in Band Interleaved by Line format.
 * Optionally collect histograms based on value of m_bCalcHistograms
 * @param ppOutputLine A pointer to a list of buffers, to recieve a scanline of BILL image data
 * @see ReadLineRGB()
 * @see ReadLineBGR()
 * @see SetView()
 * @see NCSEcwReadStatus
 * @return NCSEcwReadStatus
 */
NCSEcwReadStatus CNCSRenderer::ReadLineBIL (UINT8 **ppOutputLine)
{
	NCSEcwReadStatus eStatus = CNCSFile::ReadLineBIL(ppOutputLine);
#ifdef NCS_HIST_AND_LUT_SUPPORT	
	if(m_bCalcHistograms && NCSECW_READ_OK == eStatus) {
		NCSFileViewSetInfo *pInfo = GetFileViewSetInfo();

		INT32 nBands = pInfo->nBands;

		NCSMutexBegin(&m_HistogramMutex);
		if(m_nReadLine++ == 0) {
			if(SetupHistograms() == FALSE) {
				m_nReadLine = 0;
				return(NCSECW_READ_FAILED);
			}
		}
		for(INT32 x = 0; x < m_nSetViewWidth; x++) {
			for(int b = 0; b < nBands; b++) {
				m_pHistograms[b][ppOutputLine[b][x]]++;
			}
		}
		if(m_nReadLine >= m_nSetViewHeight) {
			m_nReadLine = 0;
		}
		NCSMutexEnd(&m_HistogramMutex);
	} else if(NCSECW_READ_OK != eStatus) {
		m_nReadLine = 0;
	}
#endif // NCS_HIST_AND_LUT_SUPPORT
	return(eStatus);
}

/**
 * Read a line in RGB format.
 * Read a line of data, in RGB format.
 * Optionally collect histograms based on value of m_bCalcHistograms
 * @param pRGBTriplet A pointer a buffer, to revieve a scanline of RGB packed image data
 * @see ReadLineBIL()
 * @see ReadLineBGR()
 * @see NCSEcwReadStatus
 * @return NCSEcwReadStatus
 */
NCSEcwReadStatus CNCSRenderer::ReadLineRGB (UINT8 *pRGBTriplet)
{
	NCSEcwReadStatus eStatus = CNCSFile::ReadLineRGB(pRGBTriplet);
#ifdef NCS_HIST_AND_LUT_SUPPORT
	if(m_bCalcHistograms && NCSECW_READ_OK == eStatus) {
		UINT8 *pRGB = pRGBTriplet;

		NCSMutexBegin(&m_HistogramMutex);
		if(m_nReadLine++ == 0) {
			if(SetupHistograms() == FALSE) {
				m_nReadLine = 0;
				return(NCSECW_READ_FAILED);
			}
		}
		for(INT32 x = 0; x < m_nSetViewWidth; x++) {
			m_pHistograms[0][*(pRGB++)]++;
			m_pHistograms[1][*(pRGB++)]++;
			m_pHistograms[2][*(pRGB++)]++;
		}
		if(m_nReadLine >= m_nSetViewHeight) {
			m_nReadLine = 0;
		}
		NCSMutexEnd(&m_HistogramMutex);
	} else if(NCSECW_READ_OK != eStatus) {
		m_nReadLine = 0;
	}
#endif // NCS_HIST_AND_LUT_SUPPORT
	return(eStatus);
}

/**
 * Read a line in BGR format.
 * Read a line of data, in BGR format.
 * Optionally collect histograms based on value of m_bCalcHistograms
 * @param pRGBTriplet A pointer a buffer, to revieve a scanline of BGR packed image data
 * @see ReadLineRGB()
 * @see ReadLineBIL()
 * @see NCSEcwReadStatus
 * @return NCSEcwReadStatus
 */
NCSEcwReadStatus CNCSRenderer::ReadLineBGR (UINT8 *pRGBTriplet)
{
	NCSEcwReadStatus eStatus = CNCSFile::ReadLineBGR(pRGBTriplet);
#ifdef NCS_HIST_AND_LUT_SUPPORT
	if(m_bCalcHistograms && NCSECW_READ_OK == eStatus) {
		UINT8 *pBGR = pRGBTriplet;

		NCSMutexBegin(&m_HistogramMutex);
		if(m_nReadLine++ == 0) {
			if(SetupHistograms() == FALSE) {
				m_nReadLine = 0;
				return(NCSECW_READ_FAILED);
			}
		}

		for(INT32 x = 0; x < m_nSetViewWidth; x++) {
			m_pHistograms[2][*(pBGR++)]++;
			m_pHistograms[1][*(pBGR++)]++;
			m_pHistograms[0][*(pBGR++)]++;
		}
		if(m_nReadLine >= m_nSetViewHeight) {
			m_nReadLine = 0;
		}
		NCSMutexEnd(&m_HistogramMutex);
	} else if(NCSECW_READ_OK != eStatus) {
		m_nReadLine = 0;
	}
#endif // NCS_HIST_AND_LUT_SUPPORT
	return(eStatus);
}

#if defined( LINUX ) || defined( SOLARIS )
#else
/**
 * Read the image into an internal buffer ready for drawing.
 * In progressive mode, when a RefreshUpdate() callback arrives
 * from the network, the client should call ReadImage() to transfer the
 * pending imagery from the network into an internal buffer. Once this
 * is done, the client can then call DrawImage() at any time to draw from
 * the internal buffer into the device. In non-progressive mode the
 * client should call ReadImage(), then immedeately call DrawImage()
 * to draw to the device.
 * @param pViewSetInfo A pointer to the NCSFileViewSetInfo struct passed to the RefreshUpdate() function
 * @see DrawImage()
 * @see RefreshUpdate()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::ReadImage(NCSFileViewSetInfo *pViewSetInfo)
{
	return ReadImage(pViewSetInfo->fTopX, pViewSetInfo->fLeftY, 
					 pViewSetInfo->fBottomX, pViewSetInfo->fRightY,
					 pViewSetInfo->nTopX, pViewSetInfo->nLeftY, 
					 pViewSetInfo->nBottomX, pViewSetInfo->nRightY, 
					 pViewSetInfo->nSizeX, pViewSetInfo->nSizeY );
}

/**
 * Read the image into an internal buffer ready for drawing.
 * In progressive mode, when a RefreshUpdate() callback arrives
 * from the network, the client should call ReadImage() to transfer the
 * pending imagery from the network into an internal buffer. Once this
 * is done, the client can then call DrawImage() at any time to draw from
 * the internal buffer into the device. In non-progressive mode the
 * client should call ReadImage(), then immedeately call DrawImage()
 * to draw to the device.
 * This overloaded function is called in progressive mode only.
 * @param dWorldTLX The view world top left X coordinate (must match the SetView() top left X).
 * @param dWorldTLY The view world top left Y coordinate (must match the SetView() top left Y).
 * @param dWorldBRX The view world bottom right X coordinate (must match the SetView() bottom right X).
 * @param dWorldBRY The view world bottom right Y coordinate (must match the SetView() bottom right Y).
 * @param nDatasetTLX The dataset top left X coordinate
 * @param nDatasetTLY The dataset top left Y coordinate
 * @param nDatasetBRX The dataset bottom right X coordinate
 * @param nDatasetBRY The dataset bottom right Y coordinate
 * @param nWidth The view width (must match the set view width).
 * @param nHeight The view height (must match the set view height).
 * @see DrawImage()
 * @see RefreshUpdate()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::ReadImage(IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY, INT32 nDatasetTLX, INT32 nDatasetTLY, INT32 nDatasetBRX, INT32 nDatasetBRY, INT32 nWidth, INT32 nHeight)
{
	//If we are in here we _MUST_ be in progressive mode
#ifdef _MACINTOSH
	NCSTimeStampMs tsStart = NCSGetTimeStampMs();
#endif //MACINTOSH

	if (!m_bHaveValidSetView)
	{
		char buf[1024];														/**[11]**/
		sprintf(buf, "A valid view has not been set.");						/**[11]**/
		NCSFormatErrorText(NCS_ECW_ERROR, buf);								/**[11]**/
		return NCS_ECW_ERROR;
	}

	m_bHaveReadImage = FALSE;

	if (!m_pRGBALocal) {
		m_pRGBALocal = (UINT8 *)NCSMalloc(nWidth*nHeight*4, 1);
	} else {
		// Assume the best, that this will not get realloced if the size is the same
		m_pRGBALocal = (UINT8 *)NCSRealloc(m_pRGBALocal, nWidth*nHeight*4, 1);
	}

	NCSEcwReadStatus eStatus = NCSECW_READ_OK;
	UINT8 *pCopyRGBA = m_pRGBALocal;

#ifndef DIB_FIX
	for (int i=0; i<nHeight; i++) {	
		eStatus = ReadLineBGRA(pCopyRGBA);
#if defined(MACINTOSH) || defined(MACOSX)
		if (((i%10) == 0) && (i>=10)) {
				NCSThreadYield(); // Yeild this thread every 30 scanlines to free up cpu time for drawing.
		}
#else
		if (((i%10) == 0) && (i>10)) {
			NCSThreadYield(); // Yeild this thread every 10 scanlines to free up cpu time for drawing.
		}
#endif
		if (eStatus != NCSECW_READ_OK) {
			break;
		}
		pCopyRGBA += nWidth*4;
	}
#else
	//if (nHeight > 0)
	{
		pCopyRGBA += (nHeight-1)*(nWidth*4);
		for (int i=0; i<nHeight; i++) {	
			eStatus = CNCSRenderer::ReadLineBGRA((UINT32*)pCopyRGBA);
			if (((i%10) == 0) && (i>10)) {
				NCSThreadYield(); // Yeild this thread every 10 scanlines to free up cpu time for drawing.
			}
			if (eStatus != NCSECW_READ_OK) {
				break;
			}
			pCopyRGBA -= nWidth*4;
		}
	}
#endif

	if (eStatus != NCSECW_READ_OK) {
		if (eStatus == NCSECW_READ_CANCELLED) {
//			m_bHaveValidSetView = FALSE;
			_RPT0(_CRT_WARN, "READ WAS CANCELLED in ReadLineBGR()!\n");
			return NCS_ECW_READ_CANCELLED;
		}
		else if (eStatus == NCSECW_READ_FAILED) {
//			m_bHaveValidSetView = FALSE;
			NCSFormatErrorText(NCS_ECW_ERROR, " ReadLineRGB() failed (returned NCSECW_READ_FAILED).");
			return NCS_ECW_ERROR;
		}
	}

	NCSMutexBegin(&m_DrawMutex);
	// Copy the rgb data and size info into the renderer's members
	
	if (!m_pRGBA) {
		m_pRGBA = (UINT8 *)NCSMalloc(nWidth*nHeight*4, 1);
	} else {
		m_pRGBA = (UINT8 *)NCSRealloc(m_pRGBA, nWidth*nHeight*4, 1);
	}
	
	if (m_pRGBA && m_pRGBALocal)
		memcpy(m_pRGBA, m_pRGBALocal, nWidth*nHeight*4);

	m_nAdjustedViewWidth = nWidth;
	m_nAdjustedViewHeight = nHeight;
	
	m_nRendererWidth = nWidth;
	m_nRendererHeight = nHeight;

	m_nRendererDatasetTLX = nDatasetTLX;
	m_nRendererDatasetTLY = nDatasetTLY;
	m_nRendererDatasetBRX = nDatasetBRX;
	m_nRendererDatasetBRY = nDatasetBRY;
	m_dRendererWorldTLX = dWorldTLX;
	m_dRendererWorldTLY = dWorldTLY;
	m_dRendererWorldBRX = dWorldBRX;
	m_dRendererWorldBRY = dWorldBRY;

	m_bHaveReadImage = TRUE;

#if defined( DIB_FIX ) || defined(MACINTOSH) || defined(MACOSX)
	m_bCreateNewDIB = TRUE; //force creation of a new DIB to load new image data /**[10]**/
#endif //DIB_FIX

	NCSMutexEnd(&m_DrawMutex);
#ifdef NOT_MACINTOSH
	NCSTimeStampMs tsElapsed = NCSGetTimeStampMs() - tsStart;
	char szBuffer[256];
	sprintf(szBuffer, "0ReadImage took %d ms", (int)tsElapsed);
	szBuffer[0] = strlen( szBuffer );
	DEBUGPRINT( (unsigned char *)szBuffer );
#endif //MACINTOSH
	return NCS_SUCCESS;
}

/**
 * Read the image into an internal buffer ready for drawing.
 * In progressive mode, when a RefreshUpdate() callback arrives
 * from the network, the client should call ReadImage() to transfer the
 * pending imagery from the network into an internal buffer. Once this
 * is done, the client can then call DrawImage() at any time to draw from
 * the internal buffer into the device. In non-progressive mode the
 * client should call ReadImage(), then immedeately call DrawImage()
 * to draw to the device. 
 * This overloaded function is called in non-progressive mode only.
 * @param nWidth The view width (must match the set view width).
 * @param nHeight The view height (must match the set view height).
 * @see DrawImage()
 * @see RefreshUpdate()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSRenderer::ReadImage(INT32 nWidth, INT32 nHeight)
{
	NCSError nError = NCS_SUCCESS;

	// If we are here we are _ALWAYS_ in NON-Progressive mode
	if (!m_bHaveValidSetView)
	{
		char buf[201];														//[11]
		sprintf(buf, "A valid view has not been set.");						//[11]
		NCSFormatErrorText(NCS_ECW_ERROR, buf);								//[11]
		return NCS_ECW_ERROR;
	}

#if defined( DIB_FIX ) || defined(MACINTOSH) || defined(MACOSX)
	m_bCreateNewDIB = TRUE; //force creation of a new DIB to load new image data /**[10]**/
#endif //DIB_FIX
	m_bHaveReadImage = FALSE;

	if (!m_bIsProgressive) {
		nWidth = m_nAdjustedViewWidth;
		nHeight = m_nAdjustedViewHeight;
	}

	if (!m_pRGBALocal) {
		m_pRGBALocal = (UINT8 *)NCSMalloc(nWidth*nHeight*4, 1);
	} else {
		// Assume the best, that this will not get realloced if the size is the same
#ifdef WIN32
		m_pRGBALocal = (UINT8 *)NCSRealloc(m_pRGBALocal, nWidth*nHeight*4, 1);
		
#elif defined(MACINTOSH) || defined(MACOSX) // Check if realloc is successful.
		UINT8 *pTemp;
		pTemp = (UINT8 *)NCSRealloc(m_pRGBTripletsLocal, nWidth*nHeight*3, 1);
		if( !pTemp ) {
			return NCS_COULDNT_ALLOC_MEMORY;
		}
		else {
			m_pRGBTripletsLocal = pTemp;
		}
#endif
	}


	NCSEcwReadStatus eStatus = NCSECW_READ_OK;
	UINT8 *pCopyRGBA = m_pRGBALocal;

#ifndef DIB_FIX
	for (int i=0; i<nHeight; i++) {
		eStatus = ReadLineBGRA(pCopyRGBA);

		if (eStatus != NCSECW_READ_OK) {
			break;
		}
		pCopyRGBA += nWidth*4;
	}
#else
	pCopyRGBA += (nHeight-1)*(nWidth*4);
	for (int i=0; i<nHeight; i++) {	
		eStatus = CNCSRenderer::ReadLineBGRA((UINT32*)pCopyRGBA);

		if (eStatus != NCSECW_READ_OK) {
			break;
		}
		pCopyRGBA -= nWidth*4;
	}
#endif

	if (eStatus != NCSECW_READ_OK) {
		if (eStatus == NCSECW_READ_CANCELLED) {
//			m_bHaveValidSetView = FALSE;
			_RPT0(_CRT_WARN, "READ WAS CANCELLED in ReadLineBGR()!\n");
		}
		else if (eStatus == NCSECW_READ_FAILED) {
#ifdef _DEBUG																/**[12]**/
			::MessageBox(NULL, NCS_T("ReadLineRGB() returned an error!"), NCS_T("NCSFile class"), MB_OK);
#endif																				/**[12]**/
			nError = NCS_ECW_ERROR;													/**[12]**/
			NCSFormatErrorText(NCS_ECW_ERROR, "ReadLineRGB() returned an error!");	/**[12]**/								//[11]
		}
	}

	NCSMutexBegin(&m_DrawMutex);

#ifdef WIN32
	if (!m_pRGBA) {
		m_pRGBA = (UINT8 *)NCSMalloc(nWidth*nHeight*4, 1);
	} else {
		m_pRGBA = (UINT8 *)NCSRealloc(m_pRGBA, nWidth*nHeight*4, 1);
	}
#endif
	m_nRendererWidth = nWidth;
	m_nRendererHeight = nHeight;

#ifdef WIN32
	memcpy(m_pRGBA, m_pRGBALocal, nWidth*nHeight*4);
#elif defined(MACINTOSH) || defined(MACOSX)
	m_pRGBA = m_pRGBALocal;
#endif
	m_bHaveReadImage = TRUE;

	NCSMutexEnd(&m_DrawMutex);

	return nError;
}

BOOLEAN CNCSRenderer::CalcStretchBltCoordinates(INT32 nViewWidth, INT32 nViewHeight,
												IEEE8 dTLX, IEEE8 dTLY,
												IEEE8 dBRX, IEEE8 dBRY,
												IEEE8 outputDeviceCoords[4],
												IEEE8 outputImageCoords[4])
{
	INT32 outputWidth, outputHeight;

	outputWidth = int(outputDeviceCoords[2] - outputDeviceCoords[0]);
	outputHeight = int(outputDeviceCoords[3] - outputDeviceCoords[1]);
	if (outputWidth != (nViewWidth-1) && outputHeight != (nViewHeight-1))
	{
		if ((outputWidth > (nViewWidth-1)) && (outputHeight > (nViewHeight-1)))	//zooming in
		{
			// Calculate the image extents of the sub area to be stretched from the main image, and only stretch that subset to the screen.
			IEEE8 outputScreenTLX = (outputDeviceCoords[0] < (IEEE8)0) ? (IEEE8)0 : outputDeviceCoords[0];
			IEEE8 outputScreenTLY = (outputDeviceCoords[1] < (IEEE8)0) ? (IEEE8)0 : outputDeviceCoords[1];
			IEEE8 outputScreenBRX = (outputDeviceCoords[2] > (IEEE8)(nViewWidth))  ? (IEEE8)(nViewWidth) : outputDeviceCoords[2];
			IEEE8 outputScreenBRY = (outputDeviceCoords[3] > (IEEE8)(nViewHeight)) ? (IEEE8)(nViewHeight) : outputDeviceCoords[3];

			IEEE8 outputImageWidth, outputImageHeight;
			outputImageWidth  = ((outputScreenBRX-outputScreenTLX) * (m_nRendererWidth))/(outputDeviceCoords[2]-outputDeviceCoords[0])  ;
		    outputImageHeight = ((outputScreenBRY-outputScreenTLY) * (m_nRendererHeight))/(outputDeviceCoords[3]-outputDeviceCoords[1])  ;
		    outputImageCoords[0] = ((outputScreenTLX-outputDeviceCoords[0])  * (outputImageWidth))/(outputScreenBRX-outputScreenTLX) ;
			outputImageCoords[1] = ((outputScreenTLY-outputDeviceCoords[1])  * (outputImageHeight))/(outputScreenBRY-outputScreenTLY) ;
			outputImageCoords[2] = outputImageCoords[0] + outputImageWidth;
			outputImageCoords[3] = outputImageCoords[1] + outputImageHeight;

			// Convert the truncated image coords back into world coords
			outputImageCoords[0] = floor(outputImageCoords[0]);
			outputImageCoords[1] = floor(outputImageCoords[1]);
			outputImageCoords[2] = ceil(outputImageCoords[2]);
			outputImageCoords[3] = ceil(outputImageCoords[3]);
			IEEE8 dNewOutputWorldTLX = m_dRendererWorldTLX + ((m_dRendererWorldBRX - m_dRendererWorldTLX)/((IEEE8)m_nRendererWidth)) * (outputImageCoords[0]);
			IEEE8 dNewOutputWorldTLY = m_dRendererWorldTLY + ((m_dRendererWorldBRY - m_dRendererWorldTLY)/((IEEE8)m_nRendererHeight)) * (outputImageCoords[1]);
			IEEE8 dNewOutputWorldBRX = m_dRendererWorldTLX + ((m_dRendererWorldBRX - m_dRendererWorldTLX)/((IEEE8)m_nRendererWidth)) * (outputImageCoords[2]);
			IEEE8 dNewOutputWorldBRY = m_dRendererWorldTLY + ((m_dRendererWorldBRY - m_dRendererWorldTLY)/((IEEE8)m_nRendererHeight)) * (outputImageCoords[3]);

			// Convert these new world coordinates into device coordinates.
			IEEE8 deviceCoords[4];
			deviceCoords[0] = (((outputDeviceCoords[2] - outputDeviceCoords[0]) * (dNewOutputWorldTLX-m_dRendererWorldTLX))/(m_dRendererWorldBRX - m_dRendererWorldTLX)) + outputDeviceCoords[0];
			deviceCoords[1] = (((outputDeviceCoords[3] - outputDeviceCoords[1]) * (dNewOutputWorldTLY-m_dRendererWorldTLY))/(m_dRendererWorldBRY - m_dRendererWorldTLY)) + outputDeviceCoords[1];
			deviceCoords[2] = (((outputDeviceCoords[2] - outputDeviceCoords[0]) * (dNewOutputWorldBRX-m_dRendererWorldTLX))/(m_dRendererWorldBRX - m_dRendererWorldTLX)) + outputDeviceCoords[0];
			deviceCoords[3] = (((outputDeviceCoords[3] - outputDeviceCoords[1]) * (dNewOutputWorldBRY-m_dRendererWorldTLY))/(m_dRendererWorldBRY - m_dRendererWorldTLY)) + outputDeviceCoords[1];
			outputDeviceCoords[0] = deviceCoords[0];
			outputDeviceCoords[1] = deviceCoords[1];
			outputDeviceCoords[2] = deviceCoords[2];
			outputDeviceCoords[3] = deviceCoords[3];

			return TRUE;
		}
		else	//Zooming out, so stretch whole image to only part of the device view
		{
			outputImageCoords[0] = 0;
			outputImageCoords[1] = 0;
			outputImageCoords[2] = m_nRendererWidth;
			outputImageCoords[3] = m_nRendererHeight;

			return TRUE;
		}
	}

	return FALSE;	//don't use StretchBlt
}

/**
 * Calculate the device coordinates of a given view
 * Internal only. Given the current view, calculate the device
 * coordinates of a new view based on the original.
 * PUT PARAMTERS IN!
 * @see AdjustExtents()
 * @return void.
 */
void inline CNCSRenderer::CalculateDeviceCoords(
						INT32 nDeviceTLX, INT32 nDeviceTLY,INT32 nDeviceBRX, INT32 nDeviceBRY,
						IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY,
						IEEE8 dNewWorldTLX, IEEE8 dNewWorldTLY, IEEE8 dNewWorldBRX, IEEE8 dNewWorldBRY,
						INT32 *pnDeviceTLX, INT32 *pnDeviceTLY,	INT32 *pnDeviceBRX, INT32 *pnDeviceBRY)
{
	*pnDeviceTLX = (INT32)(((dNewWorldTLX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(IEEE8)(nDeviceBRX-nDeviceTLX)+0.5);
	*pnDeviceTLY = (INT32)(((dNewWorldTLY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(IEEE8)(nDeviceBRY-nDeviceTLY)+0.5);
	*pnDeviceBRX = (INT32)(((dNewWorldBRX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(IEEE8)(nDeviceBRX-nDeviceTLX)+0.5);
	*pnDeviceBRY = (INT32)(((dNewWorldBRY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(IEEE8)(nDeviceBRY-nDeviceTLY)+0.5);
}

void CNCSRenderer::calculateDeviceCoords(int nDeviceTLX, int nDeviceTLY,
										 int nDeviceBRX, int nDeviceBRY,
										 double dWorldTLX, double dWorldTLY,
										 double dWorldBRX, double dWorldBRY,
										 double outputDeviceCoords[4],
										 double dRendererWorldTLX, double dRendererWorldTLY,
										 double dRendererWorldBRX, double dRendererWorldBRY)
{
	outputDeviceCoords[0] = ((dRendererWorldTLX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(double)(nDeviceBRX-nDeviceTLX);
    outputDeviceCoords[1] = ((dRendererWorldTLY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(double)(nDeviceBRY-nDeviceTLY);
    outputDeviceCoords[2] = ((dRendererWorldBRX - dWorldTLX)/(dWorldBRX-dWorldTLX))*(double)(nDeviceBRX-nDeviceTLX);
    outputDeviceCoords[3] = ((dRendererWorldBRY - dWorldTLY)/(dWorldBRY-dWorldTLY))*(double)(nDeviceBRY-nDeviceTLY);
}

void CNCSRenderer::calculateImageCoords(double dDevice1TLX, double dDevice1TLY,
										double dDevice1BRX, double dDevice1BRY,
										double dImageWidth, double dImageHeight,
										double dDevice2TLX, double dDevice2TLY,
										double dDevice2BRX, double dDevice2BRY,
										double outputImageCoords[4]) {
    double outputImageWidth, outputImageHeight;

    outputImageWidth  = ((dDevice2BRX-dDevice2TLX) * (dImageWidth))/(dDevice1BRX-dDevice1TLX)  ;
    outputImageHeight = ((dDevice2BRY-dDevice2TLY) * (dImageHeight))/(dDevice1BRY-dDevice1TLY)  ;

    outputImageCoords[0] = ((dDevice2TLX-dDevice1TLX)  * (outputImageWidth))/(dDevice2BRX-dDevice2TLX) ;
    outputImageCoords[1] = ((dDevice2TLY-dDevice1TLY)  * (outputImageHeight))/(dDevice2BRY-dDevice2TLY) ;
    outputImageCoords[2] = outputImageCoords[0] + outputImageWidth;
    outputImageCoords[3] = outputImageCoords[1] + outputImageHeight;
}

/**
 * Calculate the screen extents that the DrawImage function will draw into
 * Internal only. Given the current view, calculate the device
 * coordinates of a new view based on the original.
 * PUT PARAMTERS IN!
 * @see DrawImage()
 * @return void.
 */
void CNCSRenderer::DrawingExtents(LPRECT pClipRect,
								   IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY,
								   LPRECT pNewClipRect)		/**[09]**/

{
	INT32 nDeviceTLX, nDeviceTLY, nDeviceBRX, nDeviceBRY;
	INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;
	ConvertWorldToDataset(dWorldTLX, dWorldTLY, &nDatasetTLX, &nDatasetTLY);
	ConvertWorldToDataset(dWorldBRX, dWorldBRY, &nDatasetBRX, &nDatasetBRY);

	INT32 dAdjustedDatasetTLX, dAdjustedDatasetTLY, dAdjustedDatasetBRX, dAdjustedDatasetBRY;
	INT32 nAdjustedDeviceTLX, nAdjustedDeviceTLY, nAdjustedDeviceBRX, nAdjustedDeviceBRY;
	AdjustExtents(pClipRect->right - pClipRect->left, pClipRect->bottom - pClipRect->top,
				  nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY,
				  &dAdjustedDatasetTLX, &dAdjustedDatasetTLY, &dAdjustedDatasetBRX, &dAdjustedDatasetBRY,
				  &nAdjustedDeviceTLX, &nAdjustedDeviceTLY, &nAdjustedDeviceBRX, &nAdjustedDeviceBRY);

	// Check to see if the renderers current bitmap extents, match this draw, if so do a direct blit
	if ((dAdjustedDatasetTLX == m_nRendererDatasetTLX) && 
		(dAdjustedDatasetTLY == m_nRendererDatasetTLY) &&
		(dAdjustedDatasetBRX == m_nRendererDatasetBRX) && 
		(dAdjustedDatasetBRY == m_nRendererDatasetBRY) &&
		(nAdjustedDeviceBRX - nAdjustedDeviceTLX == m_nRendererWidth) &&
		(nAdjustedDeviceBRY - nAdjustedDeviceTLY == m_nRendererHeight)) {
		
		pNewClipRect->left	 = m_nAdjustedXOffset + pClipRect->left;
		pNewClipRect->top    = m_nAdjustedYOffset + pClipRect->top;
		pNewClipRect->right  = pNewClipRect->left + m_nAdjustedViewWidth;
		pNewClipRect->bottom = pNewClipRect->top  + m_nAdjustedViewHeight;
	}
	else {
		INT32 nRendererDatasetWidth = m_nRendererDatasetBRX - m_nRendererDatasetTLX;
		INT32 nRendererDatasetHeight = m_nRendererDatasetBRY - m_nRendererDatasetTLY;
		INT32 nNewDatasetWidth = dAdjustedDatasetBRX - dAdjustedDatasetTLX;
		INT32 nNewDatasetHeight = m_nRendererDatasetBRY - m_nRendererDatasetTLY;

		// Calculate where the old world coordinates from the current rendered image, 
		// fit to the new screen world coordinates.
		CalculateDeviceCoords(pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom,
							  dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY,
							  m_dRendererWorldTLX, m_dRendererWorldTLY, m_dRendererWorldBRX, m_dRendererWorldBRY,
							  &nDeviceTLX, &nDeviceTLY, &nDeviceBRX, &nDeviceBRY);

		pNewClipRect->left	 = nDeviceTLX + pClipRect->left;
		pNewClipRect->top    = nDeviceTLY + pClipRect->top;
		pNewClipRect->right  = pNewClipRect->left + abs(nDeviceBRX - nDeviceTLX);
		pNewClipRect->bottom = pNewClipRect->top  + abs(nDeviceBRY - nDeviceTLY);
	}
}

/**
 * Draw (blit) the internal buffer to the screen.
 * Draw ecw imagery to the screen, using the given extents. The extens do not
 * necessarilly have to match the SetView() extents. If they do, the entire image is
 * drawn. If they don't, only the intersection between the input extents and the
 * amount of imagery in the input buffer is draw.
 * @param hDeviceContext A win32 device context
 * @param pClipRect A pointer to a clip rect describing the width and height of the draw area.
 * @param dWorldTLX The top left X world coordinate of the device
 * @param dWorldTLY The top left Y world coordinate of the device
 * @param dWorldBRX The bottom right X world coordinate of the device
 * @param dWorldBRY The bottom right Y world coordinate of the device
 * @see ReadImage() 
 * @see SetView()
 * @return void.
 */
NCSError CNCSRenderer::DrawImage(HDC hDeviceContext, LPRECT pClipRect,
								IEEE8 dWorldTLX, IEEE8 dWorldTLY, IEEE8 dWorldBRX, IEEE8 dWorldBRY )
{
	BOOLEAN bStretch = FALSE;
	BOOLEAN bSpecialBlt = FALSE;	/**[16]**/
	int width, height;
	int nRet;
	NCSError nError = NCS_SUCCESS;

	INT32 nDeviceTLX = 0;
	INT32 nDeviceTLY = 0;
	INT32 nDeviceBRX = 0;
	INT32 nDeviceBRY = 0;

	// Nothing open, nothing to draw
	if (!m_bIsOpen) {
		return NCS_FILE_NOT_OPEN;								/**[11]**/
	}

	NCSMutexBegin(&m_DrawMutex);

	width = pClipRect->right - pClipRect->left;
	height = pClipRect->bottom - pClipRect->top;

	INT32 dAdjustedDatasetTLX=0, dAdjustedDatasetTLY=0, dAdjustedDatasetBRX=0, dAdjustedDatasetBRY=0;
	INT32 nAdjustedDeviceTLX=0, nAdjustedDeviceTLY=0, nAdjustedDeviceBRX=0, nAdjustedDeviceBRY=0;
	INT32 nDatasetTLX=0, nDatasetTLY=0, nDatasetBRX=0, nDatasetBRY=0;
	
	if (m_bIsProgressive) {
		ConvertWorldToDataset(dWorldTLX, dWorldTLY, &nDatasetTLX, &nDatasetTLY);
		ConvertWorldToDataset(dWorldBRX, dWorldBRY, &nDatasetBRX, &nDatasetBRY);

		AdjustExtents(pClipRect->right - pClipRect->left, pClipRect->bottom - pClipRect->top,
					  nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY,
					  &dAdjustedDatasetTLX, &dAdjustedDatasetTLY, &dAdjustedDatasetBRX, &dAdjustedDatasetBRY,
					  &nAdjustedDeviceTLX, &nAdjustedDeviceTLY, &nAdjustedDeviceBRX, &nAdjustedDeviceBRY);

		// Check to see if the renderers current bitmap extents, match this draw, if so do a direct blit
#if defined(MACINTOSH) || defined(MACOSX)
		if(0) 	// For the mac always use the stretch case as it does not
				// have seperate draw function.
#else
		if ((dAdjustedDatasetTLX == m_nRendererDatasetTLX) && 
			(dAdjustedDatasetTLY == m_nRendererDatasetTLY) &&
			(dAdjustedDatasetBRX == m_nRendererDatasetBRX) && 
			(dAdjustedDatasetBRY == m_nRendererDatasetBRY) &&
			(nAdjustedDeviceBRX - nAdjustedDeviceTLX == m_nRendererWidth) &&
			(nAdjustedDeviceBRY - nAdjustedDeviceTLY == m_nRendererHeight))
#endif		
			{
			width = m_nAdjustedViewWidth;
			height = m_nAdjustedViewHeight;
			bStretch = FALSE;
		}
		else {
			INT32 nRendererDatasetWidth = m_nRendererDatasetBRX - m_nRendererDatasetTLX;
			INT32 nRendererDatasetHeight = m_nRendererDatasetBRY - m_nRendererDatasetTLY;
			INT32 nNewDatasetWidth = dAdjustedDatasetBRX - dAdjustedDatasetTLX;
			INT32 nNewDatasetHeight = m_nRendererDatasetBRY - m_nRendererDatasetTLY;

			// Calculate where the old world coordinates from the current rendered image, 
			// fit to the new screen world coordinates.
			CalculateDeviceCoords(pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom,
								  dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY,
								  m_dRendererWorldTLX, m_dRendererWorldTLY, m_dRendererWorldBRX, m_dRendererWorldBRY,
								  &nDeviceTLX, &nDeviceTLY, &nDeviceBRX, &nDeviceBRY);

#ifdef WIN32
			//if supersampling have to do a stretch
			if (m_nRendererWidth == width && m_nRendererHeight == height)	/**[16]**/
			{
				INT32 dTLX = (INT32)floor((dWorldTLX - m_dOriginX)/m_dCellIncrementX);
				INT32 dTLY = (INT32)floor((dWorldTLY - m_dOriginY)/m_dCellIncrementY);
				INT32 dBRX = (INT32)ceil((dWorldBRX - m_dOriginX)/m_dCellIncrementX);
				INT32 dBRY = (INT32)ceil((dWorldBRY - m_dOriginY)/m_dCellIncrementY);

				if (m_nRendererDatasetBRX-m_nRendererDatasetTLX == dBRX-dTLX &&
					m_nRendererDatasetBRY-m_nRendererDatasetTLY == dBRY-dTLY)
				{
					bSpecialBlt = TRUE;
					bStretch = FALSE;
				}
				else
					bStretch = TRUE;
			}
			else
#endif //WIN32
				bStretch = TRUE;

			width = m_nAdjustedViewWidth;
			height = m_nAdjustedViewHeight;
		}
	} else {
		// The non progressive case, just find the actual dataset units,
		//  calculate the actual device coords and do a stretch.

		CalculateDeviceCoords(pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom,
							  dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY,
							  m_dAdjustedWorldTLX, m_dAdjustedWorldTLY, m_dAdjustedWorldBRX, m_dAdjustedWorldBRY,
							  &nDeviceTLX, &nDeviceTLY, &nDeviceBRX, &nDeviceBRY);

		bStretch = TRUE;
	}

	m_nBytesPerLine = NCS_WIDTH_BYTES(width*3*sizeof(UINT8));

	if (!m_pRGBA) {
		if (!m_bIsProgressive) {
#ifdef _DEBUG																/**[12]**/
			::MessageBox(NULL, NCS_T("There is no RGB information to draw to window. ReadImage() must be called in non progressive mode before drawing can occur."), NCS_T("NCSRenderer"), MB_OK);
#endif																		/**[12]**/	
			nError = NCS_ECW_ERROR;											/**[12]**/
			NCSFormatErrorText(NCS_ECW_ERROR, "There is no RGB information to draw to window. ReadImage() must be called in non progressive mode before drawing can occur.");	/**[12]**/
		}
		// If its progressive, this just means we got a draw call back before a callback from the ecw lib, so its OK to do nothing.
		NCSMutexEnd(&m_DrawMutex);
		return nError;
	}

#ifdef WIN32
	// Copy the rgb buffer into the bitmap and blit it to the dc
	if (m_bIsProgressive) {
		CreateDIBAndPallete( hDeviceContext, m_nRendererWidth, m_nRendererHeight);
	} else {
		CreateDIBAndPallete(hDeviceContext, m_nAdjustedViewWidth, m_nAdjustedViewHeight);
	}
	
	int nSavedDC = SaveDC(hDeviceContext);

#if !defined(_WIN32_WCE)
	SetStretchBltMode(hDeviceContext, COLORONCOLOR);
#endif


	DWORD dwCaps = GetDeviceCaps(hDeviceContext, RASTERCAPS);
	nRet = GDI_ERROR;
//char buf[1024];
//sprintf(buf, "DC: %ld,%ld %ldx%ld\r\n", nDeviceTLX, nDeviceTLY, nDeviceBRX, nDeviceBRY);
//OutputDebugStringA(buf);
	if(!bStretch) { 
#if !defined(_WIN32_WCE)
		if(dwCaps & RC_DIBTODEV && !m_bAlternateDraw) {
			if (bSpecialBlt) {														/**[16]**/
				nRet = SetDIBitsToDevice(hDeviceContext,							/**[16]**/
										 nDeviceTLX + pClipRect->left,
										 nDeviceTLY + pClipRect->top,
										 abs(nDeviceBRX - nDeviceTLX),
										 abs(nDeviceBRY - nDeviceTLY),
										 0,
										 0,
										 0,
										 abs(m_pbmInfo->bmiHeader.biHeight),
										 m_pBitmapImage,
										 m_pbmInfo,
										 DIB_RGB_COLORS);
			}
			else
			{
				nRet = SetDIBitsToDevice(hDeviceContext,
										 m_nAdjustedXOffset + pClipRect->left,
										 m_nAdjustedYOffset + pClipRect->top,
										 m_nAdjustedViewWidth,
										 m_nAdjustedViewHeight,
										 0,
										 0,
										 0,
										 abs(m_pbmInfo->bmiHeader.biHeight),
										 m_pBitmapImage,
										 m_pbmInfo,
										 DIB_RGB_COLORS);
			}
		}
		if(nRet == GDI_ERROR) {
			_RPT0(_CRT_WARN,"SetDIBitsToDevice Failed or not available\n");
#endif
			HDC hMemDC = CreateCompatibleDC(hDeviceContext);
		
			if(hMemDC) {
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)m_hBitmap);

				if (bSpecialBlt) {													/**[16]**/
					if(BitBlt(hDeviceContext,										/**[16]**/	
							  nDeviceTLX + pClipRect->left,
							  nDeviceTLY + pClipRect->top, 
							  abs(nDeviceBRX - nDeviceTLX),
							  abs(nDeviceBRY - nDeviceTLY),
							  hMemDC,
							  0, 
							  abs(m_pbmInfo->bmiHeader.biHeight) - abs(nDeviceBRY - nDeviceTLY) - 0,
							  SRCCOPY)) {
						nRet = m_nAdjustedViewHeight;
					} else {
						nRet = GDI_ERROR;
					}
				}
				else {
					if(BitBlt(hDeviceContext, 
							  m_nAdjustedXOffset + pClipRect->left,
							  m_nAdjustedYOffset + pClipRect->top, 
							  m_nAdjustedViewWidth,
							  m_nAdjustedViewHeight,
							  hMemDC,
							  0, 
							  abs(m_pbmInfo->bmiHeader.biHeight) - m_nAdjustedViewHeight - 0,
							  SRCCOPY)) {
						nRet = m_nAdjustedViewHeight;
					} else {
						nRet = GDI_ERROR;
					}
				}
				if(hOldBitmap) {
					SelectObject(hMemDC, (HGDIOBJ)hOldBitmap);
				}
				DeleteDC(hMemDC);
			}
#if !defined(_WIN32_WCE)
		}
#endif
	} else {
#if !defined(_WIN32_WCE)
		if(!m_bUsingAlphaBand && dwCaps & RC_STRETCHDIB && !m_bAlternateDraw) {
			nRet = StretchDIBits(hDeviceContext,
								nDeviceTLX + pClipRect->left,
								nDeviceTLY + pClipRect->top,
								abs(nDeviceBRX - nDeviceTLX),
								abs(nDeviceBRY - nDeviceTLY),
								0,
								0,
								m_nRendererWidth,
								m_nRendererHeight,
								m_pBitmapImage,
								m_pbmInfo,
								DIB_RGB_COLORS,
								SRCCOPY);
		}
		if(nRet == GDI_ERROR) {
			_RPT0(_CRT_WARN,"StretchDIBits Failed or not available\n");
#endif
			HDC hMemDC = CreateCompatibleDC(hDeviceContext);
			if(hMemDC) {
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)m_hBitmap);

				if(CNCSRenderer::StretchBlt(hDeviceContext,
							  nDeviceTLX + pClipRect->left,
							  nDeviceTLY + pClipRect->top,
							  abs(nDeviceBRX - nDeviceTLX),
							  abs(nDeviceBRY - nDeviceTLY),
							  hMemDC,
							  0,
							  abs(m_pbmInfo->bmiHeader.biHeight) - m_nRendererHeight - 0,
							  m_nRendererWidth,
							  m_nRendererHeight))
				{
					nRet = m_nRendererHeight;
				} else {
					nRet = GDI_ERROR;
				}
				if(hOldBitmap) {
					SelectObject(hMemDC, (HGDIOBJ)hOldBitmap);
				}
				DeleteDC(hMemDC);
			}
#if !defined(_WIN32_WCE)
		}
#endif
	}
	if(nRet == GDI_ERROR) {
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, 
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL);
		_RPT1(_CRT_WARN,"Stretch/SetDIBits Failed : %s", lpMsgBuf);
	}

	// Fill in the background area.
	if (!m_bIsTransparent) {													/**[14]**/											
		if ((m_nAdjustedXOffset != 0) || (m_nAdjustedYOffset !=0) || 
			(m_nAdjustedViewWidth != pClipRect->right - pClipRect->left) || 
			(m_nAdjustedViewHeight != pClipRect->bottom - pClipRect->top)) {

			// Create a pen and solid brush to fill in the background
			HPEN hPen = CreatePen(PS_SOLID, 0, m_nBackgroundColor );
			HPEN hPenOld = (HPEN)SelectObject(hDeviceContext, hPen);
			HBRUSH hBrush = CreateSolidBrush(m_nBackgroundColor);
			HBRUSH hBrOld = (HBRUSH)SelectObject(hDeviceContext, hBrush);

			// Clip the image area.
			if (m_bIsProgressive)
			{
				RECT excludeRect;
				DrawingExtents(pClipRect, dWorldTLX, dWorldTLY, dWorldBRX, dWorldBRY, &excludeRect);
				ExcludeClipRect(hDeviceContext, excludeRect.left, excludeRect.top, excludeRect.right, excludeRect.bottom);
			}
			else
				ExcludeClipRect(hDeviceContext, nDeviceTLX, nDeviceTLY, nDeviceBRX, nDeviceBRY);

			// Clip the image area.
			/*ExcludeClipRect(hDeviceContext,
					(int)m_nAdjustedXOffset,
					(int)m_nAdjustedYOffset,
					(int)m_nAdjustedXOffset + m_nAdjustedViewWidth,
					(int)m_nAdjustedYOffset + m_nAdjustedViewHeight);*/

			Rectangle(hDeviceContext, pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom);

			SelectObject(hDeviceContext, hBrOld);
			SelectObject(hDeviceContext, hPenOld);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
	}

#ifdef _DEBUG
			// Draw a RED rectangle around the plugin
#if !defined(_WIN32_WCE)
			LOGBRUSH brush;
			brush.lbStyle = BS_HOLLOW;
			HBRUSH hBrush = CreateBrushIndirect(&brush);
#else
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
#endif
			HPEN hPen = CreatePen( PS_SOLID, 1, RGB(255,0,0));
			HPEN hOldPen = (HPEN)SelectObject(hDeviceContext, hPen);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hDeviceContext, hBrush);

			Rectangle(hDeviceContext, pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom);

			SelectObject(hDeviceContext, hOldBrush);
			SelectObject(hDeviceContext, hOldPen);
			DeleteObject(hOldBrush);
			DeleteObject(hPen);
#endif

	RestoreDC(hDeviceContext, nSavedDC);
			
#endif //WIN32

#if defined(MACINTOSH) || defined(MACOSX)
	Rect rectDest; //= ((GrafPtr)gpWindow)->portRect;
	Rect rectSource; //= ((GrafPtr)gpWindow)->portRect;
	long time;
	short i;
	BOOLEAN bResult=TRUE;

	// Copy the rgb buffer into the bitmap and blit it to the dc
	if (m_bIsProgressive) {
		//CreateDIBAndPallete( hDeviceContext, m_nRendererWidth, m_nRendererHeight);
		bResult = CreatePixMapAndPallete( (GrafPtr)hDeviceContext, m_nRendererWidth, m_nRendererHeight );
	} else {
		bResult = CreatePixMapAndPallete( (GrafPtr)hDeviceContext, m_nAdjustedViewWidth, m_nAdjustedViewHeight );
	}
	
	if( !bResult )
		return NCS_COULDNT_ALLOC_MEMORY;
	
	/*if( !bStretch ) {
		rectDest.left = m_nAdjustedXOffset + pClipRect->left;
		rectDest.top = m_nAdjustedYOffset + pClipRect->top;
		rectDest.right = m_nAdjustedViewWidth;
		rectDest.bottom = m_nAdjustedViewHeight;
	} else {*/
	
		GetPortBounds(m_hLocalMemDC, &rectSource);
		
		rectDest.left = nDeviceTLX;// + pClipRect->left;
		rectDest.top = nDeviceTLY;// + pClipRect->top;
		rectDest.right = nDeviceBRX;
		rectDest.bottom = nDeviceBRY;
				
	//}

    RGBColor	cblack = { 0x0000, 0x0000, 0x0000 };
    RGBColor	cwhite = { 0xFFFF, 0xFFFF, 0xFFFF };
    RGBForeColor( &cblack );
    RGBBackColor( &cwhite );

	CopyBits( GetPortBitMapForCopyBits( m_hLocalMemDC ), GetPortBitMapForCopyBits( hDeviceContext ),
		&rectSource, &rectDest, srcCopy, 0L);
	
	SInt16 qdError = QDError ();
	
	if( qdError == noErr ) {
		nError = NCS_SUCCESS;
	} else {
		nError = NCS_COULDNT_ALLOC_MEMORY;
	}

#endif //MACINTOSH
	NCSMutexEnd(&m_DrawMutex);

	return nError;
}

#ifdef WIN32
	bool CNCSRenderer::StretchBlt(
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
		)
	{
		if( m_pAlphaBlend && m_bUsingAlphaBand ) {
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xFF;  // 0x00 (transparent) through 0xFF (opaque)
											// want bitmap alpha, so no constant.
			bf.AlphaFormat = AC_SRC_ALPHA;  // Use bitmap alpha

			return (*(BOOL(__stdcall *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION))m_pAlphaBlend)
				(hdcDest,
				nXOriginDest,
				nYOriginDest,
				nWidthDest,
				nHeightDest,
				hdcSrc,
				nXOriginSrc,
				nYOriginSrc,
				nWidthSrc,
				nHeightSrc,
				bf)?true:false;
		} else {
			return ::StretchBlt(hdcDest,
				nXOriginDest,
				nYOriginDest,
				nWidthDest,
				nHeightDest,
				hdcSrc,
				nXOriginSrc,
				nYOriginSrc,
				nWidthSrc,
				nHeightSrc,
				SRCCOPY)?true:false;
		}
	}

BOOLEAN CNCSRenderer::CreateDIBAndPallete(HDC hDeviceContext, INT32 nWidth, INT32 nHeight)
{
	INT32 nHeaderSize;
	INT32 nLine;
	INT32 nCell;
	INT32 nColors = 0;
	BOOLEAN bChangeInSize = FALSE;
#ifdef DIB_FIX
	INT32 nPrevDCBitDepth = m_nDCBitDepth;	//save previous depth for comparison	/**[10]**/
#endif

	if ((m_nDCWidth!= nWidth) ||(m_nDCHeight != nHeight) || (m_pbmInfo == NULL)
#ifdef NCS_HIST_AND_LUT_SUPPORT
		|| (m_bLutChanged && m_bApplyLUTs)
#endif // NCS_HIST_AND_LUT_SUPPORT
		) {
		bChangeInSize = TRUE;
	}

	m_nDCWidth = nWidth;
	m_nDCHeight = nHeight;

	if(GetDeviceCaps(hDeviceContext, TECHNOLOGY) == DT_RASPRINTER) {
		// Printing
		m_nDCBitDepth = 24;
	} else {
		m_nDCBitDepth = GetDeviceCaps(hDeviceContext, PLANES) * GetDeviceCaps(hDeviceContext, BITSPIXEL);
		if(m_nDCBitDepth <= 8) {
			nColors = 256;
			m_nDCBitDepth = 8;	/**[01]**/
		}
	}

	if (m_nDCBitDepth != nPrevDCBitDepth)
		bChangeInSize = TRUE;

	if (bChangeInSize) {
		if(m_hBitmap) {
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
		if(m_pbmInfo) {
			NCSFree(m_pbmInfo);
			m_pbmInfo = (BITMAPINFO*)NULL;
		}
	}	

#ifdef DIB_FIX
	//If the bit depth hasn't changed and no new image data has arrived
	//we don't have to do anything further in this function.
	if (bChangeInSize == FALSE && nPrevDCBitDepth == m_nDCBitDepth && m_bCreateNewDIB == FALSE)		/**[10]**/
		return TRUE;														/**[10]**/	
#endif

	if (bChangeInSize) {
		nHeaderSize = sizeof(BITMAPINFOHEADER) + nColors * sizeof(RGBQUAD);
		m_pbmInfo = (BITMAPINFO*)NCSMalloc(nHeaderSize + 3 * sizeof(DWORD), TRUE);
	}
	if(!m_pbmInfo) {
		DestroyDibAndPalette();
		return(FALSE);
	}
		// Note: Extra because of BITFIELDS for 16bit format
		// dib_header_size gets reset below if this format.
	m_pbmInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pbmInfo->bmiHeader.biWidth = nWidth;
#ifndef DIB_FIX
	m_pbmInfo->bmiHeader.biHeight = -nHeight;
#else
	m_pbmInfo->bmiHeader.biHeight = nHeight;
#endif
	m_pbmInfo->bmiHeader.biPlanes = 1;
	m_pbmInfo->bmiHeader.biBitCount = (WORD)m_nDCBitDepth;
	m_pbmInfo->bmiHeader.biSizeImage = NCS_WIDTH_BYTES((DWORD)nWidth * m_nDCBitDepth) * nHeight;
	m_pbmInfo->bmiHeader.biCompression = BI_RGB;
	
#if !defined(_WIN32_WCE)
	if(m_nDCBitDepth == 16) {
		PIXELFORMATDESCRIPTOR pfd;
		HDC hPFDHDC = hDeviceContext;
		int i = 1;
		pfd.nSize = sizeof(pfd);

		/*
		** On some drivers DescribePixelFormat() always fails on memory DC's,
		** so we put this hacky check in to see if it's really 15bit (RGB555).
		** There doesn't seem to be any other way to find this out.
		*/
		if(!m_hOpenGLDLL) {
			/*
			** This may seem pretty pointless as we don't use the instance handle at all.
			** In fact, this is an optimisation to prevent GDI doing it itself every time
			** DescribePixelFormat() is called.  Otherwise opengl32.dll is loaded/unloaded 
			** every time we call DescribePixelFormat() - very sloooow.
			*/
			m_hOpenGLDLL = LoadLibraryA("opengl32.dll");
		}
		if(DescribePixelFormat(hPFDHDC,
							   i,
							   sizeof(PIXELFORMATDESCRIPTOR),
							   &pfd) == 0) {
			COLORREF cOrig;
			COLORREF cDst;
			RECT rect;

			GetClipBox(hDeviceContext, &rect);

			cOrig = GetPixel(hDeviceContext, rect.left, rect.top);
			cDst = SetPixel(hDeviceContext, rect.left, rect.top, RGB(0x00, 0x04, 0x00));
			if(cDst != 0x04) {
				m_nDCBitDepth = 15;
			}
			SetPixel(hDeviceContext, rect.left, rect.top, cOrig);
		} else {
			while(DescribePixelFormat(hPFDHDC,
									  i,
									  sizeof(PIXELFORMATDESCRIPTOR),
									  &pfd)) {
				if(pfd.iPixelType == PFD_TYPE_RGBA) {
					INT32 bits = pfd.cRedBits + pfd.cGreenBits + pfd.cBlueBits;

					if(bits == 15) {
						/*
						** 5/5/5 RGB
						*/
						m_nDCBitDepth = 15;
						break;
					} else if(bits == 16) {
						/*
						** 5/6/5 RGB
						*/
						break;
					}
				}
				i++;
			}
		}
		if(hPFDHDC != hDeviceContext) {
			ReleaseDC(NULL, hPFDHDC);
			hPFDHDC = hDeviceContext;
		}
	}
#endif

	if(m_nDCBitDepth == 16) {
		DWORD *pColors = (DWORD*)m_pbmInfo->bmiColors;
			
		m_pbmInfo->bmiHeader.biCompression = BI_BITFIELDS;

		/*
		** This is the only 16bit bitfield format supported under win95.
		*/
		pColors[0] = 0xf800;	/* 5 Red */
		pColors[1] = 0x07e0;	/* 6 Green */
		pColors[2] = 0x001f;	/* 5 Blue */
	} else if(m_nDCBitDepth <= 8) {
		LOGPALETTE lPalette;
		HPALETTE hOldPalette = NULL;
		HPALETTE hPalette = NULL;
		int i;

		lPalette.palNumEntries = 1;
		lPalette.palVersion = 0x300;
		lPalette.palPalEntry[0].peRed = 0;
		lPalette.palPalEntry[0].peGreen = 0;
		lPalette.palPalEntry[0].peBlue = 0;
		lPalette.palPalEntry[0].peFlags = NULL;

		if((hPalette = CreatePalette(&lPalette)) != NULL) {
			PALETTEENTRY peEntries[256];

			hOldPalette = SelectPalette(hDeviceContext, hPalette, TRUE);

			memset(peEntries, 0, sizeof(peEntries));
			GetPaletteEntries(hOldPalette, 0, 256, peEntries);

			for(i = 0; i < 256; i++) {
				m_pbmInfo->bmiColors[i].rgbRed = peEntries[i].peRed;
				m_pbmInfo->bmiColors[i].rgbGreen = peEntries[i].peGreen;
				m_pbmInfo->bmiColors[i].rgbBlue = peEntries[i].peBlue;
				m_pbmInfo->bmiColors[i].rgbReserved = 0;

				if(m_pPaletteEntries && ((peEntries[i].peRed != m_pPaletteEntries[i].peRed) ||
										 (peEntries[i].peGreen != m_pPaletteEntries[i].peGreen) ||
										 (peEntries[i].peBlue != m_pPaletteEntries[i].peBlue) || 
										 (peEntries[i].peFlags != m_pPaletteEntries[i].peFlags))) {
					if(m_pColorTable) {
						NCSFree(m_pColorTable);
						m_pColorTable = NULL;
					}
				}
			}
			
			if(!m_pPaletteEntries) {
				m_pPaletteEntries = (PALETTEENTRY*)NCSMalloc(sizeof(peEntries), TRUE);
			}
			if(!m_pColorTable) {
				if((m_pColorTable = (UINT8*)NCSMalloc(65536, FALSE)) != NULL) {
					int r, g, b;

					/* initialize color_table[], red_table[], etc */
					for (r = 0; r < _MR; r++) {
						UINT32 red = (INT32)(((double)r / (double)(_MR - 1)) * 255.0);

						for (g = 0; g < _MG; g++) {
							UINT32 green = (INT32)(((double)g / (double)(_MG - 1)) * 255.0);

							for (b = 0; b < _MB; b++) {
								UINT32 blue = (INT32)(((double)b / (double)(_MB - 1)) * 255.0);
													
								m_pColorTable[_MIX( r, g, b )] = GetNearestPaletteIndex(hOldPalette, 
																					  RGB(red, green, blue));
							}
						}
					}
					memcpy(m_pPaletteEntries, peEntries, sizeof(peEntries));
				} else {
					DestroyDibAndPalette();
					return(FALSE);
				}
			}
			SelectPalette(hDeviceContext, hOldPalette, TRUE);
			hOldPalette = NULL;
			DeleteObject(hPalette);
			hPalette = NULL;
		}
		m_nDCBitDepth = 8;
	}
	if (bChangeInSize) {
		m_hBitmap = CreateDIBSection(hDeviceContext,
									 m_pbmInfo,
									 m_nDCBitDepth <= 8 ? DIB_PAL_COLORS : DIB_RGB_COLORS,
									 (void **)&m_pBitmapImage,
									 NULL, NULL);
	}

	if( !m_hBitmap || !m_pBitmapImage ) {
		return(FALSE);
	}

#ifdef DIB_FIX
//Only copy triplets to bitmap if bit depth changed from last call to this function
//or new image data has arrived
if (bChangeInSize == TRUE || nPrevDCBitDepth != m_nDCBitDepth || m_bCreateNewDIB == TRUE)		/**[10]**/
{
#endif
	switch(m_nDCBitDepth) {
		case 32:
#ifndef NCS_HIST_AND_LUT_SUPPORT
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8	*pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT32	*pImagePtr32 = (UINT32*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 32) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						*(pImagePtr32++) = (UINT32)(*pRGBTriplets |
												    (*(pRGBTriplets + 1) << 8) |
												    (*(pRGBTriplets + 2) << 16));
						pRGBTriplets += 3;
					}
				}
#else // NCS_HIST_AND_LUT_SUPPORT
			{
				if(!m_bApplyLUTs && !m_bUsingAlphaBand) {
					// Just do a mem copy
					for(nLine = 0; nLine < nHeight; nLine++) {
						UINT8	*pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
						UINT32	*pImagePtr32 = (UINT32*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 32) * nLine]);

						memcpy( pImagePtr32, pRGBA, nWidth*4 );
					}
				} else {
					int i;
					UINT8 rLut[256];
					UINT8 gLut[256];
					UINT8 bLut[256];

					if(m_bApplyLUTs) {
						for(i = 0; i < 256; i++) {
							rLut[i] = (m_LUTs[2][i]);
							gLut[i] = (m_LUTs[1][i]);
							bLut[i] = (m_LUTs[0][i]);			
						}
					} else {
						for(i = 0; i < 256; i++) {
							rLut[i] = i;
							gLut[i] = i;
							bLut[i] = i;			
						}
					}
					for(nLine = 0; nLine < nHeight; nLine++) {
						UINT8	*pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
						UINT32	*pImagePtr32 = (UINT32*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 32) * nLine]);

						for(nCell = 0; nCell < nWidth; nCell++) {
							UINT32 r = 0;
							UINT32 g = 0;
							UINT32 b = 0;
							UINT32 a = 0xFF;
							if( m_bUsingAlphaBand ) {
								a = *(pRGBA + 3);
								r = (rLut[(*pRGBA)]*a)/0xff;
								g = (gLut[(*(pRGBA + 1))]*a)/0xff;
								b = (bLut[(*(pRGBA + 2))]*a)/0xff;
							} else {
								r = rLut[(*pRGBA)];
								g = gLut[(*(pRGBA + 1))];
								b = bLut[(*(pRGBA + 2))];
							}

							*(pImagePtr32++) = (UINT32)( r | (g << 8) | (b << 16) | (a << 24) );

							pRGBA += 4;
						}
					}
				}
			}
#endif // NCS_HIST_AND_LUT_SUPPORT
			break;
			
		case 24:
		default:
#ifndef NCS_HIST_AND_LUT_SUPPORT
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT8 *pImagePtr8 = (UINT8*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 24) * nLine]);

					memcpy(pImagePtr8, pRGBTriplets, nWidth * 3);
				}
#else // NCS_HIST_AND_LUT_SUPPORT
				int i;
				UINT16 rLut[256];
				UINT16 gLut[256];
				UINT16 bLut[256];	

				if(m_bApplyLUTs) {				
					for(i = 0; i < 256; i++) {
						rLut[i] = ((UINT16)(m_LUTs[2][i])) >> 3;
						gLut[i] = (((UINT16)(m_LUTs[1][i])) & 0xf8) << 3;
						bLut[i] = (((UINT16)(m_LUTs[0][i])) & 0xf8) << 8;			
					}
				} else {
					for(int i = 0; i < 256; i++) {
						rLut[i] = (UINT16)i >> 3;
						gLut[i] = ((UINT16)i & 0xf8) << 3;
						bLut[i] = ((UINT16)i & 0xf8) << 8;			
					}
				}

				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
					UINT8 *pImagePtr8 = (UINT8*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 24) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						*(pImagePtr8) = rLut[*(pRGBA)];
						*(pImagePtr8 + 1) = gLut[*(pRGBA + 1)];
						*(pImagePtr8 + 2) = bLut[*(pRGBA + 2)];
						pImagePtr8 += 3;
						pRGBA+=4;
					}
				}
#endif // NCS_HIST_AND_LUT_SUPPORT
			break;

		case 16:
#ifndef NCS_HIST_AND_LUT_SUPPORT
					for(nLine = 0; nLine < nHeight; nLine++) {
						UINT8 *pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
						UINT16 *pImagePtr16 = (UINT16*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 16) * nLine]);

						for(nCell = 0; nCell < nWidth; nCell++) {
							*(pImagePtr16++) = (UINT16)((*pRGBTriplets >> 3) |
														((*(pRGBTriplets + 1) & 0xf8) << 3) |
														((*(pRGBTriplets + 2) & 0xf8) << 8));
							pRGBTriplets += 3;
						}
					}
#else // NCS_HIST_AND_LUT_SUPPORT
			{
				int i;
				UINT16 rLut[256];
				UINT16 gLut[256];
				UINT16 bLut[256];	

				if(m_bApplyLUTs) {				
					for(i = 0; i < 256; i++) {
						rLut[i] = ((UINT16)(m_LUTs[2][i])) >> 3;
						gLut[i] = (((UINT16)(m_LUTs[1][i])) & 0xf8) << 3;
						bLut[i] = (((UINT16)(m_LUTs[0][i])) & 0xf8) << 8;			
					}
				} else {
					for(int i = 0; i < 256; i++) {
						rLut[i] = (UINT16)i >> 3;
						gLut[i] = ((UINT16)i & 0xf8) << 3;
						bLut[i] = ((UINT16)i & 0xf8) << 8;			
					}
				}

				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
					UINT16 *pImagePtr16 = (UINT16*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 16) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						INT32 r = *pRGBA;
						INT32 g = *(pRGBA + 1);
						INT32 b = *(pRGBA + 2);

						*(pImagePtr16++) = (UINT16)(rLut[r] | gLut[g] | bLut[b]);
						pRGBA += 4;
					}
				}
			}
#endif // NCS_HIST_AND_LUT_SUPPORT
			break;

		case 15:
#ifndef NCS_HIST_AND_LUT_SUPPORT
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBA = (UINT8*)&(m_pRGBA[nWidth * 3 * nLine]);
					UINT16 *pImagePtr16 = (UINT16*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 16) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						*(pImagePtr16++) = (UINT16)((*pRGBA >> 3) |
												    ((*(pRGBA + 1) & 0xf8) << 2) |
												    ((*(pRGBA + 2) & 0xf8) << 7));
						pRGBA += 4;
					}
				}
#else // NCS_HIST_AND_LUT_SUPPORT
			{
				int i;
				UINT16 rLut[256];
				UINT16 gLut[256];
				UINT16 bLut[256];	

				if(m_bApplyLUTs) {				
					for(i = 0; i < 256; i++) {
						rLut[i] = ((UINT16)(m_LUTs[2][i])) >> 3;
						gLut[i] = (((UINT16)(m_LUTs[1][i])) & 0xf8) << 2;
						bLut[i] = (((UINT16)(m_LUTs[0][i])) & 0xf8) << 7;			
					}
				} else {
					for(int i = 0; i < 256; i++) {
						rLut[i] = (UINT16)i >> 3;
						gLut[i] = ((UINT16)i & 0xf8) << 2;
						bLut[i] = ((UINT16)i & 0xf8) << 7;			
					}
				}

				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
					UINT16 *pImagePtr16 = (UINT16*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 16) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						INT32 r = *pRGBA;
						INT32 g = *(pRGBA + 1);
						INT32 b = *(pRGBA + 2);

						*(pImagePtr16++) = (UINT16)(rLut[r] | gLut[g] | bLut[b]);
						pRGBA += 4;
					}
				}
			}
#endif // NCS_HIST_AND_LUT_SUPPORT
			break;

		case 8:
#ifdef NCS_HIST_AND_LUT_SUPPORT
				if(m_bApplyLUTs) {
					for(nLine = 0; nLine < nHeight; nLine++) {
						UINT8	*pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
						UINT8	*pImagePtr8 = (UINT8*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 8) * nLine]);

						for(nCell = 0; nCell < nWidth; nCell++) {
							UINT8 b = *pRGBA;
							UINT8 g = *(pRGBA + 1);
							UINT8 r = *(pRGBA + 2);

							pRGBA += 4;

							*(pImagePtr8++) = KERNEL_DITHER(nCell, nLine, m_LUTs[2][r], m_LUTs[1][g], m_LUTs[0][b]);
						}
					}
				} else {
#endif // NCS_HIST_AND_LUT_SUPPORT
					for(nLine = 0; nLine < nHeight; nLine++) {
						UINT8	*pRGBA = (UINT8*)&(m_pRGBA[nWidth * 4 * nLine]);
						UINT8	*pImagePtr8 = (UINT8*)&(m_pBitmapImage[WIDTHBYTES(nWidth * 8) * nLine]);

						for(nCell = 0; nCell < nWidth; nCell++) {
							UINT8 b = *pRGBA;
							UINT8 g = *(pRGBA + 1);
							UINT8 r = *(pRGBA + 2);

							pRGBA += 4;

							*(pImagePtr8++) = KERNEL_DITHER(nCell, nLine, r, g, b);
						}
					}	
#ifdef NCS_HIST_AND_LUT_SUPPORT
				}
#endif // NCS_HIST_AND_LUT_SUPPORT
				break;
	}
#ifdef DIB_FIX
}
#endif

#ifdef DIB_FIX
	m_bCreateNewDIB = FALSE;								/**[10]**/
#endif
#ifdef NCS_HIST_AND_LUT_SUPPORT
	m_bLutChanged = FALSE;
#endif // NCS_HIST_AND_LUT_SUPPORT
	return(TRUE);
}

BOOLEAN CNCSRenderer::DestroyDibAndPalette(void)
{
	if(m_hBitmap) {
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	if(m_pbmInfo) {
		NCSFree(m_pbmInfo);
		m_pbmInfo = NULL;
	}
	if(m_pColorTable) {
		NCSFree(m_pColorTable);
		m_pColorTable = NULL;
	}
	if(m_pPaletteEntries) {
		NCSFree(m_pPaletteEntries);
		m_pPaletteEntries = NULL;
	}
	return(TRUE);
}

#elif defined(MACINTOSH) || defined(MACOSX)

// returns GDHandle that window resides on (most of it anyway)
GDHandle CNCSRenderer::GetWindowDevice (WindowPtr pWindow)
{
	GrafPtr pgpSave;
	Rect rectWind, rectSect;
	long greatestArea, sectArea;
	GDHandle hgdNthDevice, hgdZoomOnThisDevice;
	
	GetPort (&pgpSave);
#if TARGET_API_MAC_CARBON
	SetPortWindowPort (pWindow);
	GetPortBounds(GetWindowPort(pWindow), &rectWind);
#else
	SetPort (pWindow);
	rectWind = pWindow->portRect;
#endif
	LocalToGlobal ((Point*)& rectWind.top);	// convert to global coordinates
	LocalToGlobal ((Point*)& rectWind.bottom);
	hgdNthDevice = GetDeviceList ();
	greatestArea = 0;	 // initialize to 0
	// check window against all gdRects in gDevice list and remember 
	//  which gdRect contains largest area of window}
	while (hgdNthDevice)
	{
		if (TestDeviceAttribute (hgdNthDevice, screenDevice))
			if (TestDeviceAttribute (hgdNthDevice, screenActive))
			{
				// The SectRect routine calculates the intersection 
				//  of the window rectangle and this gDevice 
				//  rectangle and returns TRUE if the rectangles intersect, 
				//  FALSE if they don't.
				SectRect(&rectWind, &(**hgdNthDevice).gdRect, &rectSect);
				// determine which screen holds greatest window area
				//  first, calculate area of rectangle on current device
				sectArea = (long)(rectSect.right - rectSect.left) * (rectSect.bottom - rectSect.top);
				if ( sectArea > greatestArea )
				{
					greatestArea = sectArea;	// set greatest area so far
					hgdZoomOnThisDevice = hgdNthDevice;	// set zoom device
				}
				hgdNthDevice = GetNextDevice(hgdNthDevice);
			}
	} 	// of WHILE
	SetPort (pgpSave);
	return hgdZoomOnThisDevice;
}

BOOLEAN CNCSRenderer::CreatePixMapAndPallete( GrafPtr pGPtr, INT32 nWidth, INT32 nHeight ) {

	BOOLEAN bChangeInSize = FALSE;

	if( (m_nDCWidth!= nWidth) || ( m_nDCHeight != nHeight) ){
		bChangeInSize = TRUE;
	}

	if( bChangeInSize ) {
		short wPixDepth = 32;//(**(GetPortPixMap(hDC))).pixelSize;
		Rect	tempRect1 = { 0,0,nHeight,nWidth };
		GDHandle hgdWindow = GetWindowDevice( (WindowPtr)pGPtr );  // may not work
		m_nDCWidth = nWidth;
		m_nDCHeight = nHeight;
		
		DestroyPixMapAndPallete();
		if (noErr != NewGWorld (&m_hLocalMemDC, wPixDepth, &tempRect1, NULL, hgdWindow, noNewDevice | keepLocal))
		{
			if (noErr != NewGWorld (&m_hLocalMemDC, wPixDepth, &tempRect1, NULL, hgdWindow, noNewDevice | useTempMem))
			{
				//SysBeep( 30 );
				m_hLocalMemDC = NULL;
				//BAD: we are out of mem
				return FALSE;
			}
		}
	}

	if ( bChangeInSize || m_bCreateNewDIB
#ifdef NCS_HIST_AND_LUT_SUPPORT
		|| (m_bLutChanged && m_bApplyLUTs)
#endif // NCS_HIST_AND_LUT_SUPPORT
		){
		INT32 nBytesPerRow;
		Ptr offBaseAddr = NULL;	/* Pointer to the off-screen pixel image */
		INT32 i,j,index;
		INT32 nLine;

		PixMapHandle hPixMap = GetGWorldPixMap(m_hLocalMemDC);
		offBaseAddr = GetPixBaseAddr( hPixMap );
		m_nDCBitDepth = 32;
		nBytesPerRow = ((*hPixMap)->rowBytes & 0x7FFF);//(nWidth * m_nDCBitDepth) / 8;
		
		m_bCreateNewDIB = FALSE;
#ifdef NCS_HIST_AND_LUT_SUPPORT
		m_bLutChanged = FALSE;
#endif // NCS_HIST_AND_LUT_SUPPORT
	
		// Fill PixMap with RGB data
		switch(m_nDCBitDepth) {
		case 32:
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8	*pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					index = nLine*nBytesPerRow;
					for(j=0, i=0; (j < nBytesPerRow) && (i < (nWidth * 3)); j+=4, i+=3) {
						offBaseAddr[index+j] = 0;
						offBaseAddr[index+j+1] = pRGBTriplets[i+2];
						offBaseAddr[index+j+2] = pRGBTriplets[i+1];
						offBaseAddr[index+j+3] = pRGBTriplets[i+0];
					}
				}
			break;
			
/*		case 24:	// other bit depths not tested and probably don't work
		default:
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT8 *pImagePtr8 = (UINT8*)&(offBaseAddr[WIDTHBYTES(nWidth * 24) * nLine]);

					memcpy(pImagePtr8, pRGBTriplets, nWidth * 3);
				}
			break;

		case 16:
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT16 *pImagePtr16 = (UINT16*)&(offBaseAddr[WIDTHBYTES(nWidth * 16) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						*(pImagePtr16++) = (UINT16)((*pRGBTriplets >> 3) |
												    ((*(pRGBTriplets + 1) & 0xf8) << 3) |
												    ((*(pRGBTriplets + 2) & 0xf8) << 8));
						pRGBTriplets += 3;
					}
				}
			break;

		case 15:
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8 *pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT16 *pImagePtr16 = (UINT16*)&(offBaseAddr[WIDTHBYTES(nWidth * 16) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						*(pImagePtr16++) = (UINT16)((*pRGBTriplets >> 3) |
												    ((*(pRGBTriplets + 1) & 0xf8) << 2) |
												    ((*(pRGBTriplets + 2) & 0xf8) << 7));
						pRGBTriplets += 3;
					}
				}
			break;

		case 8:
				for(nLine = 0; nLine < nHeight; nLine++) {
					UINT8	*pRGBTriplets = (UINT8*)&(m_pRGBTriplets[nWidth * 3 * nLine]);
					UINT8	*pImagePtr8 = (UINT8*)&(offBaseAddr[WIDTHBYTES(nWidth * 8) * nLine]);

					for(nCell = 0; nCell < nWidth; nCell++) {
						UINT8 b = *pRGBTriplets;
						UINT8 g = *(pRGBTriplets + 1);
						UINT8 r = *(pRGBTriplets + 2);

						pRGBTriplets += 3;
						
						//*(pImagePtr8++) = (b & ) & (g & ) & (r & );

						//*(pImagePtr8++) = KERNEL_DITHER(nCell, nLine, r, g, b);
					}
				}	
			break;*/
		default:
			return FALSE;
			break;
		}
	}

	return TRUE;
}
	

BOOLEAN CNCSRenderer::DestroyPixMapAndPallete( void ) {

	//if( m_hCTable ) DisposeHandle( (Handle)m_hCTable );
	/*if(m_hPixMap) {
		if( (*m_hPixMap)->pmTable ) DisposeHandle( (Handle)(*m_hPixMap)->pmTable );
		if( (*m_hPixMap)->baseAddr ) DisposePtr( (*m_hPixMap)->baseAddr );
		DisposePixMap( m_hPixMap );//DeleteObject(m_hBitmap);
		m_hPixMap = NULL;
	}*/
	if( m_hLocalMemDC ) DisposeGWorld( m_hLocalMemDC );
	m_hLocalMemDC = NULL;
	
	return TRUE;
}

#endif // defined(MACINTOSH)

/**
 * Set the background color.
 * In non-transparent mode, specifies the color of the device background, 
 * which will get filled before drawing occurs.
 * @param nBackgroundColor A 32bit color reference
 * @see SetTransparent() 
 * @see GetTransparent()
 * @return void.
 */
void CNCSRenderer::SetBackgroundColor(COLORREF nBackgroundColor)
{
	m_nBackgroundColor = nBackgroundColor;
}

#endif //LINUX

/**
 * Set the transparent property.
 * Specifies wheather or not the renderer is responsible for filling the
 * background before drawing imagery. When integrated with other applications
 * that draw layers, you most likely need to set transparency to FALSE and let
 * the application do the work. If implementing the renderer into a control on
 * its own, setting the transparency to TRUE allows the renderer to erase/paint
 * the client area before drawing, thus minimising the work the container must do.
 * @param bTransparent A boolean specifying the transparency value.
 * @see SetBackgroundColor() 
 * @see GetTransparent()
 * @return void.
 */
void CNCSRenderer::SetTransparent(BOOLEAN bTransparent)
{
	m_bIsTransparent=bTransparent;
}

/**
 * Get the transparent property.
 * @param pbTransparent A pointer to a BOOLEAN value that will reveive the property on return
 * @see SetBackgroundColor()
 * @see SetTransparent()
 * @return void.
 */
void CNCSRenderer::GetTransparent(BOOLEAN *pbTransparent)
{
	*pbTransparent = m_bIsTransparent;
}

#ifdef NCS_HIST_AND_LUT_SUPPORT

#if defined(POSIX)
#	define max(a,b) (b > a ? b : a)
#endif

/**
 * Setup the histogram bins.
 * @see CalcHistograms()
 * @see SetView()
 * @return BOOLEAN.
 */
BOOLEAN CNCSRenderer::SetupHistograms(void)
{
	if(m_bCalcHistograms) {
		NCSMutexBegin(&m_HistogramMutex);
		NCSFree(m_pHistograms);
		m_pHistograms = (Histogram*)NCSMalloc(NCSMax(3, m_nNumberOfBands) * sizeof(Histogram), TRUE);

		if(!m_pHistograms) {
			NCSMutexEnd(&m_HistogramMutex);
			return(FALSE);
		}
		NCSMutexEnd(&m_HistogramMutex);
	}
	return(TRUE);
}

/**
 * Enable/disable histogram calculations.
 * @param bEnable A BOOLEAN value to enable/disable histogram calculations (call before SetView())
 * @see SetupHistograms()
 * @see SetView()
 * @return void.
 */
BOOLEAN CNCSRenderer::CalcHistograms(BOOLEAN bEnable)
{
	m_bCalcHistograms = bEnable;
	return(m_bCalcHistograms);
}

/**
 * Get Calculated Histogram for a specific band.
 * @param nBand An INT32 band index to retrieve
 * @param Histogram A UINT32[256] Histogram array to fill
 * @see CalcHistograms()
 * @see SetView()
 * @see ReadLineBIL()
 * @return BOOLEAN.
 */
BOOLEAN	CNCSRenderer::GetHistogram(INT32 nBand, UINT32 Histogram[256])
{
	NCSMutexBegin(&m_HistogramMutex);
	if(m_pHistograms && nBand < NCSMax(3, m_nNumberOfBands)) {
		memcpy(Histogram, m_pHistograms[nBand], sizeof(UINT32) * 256);
		NCSMutexEnd(&m_HistogramMutex);
		return(TRUE);
	}
	NCSMutexEnd(&m_HistogramMutex);
	return(FALSE);
}

BOOLEAN CNCSRenderer::ApplyLUTs(BOOLEAN bEnable)
{
	m_bApplyLUTs = bEnable;
	return(m_bApplyLUTs);
}

BOOLEAN	CNCSRenderer::SetLUT(INT32 nBand, UINT8 Lut[256])
{
	bool bChanged = false;
	if(nBand >= 0 && nBand < 3) {
		for(int i = 0; i < 256; i++) {
			if(m_LUTs[nBand][i] != Lut[i]) {
				bChanged = true;
				m_LUTs[nBand][i] = Lut[i];
			}
		}
//		memcpy(m_LUTs[nBand], Lut, sizeof(Lut));
	} else {
		return(FALSE);
	}
	m_bLutChanged = bChanged;
	return(TRUE);
}

#endif // NCS_HIST_AND_LUT_SUPPORT
