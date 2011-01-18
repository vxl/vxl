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
** FILE:   	NCSFile.c
** CREATED:	12 Jan 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	C++ class wrappers for the ECW library
** EDITS:
** [01] 08Sep00 sjc Fixed potential error problem - FormatErrorText() takes varargs, we don't have them...
** [02] 23Sep00 sjc Changed to NCSGetLastErrorText() to get extended error text
** [03] rar	17-Jan-01 Mac port changes
** [04] sjc 10Dec02 Temp fix for bug #1219
*******************************************************/

/*! \class CNCSFile  "include/NCSFile.h"
 *  \file NCSFile.h
 *  \brief A C++ wrapper class around the NCSECW library.
 *
 *  This class provides a file oriented object to access ecw images.
 *  It provides simple open/setview/read/close operations, which are 
 *  simple wrappers around the NCSEcwlibrary (which is view oriented).
 *  \author Mark Sheridan
 *  \date    12 Jan 2000
 *  \par Edits:
 *       07Mar00 mjs Started the NCS documentation project.
 */

#include <string.h>
#ifdef WIN32
	#define _WIN32_WINNT 0x0500	// For MB_SERVICE_NOTIFICATION
#endif
#include "NCSFile.h"
#include "NCSUtil.h"
#include "ECW.h"
#include "NCSECWCompress.h"
#include "NCSJP2File.h"
#include "NCSJP2FileView.h"

//extern "C" NCSecwSetConfig(NCSEcwConfigType eType, ...);

/**
 * Constructor.
 * Creates a NCS file object ready to read from a local or remote (URL) file source.
 * @see ~CNCSFile()
 */
CNCSFile::CNCSFile()
{
	m_pClientData = NULL;
}

/**
 * Destructor.
 * Closes any open ecw file, frees memory, deletes the instance.
 * @see CNCSFile()
 */
CNCSFile::~CNCSFile()
{
}

/**
 * Open an ECW file.
 * Opens an ecw file, local or remote (eg "ecwp://www.earthetc.com/images/usa/1metercalif.ecw").
 * @param pURLPath Pointer to a c string desribing the path of the file
 * @param bProgressiveDisplay Open the file in progressive or non-progressive mode
 * @see Close()
 * @see FormatErrorText()
 * @see BreakdownURL()
 * @return ERS_SUCCESS if successfull, or some NCSError if failed. Use FormatErrorText() to determine the reason for failure.
 */
NCSError CNCSFile::Open(char * pURLPath, 
						BOOLEAN bProgressiveDisplay,
						BOOLEAN bWrite)
{
	return(CNCSJP2FileView::Open(pURLPath, bProgressiveDisplay ? true : false, bWrite ? true : false).GetErrorNumber());
}

