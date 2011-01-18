/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSEcw.cpp $
** CREATED:  26/03/03 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSEcw wrapper 
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

// Need to define this to get COINIT_MULTITHREADED etc.
#define _WIN32_WINNT 0x0500

#include "NCSEcw.h"				/**[17]**/
#include "NCSJP2FileView.h"
#include "NCSJPCFileIOStream.h"

#include "NCSBuildNumber.h"

/*******************************************************
**	NCScbmSetFileView()	- set bands/extents/size for view area
**
**	Notes:
**	(1)	-	You can alter any or none of the parameters
**	(2)	-	You can't read from the view until at least one SetViewFile call is done
**	(3)	-	Only one thread fiddles with QMF Region at one time
**	(4) -	A refresh callback will be made immediately if all blocks are already available
**
********************************************************/

extern "C"
NCSError	NCScbmSetFileViewEx(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
			    UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Right in image coordinates
				UINT32 nSizeX, UINT32 nSizeY,	// Output view size in window pixels
				IEEE8 fTopX, IEEE8 fLeftY,		// Top-Left in world coordinates
				IEEE8 fBottomX, IEEE8 fRightY)	// Bottom-Right in world coordinates
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		CNCSError Error = pView->SetView(nBands, pBandList, 
										 nTopX, nLeftY, 
										 nBottomX, nRightY, 
										 nSizeX, nSizeY,
										 fTopX, fLeftY, 
										 fBottomX, fRightY);
		return(Error.GetErrorNumber());
	} else {
		return(NCS_INVALID_PARAMETER);
	}
}

extern "C"
NCSError	NCScbmSetFileView(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
				UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Left in image coordinates
				UINT32 nSizeX, UINT32 nSizeY)	// Output view size in window pixels
{
	return(NCScbmSetFileViewEx(pNCSFileView,
							   nBands,
							   pBandList,
							   nTopX, nLeftY,
							   nBottomX, nRightY,
							   nSizeX, nSizeY,
							   nTopX, nLeftY,
							   nBottomX, nRightY));
}

/*******************************************************
**	NCScbmReadFileBlockLocal()	- Read a physical block for the file
**
**	Notes:
**	-	Always reads the requested block from the local file
********************************************************/
#ifdef MACINTOSH
extern "C" Handle NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#else
extern "C" UINT8	*NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#endif
{
#ifdef NCSJPC_ECW_SUPPORT
	return(NCScbmReadFileBlockLocal_ECW(pNCSFile, nBlock, pBlockLength));
#else
	return(NULL);
#endif
}

extern "C" BOOLEAN NCScbmGetFileBlockSizeLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength, UINT64 *pBlockOffset )
{
#ifdef NCSJPC_ECW_SUPPORT
	return(NCScbmGetFileBlockSizeLocal_ECW(pNCSFile, nBlock, pBlockLength, pBlockOffset));
#else
	return(FALSE);
#endif
}

