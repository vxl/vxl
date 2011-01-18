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
** [01] sjc 10Dec02 Temporary fix for bug #1219
** [02] tfl 08Dec05 lint fixes
*******************************************************/
/** @file NCSFile.h */

#ifndef NCSFILE_H
#define NCSFILE_H

#include "NCSJP2FileView.h"
#include "NCSECWClient.h"
#include "NCSErrors.h"
#include "NCSDefs.h"

typedef NCSEcwReadStatus (*NCSRefreshCallback)( class CNCSFile *pFile);

/** @class CNCSFile
 *	@brief This class is the main access point for SDK functionality using the C++ API.
 *
 *	CNCSFile inherits from CNCSJP2FileView and is the parent class of CNCSRenderer.
 *	Client applications will generally include classes that inherit from CNCSFile 
 *	and override its RefreshUpdateEx(NCSFileViewSetInfo *pViewSetInfo) method.
 */
class NCS_EXPORT CNCSFile: public CNCSJP2FileView 
{
public:
	/**
	 *	Constructor.
	 */
	CNCSFile();
	/**
	 *	Destructor.
	 */
	virtual ~CNCSFile();
	/**
	 *	Open a file for input or output.
	 *
	 *	@param[in]	pURLPath			The location of the file - if for input, can be a remote file.  Can be a UNC location.
	 *	@param[in]	bProgressiveDisplay	Whether the file will be opened in progressive mode if for input.
	 *	@param[in]	bWrite				Whether the file is being opened for output.
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError Open ( char * pURLPath, 
					BOOLEAN bProgressiveDisplay, 
					BOOLEAN bWrite = FALSE);
	/**
	 *	Close the file.
	 *
	 *	@param[in]	bFreeCache			Whether to free the memory cache that is associated with the file after closing it.
	 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
	 */
	NCSError Close ( BOOLEAN bFreeCache = TRUE );
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
	NCSError SetView ( INT32 nBands, INT32 *pBandList, 
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
	NCSError SetView ( INT32 nBands, INT32 *pBandList, 
					   INT32 nWidth, INT32 nHeight,
					   INT32 dDatasetTLX, INT32 dDatasetTLY,
					   INT32 dDatasetBRX, INT32 dDatasetBRY );
	/**
	 *	Rectilinear conversion from world coordinates to dataset coordinates.
	 *
	 *	@param[in]	dWorldX				The world X coordinate.
	 *	@param[in]	dWorldY				The world Y coordinate.
	 *	@param[out]	pnDatasetX			A buffer for the output dataset X coordinate.
	 *	@param[out]	pnDatasetY			A buffer for the output dataset Y coordinate.
	 */
	NCSError ConvertWorldToDataset(IEEE8 dWorldX, IEEE8 dWorldY, INT32 *pnDatasetX, INT32 *pnDatasetY);
	/**
	 *	Rectilinear conversion from dataset coordinates to world coordinates.
	 *
	 *	@param[in]	nDatasetX			The dataset X coordinate.
	 *	@param[in]	nDatasetY			The dataset Y coordinate.
	 *	@param[out]	pdWorldX			A buffer for the output world X coordinate.
	 *	@param[out]	pdWorldY			A buffer for the output world Y coordinate.
	 */
	NCSError ConvertDatasetToWorld(INT32 nDatasetX, INT32 nDatasetY, IEEE8 *pdWorldX, IEEE8 *pdWorldY);
	/**
	 *	Set a (void *) to a data structure containing any client data that must be accessed in the read callback.
	 *
	 *	@param[in]	pClientData			(void *) pointer to client data.
	 */
	void SetClientData(void *pClientData);
	/**	
	 *	Obtain any client data that has been established.
	 *
	 *	@return							(void *) pointer to client data.
	 */
	void *GetClientData();

	/** 
	 *	Utility function.  Breaks down a URL string into protocol, hostname, and filename components.
	 *
	 *	@param[in]	pURLPath		The URL to be broken down and analyzed.
	 *	@param[out]	ppProtocol		A pointer to the protocol string resulting from the URL decomposition.
	 *	@param[out]	ppHost			A pointer to the hostname resulting from the URL decomposition.
	 *	@param[out]	ppFilename		A pointer to the filename resulting from the URL decomposition.
	 *	@return						BOOLEAN value, whether the input URL is a remote file.
	 */
	static BOOLEAN BreakdownURL(  char *pURLPath, 
							char **ppProtocol,
							char **ppHost, 
							char **ppFilename);
	/**	
	 *	Obtain meaningful error text from a returned error code.
	 *
	 *	@param[in]	nErrorNum		Error code
	 *	@return						(char *) value, an explanatory ASCII string for the error code
	 */
	static const char *FormatErrorText ( NCSError nErrorNum );
	/**
	 *	More data has become available and a refresh update should be done.  Deprecated.
	 *
	 *	@param[in]	pViewSetInfo	Pointer to a SetViewInfo containing details on view the update is from.
	 */
	virtual void RefreshUpdate(NCSFileViewSetInfo *pViewSetInfo);
	/** 
	 * More data is available and a refresh update should be done.
	 *
	 * @param[in]	pSetViewInfo		Pointer to SetViewInfo containing details on view the update is for
	 * @return      NCSEcwReadStatus	Return the Read status code from the ReadLine*() call.
	 */
	virtual NCSEcwReadStatus RefreshUpdateEx(NCSFileViewSetInfo *pViewSetInfo);

private:
	void *m_pClientData;
};
	/**
	 * Set the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
	 * @param szPath the new GDT data path
	 */
extern "C" void	NCSSetGDTPath(char *szPath);
	/**
	 * Get the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
	 * @return the custom path being used, or NULL
	 */
extern "C" char *NCSGetGDTPath(void);
	/** 
	 * Get an EPSG Code for a specified GDT Datum/Projection pairing
	 * @param		szDatum			GDT Datum name
	 * @param		szProjection	GDT Projection name
	 * @param		pnEPSG			Returned EPSG code
	 * @return		NCSError		NCS_SUCCESS, or error on failure
	 */
extern "C" NCSError NCSGetEPSGCode(char *szDatum, char *szProjection, INT32 *pnEPSG);
	/**
	 * Get an ER Mapper projection/datum pair from an EPSG code, if any are applicable
	 * @param		nEPSGCode		EPSG code
	 * @param		pszProjection	Returned ER Mapper projection string
	 * @param		pszDatum		Returned ER Mapper datum string
	 * @return		NCSError		NCS_SUCCESS, or error on failure
	 */
extern "C" NCSError NCSGetProjectionAndDatum(INT32 nEPSGCode, char **pszProjection, char **pszDatum);
	/**
	 * Refresh the custom GDT data path, doing a search and validity check on 
	 * the current path value and several common GDT data locations
	 */
extern "C" void NCSDetectGDTPath(void);
	/**
	 * Set the usage of geographical metadata when reading and writing JPEG 2000 files.
	 * @param		nGeodataUsage	GeodataUsage enum value specifying which metadata to use
	 */
extern "C" void NCSSetJP2GeodataUsage(GeodataUsage nGeodataUsage);
#endif /* NCSFILE_H */