/**
 * Close an ECW File.
 * Closes an ecw image and optionally free the cache. If passed false, the cache is free'd 
 * when the ecw library performs garbage collection at a later time and deems the image to
 * be no longer in use. This is usefull if the user is continually opening and closing a view
 * on the same image file.
 * @param bFreeCache TRUE frees the cache and closes the connection, FALSE cancels all pending blocks but leaves the cache intact.
 * @see Open()
 * @see FormatErrorText()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSFile::Close (BOOLEAN bFreeCache)
{
	return(CNCSJP2FileView::Close(bFreeCache ? true : false).GetErrorNumber());
}

/**
 * Set a file view.
 * Set a view of the ecw file in world coordinates, specifying the top left and bottom right extents.
 * @param nBands Number of bands
 * @param pBandList Pointer to a list of INT bands to view
 * @param nWidth The output view sampled width
 * @param nHeight The output view sampled height
 * @param dWorldTLX Top left X value in world coordinates 
 * @param dWorldTLY Top left Y value in world coordinates
 * @param dWorldBRX Bottom right X value in world coordinates
 * @param dWorldBRY Bottom right Y value in world coordinates
 * @see SetView()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSFile::SetView ( INT32 nBands, INT32 *pBandList,
					   INT32 nWidth, INT32 nHeight,
					   IEEE8 dWorldTLX, IEEE8 dWorldTLY,
					   IEEE8 dWorldBRX, IEEE8 dWorldBRY )
{
	INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;
	NCSError nError;

	m_dSetViewWorldTLX = dWorldTLX;
	m_dSetViewWorldTLY = dWorldTLY;
	m_dSetViewWorldBRX = dWorldBRX;
	m_dSetViewWorldBRY = dWorldBRY;

	// We still need to correct dataset coords, so convert the world to datasets.
	ConvertWorldToDataset(m_dSetViewWorldTLX, m_dSetViewWorldTLY, &nDatasetTLX, &nDatasetTLY);
	ConvertWorldToDataset(m_dSetViewWorldBRX, m_dSetViewWorldBRY, &nDatasetBRX, &nDatasetBRY);

		// Do the set view with the adjusted extents
	nError = CNCSJP2FileView::SetView(nBands, 
									(UINT32*)pBandList, 
									nDatasetTLX, nDatasetTLY,
									nDatasetBRX, nDatasetBRY,
									nWidth, nHeight,
									m_dSetViewWorldTLX, m_dSetViewWorldTLY,
									m_dSetViewWorldBRX, m_dSetViewWorldBRY).GetErrorNumber();

	if (nError == NCS_SUCCESS) {
		m_bHaveValidSetView = TRUE;
	} else if( nError == NCS_FILEIO_ERROR ) {
		m_bHaveValidSetView = FALSE;
	} else if( nError == NCS_CONNECTION_LOST ) {
		char buf[1024];														//[01]
		sprintf(buf, "SetView() Error - %s", NCSGetErrorText(nError));		//[01]
		NCSFormatErrorText(nError, buf);									//[01]
		m_bHaveValidSetView = FALSE;
	} else {
		char buf[1024];														//[01]
		sprintf(buf, "SetView() Error - %s", NCSGetErrorText(nError));		//[01]
		nError = NCS_ECW_ERROR;												//[01]
		NCSFormatErrorText(nError, buf);									//[01]

		m_bHaveValidSetView = FALSE;
	}
	m_bSetViewModeIsWorld = TRUE;

	return (NCSError)nError;
}

/**
 * Set a file view.
 * Set a view of the ecw file in dataset units, specifying the top left and bottom right extents.
 * @param nBands Number of bands
 * @param pBandList Pointer to a list of INT bands to view
 * @param nWidth The output view sampled width
 * @param nHeight The output view sampled height
 * @param dDatasetTLX Top left X value in dataset units 
 * @param dDatasetTLY Top left Y value in dataset units
 * @param dDatasetBRX Bottom right X value in dataset units
 * @param dDatasetBRY Bottom right Y value in dataset units
 * @see SetView()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSFile::SetView ( INT32 nBands, INT32 *pBandList, 
				   INT32 nWidth, INT32 nHeight,
				   INT32 dDatasetTLX, INT32 dDatasetTLY,
				   INT32 dDatasetBRX, INT32 dDatasetBRY )
{
	NCSError	nError;
	
	m_nSetViewDatasetTLX = dDatasetTLX;
	m_nSetViewDatasetTLY = dDatasetTLY;
	m_nSetViewDatasetBRX = dDatasetBRX;
	m_nSetViewDatasetBRY = dDatasetBRY;

	if ((m_nSetViewDatasetTLX < 0) || (m_nSetViewDatasetTLY < 0 ) ||
		(m_nSetViewDatasetBRX > m_nWidth-1) || (m_nSetViewDatasetBRY > m_nHeight-1)) {
		// OK some or all of the view is outside the dataset, adjust accordingly
/*#ifdef _DEBUG
		::MessageBox(NULL, NCS_T("Problem with view"), NCS_T("DEBUG"), MB_OK);
#endif*/
		
		// clamp
		if (m_nSetViewDatasetTLX < 0)
			m_nSetViewDatasetTLX = 0;
		if (m_nSetViewDatasetTLY < 0 )
			m_nSetViewDatasetTLY = 0;
		if (m_nSetViewDatasetBRX > m_nWidth-1)
			m_nSetViewDatasetBRX = m_nWidth-1;
		if (m_nSetViewDatasetBRY > m_nHeight-1)
			m_nSetViewDatasetBRY = m_nHeight-1;
	}

	// Do the set view with the adjusted extents
	nError = CNCSJP2FileView::SetView(nBands, 
									  (UINT32*)pBandList, 
									  m_nSetViewDatasetTLX, m_nSetViewDatasetTLY,
									  m_nSetViewDatasetBRX, m_nSetViewDatasetBRY,
									  nWidth, nHeight).GetErrorNumber();

	if (nError == NCS_SUCCESS) {
		m_bHaveValidSetView = TRUE;
	} else if( nError == NCS_FILEIO_ERROR ) {
		m_bHaveValidSetView = FALSE;
	} else {
		char buf[1024];														//[01]
		sprintf(buf, "SetView() Error - %s", NCSGetErrorText(nError));		//[01]
		nError = NCS_ECW_ERROR;												//[01]
		NCSFormatErrorText(nError, buf);									//[01]
		
		m_bHaveValidSetView = FALSE;
	}

	m_bSetViewModeIsWorld = FALSE;

	return (NCSError)nError;
}