/*******************************************************
**	NCScbmReadViewLineBIL() - read a block from an ECW file in BIL format (UINT8)
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineBIL( NCSFileView *pNCSFileView, UINT8 **p_p_output_line)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(pView->ReadLineBIL(p_p_output_line));
	}
	return(NCSECW_READ_FAILED);
}

/*******************************************************
**	NCScbmReadViewLineBIL() - read a block from an ECW file in BIL format (UINT8)
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineBILEx( NCSFileView *pNCSFileView, NCSEcwCellType eType, void **p_p_output_line)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		switch (eType) {
			case NCSCT_UINT8 :
						return(pView->ReadLineBIL((UINT8**)p_p_output_line));
					break;
			case NCSCT_UINT16 :
						return(pView->ReadLineBIL((UINT16**)p_p_output_line));
					break;
			case NCSCT_UINT32 :
						return(pView->ReadLineBIL((UINT32**)p_p_output_line));
					break;
			case NCSCT_UINT64 :
						return(pView->ReadLineBIL((UINT64**)p_p_output_line));
					break;
			case NCSCT_INT8	:
						return(pView->ReadLineBIL((INT8**)p_p_output_line));
					break;
			case NCSCT_INT16 :
						return(pView->ReadLineBIL((INT16**)p_p_output_line));
					break;
			case NCSCT_INT32 :
						return(pView->ReadLineBIL((INT32**)p_p_output_line));
					break;
			case NCSCT_INT64 :
						return(pView->ReadLineBIL((INT64**)p_p_output_line));
					break;
			case NCSCT_IEEE4 :
						return(pView->ReadLineBIL((IEEE4**)p_p_output_line));
					break;
			case NCSCT_IEEE8 :
				return (NCSECW_READ_FAILED);//NCS_INVALID_PARAMETER); [18]
			default :
				return (NCSECW_READ_FAILED);//NCS_INVALID_PARAMETER); [18]
		}
	}
	return(NCSECW_READ_FAILED);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineRGB( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(pView->ReadLineRGB(pRGBTriplets));
	}
	return(NCSECW_READ_FAILED);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineRGBA( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(pView->ReadLineRGBA(pRGBTriplets));
	}
	return(NCSECW_READ_FAILED);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineBGRA( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(pView->ReadLineBGRA(pRGBTriplets));
	}
	return(NCSECW_READ_FAILED);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewLineBGR( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(pView->ReadLineBGR(pRGBTriplets));
	}
	return(NCSECW_READ_FAILED);
}

/*******************************************************
**	NCScbmReadViewFake() - Do a fake read on the View
** ONLY used for testing server performance. Used to
**	ensure cache is updated, to simulate a normal
**	client block request cache hit.
**	FIXME!! For now, does a top-down traverse to get
**	the block hits, which is different to the normal
**	inverse DWT which does a recursive by line approach,
**	and frees on a recursive line basis also.
**	Should not make much difference, but it is not EXACTLY
**	simulating reality...
********************************************************/
extern "C" NCSEcwReadStatus NCScbmReadViewFake( NCSFileView *pNCSFileView)
{
#ifdef NCSJPC_ECW_SUPPORT
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return(NCScbmReadViewFake_ECW(pView->GetNCSFileView()));
	}
#endif
	return(NCSECW_READ_FAILED);
}

/*******************************************************
**	NCScbmOpenFileView() -	Opens a file view
**
**	Notes:
**	-	File could be local or remote
**	-	Blocks until file can be opened
**	-	The RefreshCallback can be NULL or a client refresh
**		callback function to call when blocks have arrived
**	Special notes for the RefreshCallback:
**	(1)	You must set this for each FileView that you want to have refresh
**		calls received for.  If you don't set it, the FileView
**		will instead revert to the blocking timer when blocks are
**		requested.
**
**	Returns:	NCSError (cast to int).	FIXME: change return type to NCSError, along 
**				with return types for all SDK public funcs, and ship NCSError.h in SDK
**
********************************************************/

NCSError NCScbmOpenFileView(char *szUrlPath, NCSFileView **ppNCSFileView, /**[11]**/
					   NCSEcwReadStatus (*pRefreshCallback)(NCSFileView *pNCSFileView))
{
	CNCSJP2FileView *pView = new CNCSJP2FileView;
	*ppNCSFileView = (NCSFileView*)pView;
	if(pView) {
		CNCSError Error = pView->Open(szUrlPath, pRefreshCallback ? true : false);
		if(Error == NCS_SUCCESS) {
			pView->SetRefreshCallback(pRefreshCallback);
		}
		return(Error.GetErrorNumber());
	}
	return(NCS_INVALID_PARAMETER);
}
/*******************************************************
**	NCScbmCloseFileView() -	Closes a previously opened file view
**
**	Notes:
**	-	File could be local or remote
**	-	Blocks until file can be opened
********************************************************/

extern "C"
NCSError	NCScbmCloseFileViewEx(NCSFileView *pNCSFileView,			/**[07]**/ /**[11]**/
						  BOOLEAN bFreeCachedFile)				/**[07]**/
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		CNCSError Error = pView->Close((bFreeCachedFile == TRUE) ? true : false);
		delete pView;
		return(Error.GetErrorNumber());
	}
	return(NCS_INVALID_PARAMETER);
}

NCSError	NCScbmCloseFileView(NCSFileView *pNCSFileView) /**[11]**/
{
	return(NCScbmCloseFileViewEx(pNCSFileView, FALSE));	/**[07]**/
}