/**
 * Get URL component paths.
 * Decompose a URL into its constituent components, returned strings should be freed with NCSFree().
 * @param pURLPath A pointer to a C string. eg ecwp://www.earthetc.com/images/usa/1metercalif.ecw
 * @param ppProtocol The address of a pointer to the C string returned. eg ecwp://
 * @param ppHost The address of a pointer to the C string returned. eg www.earthetc.com
 * @param ppFilename The address of a pointer to the C string returned. eg images/usa/1metercalif.ecw
 * @see NCSFree()
 * @return TRUE or FALSE if the path represented a valid URL
 */
BOOLEAN CNCSFile::BreakdownURL(  char *pURLPath, char **ppProtocol, char **ppHost, char **ppFilename)
{
	int nProtocolLength;
	int nHostLength;
	int nFilenameLength;
	char *pHost, *pProtocol, *pFilename;

	BOOLEAN bError = NCSecwNetBreakdownUrl(pURLPath,
										   &pProtocol, &nProtocolLength,
										   &pHost, &nHostLength,
										   &pFilename, &nFilenameLength);
	if (bError == TRUE) {
		*ppProtocol = (char *)NCSMalloc(nProtocolLength + 1, TRUE);
		strncpy(*ppProtocol, pProtocol, nProtocolLength);

		pHost+=2;
		nHostLength -= 3;
		*ppHost = (char *)NCSMalloc(nHostLength + 1, TRUE); 
		strncpy(*ppHost, pHost, nHostLength);
		
		*ppFilename = (char *)NCSMalloc(nFilenameLength + 1, TRUE);
		strncpy(*ppFilename, pFilename, nFilenameLength);
	}

	return bError;
}

/**
 * Return a pointer to a C string describing the error given by the error number.
 * @param nErrorNum An error number, of enumerated type NCSError.
 * @return A constant pointer to a C string containing the error text
 */
const char *CNCSFile::FormatErrorText ( NCSError nErrorNum )
{
	return NCSGetLastErrorText(nErrorNum);	//[02]
}
																								
/**
 * Set the custome client data.
 * @param pClientData A pointer to a custom client data structure
 * @see GetClientData()
 * @return void
 */
void CNCSFile::SetClientData(void *pClientData)
{
	m_pClientData = pClientData;
}

/**
 * Get the custom client data
 * @see SetClientData()
 * @return A pointer to the custom client data
 */
void *CNCSFile::GetClientData ()
{
	return m_pClientData;
}


/**
 * Convert a coordinate in world space to dataset units
 * Simple rectilinear conversions from world coordinates to dataset units
 * @param dWorldX Input X world coordinate
 * @param dWorldY Input Y world coordinate
 * @param pnDatasetX Pointer to returned X dataset coordinate
 * @param pnDatasetX Pointer to returned Y dataset coordinate
 * @see ConvertDatasetToWorld()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSFile::ConvertWorldToDataset(IEEE8 dWorldX, IEEE8 dWorldY, INT32 *pnDatasetX, INT32 *pnDatasetY)
{
	NCSError nError = NCS_SUCCESS;
	if (m_bIsOpen) {
		_ASSERT(m_dCellIncrementX!=0.0);
		_ASSERT(m_dCellIncrementY!=0.0);
		*pnDatasetX = (INT32)(((dWorldX - m_dOriginX)/m_dCellIncrementX) - 0.5);
		*pnDatasetY = (INT32)(((dWorldY - m_dOriginY)/m_dCellIncrementY) - 0.5);
	}
	else {
		*pnDatasetX = 0;
		*pnDatasetY = 0;
		nError = NCS_ECW_ERROR;
	}
	return nError;
}

/**
 * Convert a coordinate in dataset units to world space.
 * Simple rectilinear conversions from dataset units to world coordinates.
 * @param nDatasetX Input X world coordinate
 * @param nDatasetY Input Y world coordinate
 * @param pdWorldX Pointer to returned X dataset coordinate
 * @param pdWorldY Pointer to returned Y dataset coordinate
 * @see ConvertWorldToDataset()
 * @return NCS_SUCCESS if successfull, or an NCSError value.
 */
NCSError CNCSFile::ConvertDatasetToWorld(INT32 nDatasetX, INT32 nDatasetY, IEEE8 *pdWorldX, IEEE8 *pdWorldY)
{
	NCSError nError = NCS_SUCCESS;
	if (m_bIsOpen) {
		*pdWorldX = m_dOriginX + ((IEEE8)nDatasetX * m_dCellIncrementX);
		*pdWorldY = m_dOriginY + ((IEEE8)nDatasetY * m_dCellIncrementY);
	}
	else {
		*pdWorldX = 0.0;
		*pdWorldY = 0.0;
		nError = NCS_ECW_ERROR;
	}
	return nError;
}

void CNCSFile::RefreshUpdate(NCSFileViewSetInfo *pViewSetInfo)
{
}

NCSEcwReadStatus CNCSFile::RefreshUpdateEx(NCSFileViewSetInfo *pViewSetInfo)
{
	RefreshUpdate(pViewSetInfo);
	return(NCSECW_READ_OK);
}


//Implementations of utility GDT functions
extern "C" void NCSSetGDTPath(char *szPath)
{
	CNCSJP2FileView::SetGDTPath(szPath);
}

extern "C" char *NCSGetGDTPath()
{
	return CNCSJP2FileView::GetGDTPath();
}

extern "C" NCSError NCSGetEPSGCode(char *szProjection, char *szDatum, INT32 *pnEPSG)
{
	return (CNCSJP2FileView::GetEPSGCode(szProjection,szDatum,pnEPSG)).GetErrorNumber();
}

extern "C" NCSError NCSGetProjectionAndDatum(INT32 nEPSG, char **pszProjection, char **pszDatum)
{
	return (CNCSJP2FileView::GetProjectionAndDatum(nEPSG, pszProjection, pszDatum)).GetErrorNumber();
}

extern "C" void NCSDetectGDTPath()
{
	CNCSJP2FileView::DetectGDTPath();
}

extern "C" void NCSSetJP2GeodataUsage(GeodataUsage nGeodataUsage)
{
	CNCSJP2FileView::SetGeodataUsage(nGeodataUsage);
}