/*******************************************************
**	NCScbmGetViewFileInfo() -	Returns the information about a file opened using OpenView
**
**	Notes:
**	-	Just returns information about the file, not the current SetView in this view into that file
********************************************************/
extern "C" NCSError NCScbmGetViewFileInfo(NCSFileView *pNCSFileView, NCSFileViewFileInfo **ppNCSFileViewFileInfo) /**[11]**/
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		*ppNCSFileViewFileInfo = (NCSFileViewFileInfo *)pView->GetFileInfo();
		return(*ppNCSFileViewFileInfo ? NCS_SUCCESS : NCS_INVALID_PARAMETER);
	}
	return(NCS_INVALID_PARAMETER);
}

extern "C" NCSError NCScbmGetViewFileInfoEx(NCSFileView *pNCSFileView, NCSFileViewFileInfoEx **ppNCSFileViewFileInfo) /**[11]**/
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		*ppNCSFileViewFileInfo = (NCSFileViewFileInfoEx *)pView->GetFileInfo();
		return(*ppNCSFileViewFileInfo ? NCS_SUCCESS : NCS_INVALID_PARAMETER);
	}
	return(NCS_INVALID_PARAMETER);
}

/*******************************************************
**	NCScbmGetViewFileInfo() -	Returns the information about the current SetView
**
**	Notes:
**	-	Only use this inside a callback, to determine which SetView is currently being processed
********************************************************/
extern "C" NCSError NCScbmGetViewInfo(NCSFileView *pNCSFileView, NCSFileViewSetInfo **ppNCSFileViewSetInfo) /**[11]**/
{
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		*ppNCSFileViewSetInfo = (NCSFileViewSetInfo*)pView->GetFileViewSetInfo();
		return(*ppNCSFileViewSetInfo ? NCS_SUCCESS : NCS_INVALID_PARAMETER);
	}
	return(NCS_INVALID_PARAMETER);
}

/*******************************************************
**	NCSecwNetBreakdownUrl()	- Returns pointers into the URL to each
**	part.  Returns TRUE if the file is a valid ECW file (or URL
**	to a ECW file) or FALSE if it is not a valid ECW file or URL.
**
**	Notes:
**
**	If Protocol or Host are NULL, then must be a local file
**	If a pointer is returned as NULL, then LENGTH for that pointer is undefined
**	These are pointers into the original string, so the strings are not null terminated
********************************************************/

BOOLEAN NCSecwNetBreakdownUrl( char *szUrlPath,
						   char **ppProtocol, int *pnProtocolLength,
						   char **ppHost,	  int *pnHostLength,
						   char **ppFilename, int *pnFilenameLength)
{
#ifdef NCSJPC_ECW_SUPPORT
	return(NCSecwNetBreakdownUrl_ECW(szUrlPath, 
									 ppProtocol, pnProtocolLength, 
									 ppHost, pnHostLength, 
									 ppFilename, pnFilenameLength));
#else // NCSJPC_ECW_SUPPORT
	return(NCS_INVALID_PARAMETER);
#endif
}

/*
**	NCSecwSetIOCallbacks()
**
** Allows user to register callback functions to be used for ECW File IO.
** This can be used to read from memory etc., instead of disk.
*/
NCSError NCSecwSetIOCallbacks(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
							  NCSError (NCS_CALL *pCloseCB)(void *pClientData),
							  NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
							  NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
							  NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset))
{
#ifdef NCS_BUILD_UNICODE
	CNCSError Error = CNCSJPCFileIOStream::SetIOCallbacks(pOpenCB, NULL, pCloseCB, pReadCB, pSeekCB, pTellCB);
#else
	CNCSError Error = CNCSJPCFileIOStream::SetIOCallbacks(pOpenCB, pCloseCB, pReadCB, pSeekCB, pTellCB);
#endif
	return(Error.GetErrorNumber());
}

extern "C" NCSFileType NCScbmGetFileType( NCSFileView *pNCSFileView ) {
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return pView->GetFileType();
	} else {
		return NCS_FILE_UNKNOWN;
	}
}

extern "C" char *NCScbmGetFileMimeType( NCSFileView *pNCSFileView ) {
	CNCSJP2FileView *pView = CNCSJP2FileView::FindJP2FileView(pNCSFileView);
	if(pView) {
		return pView->GetFileMimeType();
	} else {
		return NULL;
	}
}
