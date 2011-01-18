/********************************************************** 
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCSECWCompessClient.h
** CREATED:	Thu 19/08/1999
** AUTHOR: 	Simon Cope
** PURPOSE:	Public Interface library to compress ECW v2.0 image files
** EDITS:
** [01] sjc 08Dec1999	Updated comments
**
********************************************************/

/** @file NCSECWCompressClient.h */
#ifndef NCSECWCOMPRESSCLIENT_H
#define NCSECWCOMPRESSCLIENT_H

#ifndef NCSECWCLIENT_H
#include "NCSECWClient.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif
#ifndef NCSERRORS_H
#include "NCSErrors.h"
#endif

#define ERSWAVE_VERSION 2
#define ERSWAVE_VERSION_STRING	"2.0"		/* should be in sync with the above */

/** 
 *  @enum
 *	An enumerated type specifying the format of the compressed data. Currently greyscale, RGB
 *	and luminance-chrominance formats are supported.  This will later need expanding to include 
 *	multiband formats such as CMYK.
 */
typedef enum {
	/** The compressed data is unformatted */
	COMPRESS_NONE		= NCSCS_NONE,
	/** Greyscale format, single band */
	COMPRESS_UINT8		= NCSCS_GREYSCALE,
	/** JPEG standard YUV digital format, three band */
	COMPRESS_YUV		= NCSCS_YUV,
	/** Multiband format */
	COMPRESS_MULTI		= NCSCS_MULTIBAND,
	/** RGB images (converted to COMPRESS_YUV internally) */
	COMPRESS_RGB		= NCSCS_sRGB			
} CompressFormat;

/**	
 *	@enum
 *	An enumerated type allowing the user to hint at an appropriate compression scheme.  The 
 *	value is a guideline for the SDK compression functions.
 */
typedef enum {
	/** No compression hint */
	COMPRESS_HINT_NONE	= 0,
	/** Do the fastest compression possible */
	COMPRESS_HINT_FAST	= 1,
	/** Try to achieve the maximum possible compression ratio */
	COMPRESS_HINT_BEST	= 2,
	/** Optimise compression process for later Internet use of the compressed file */
	COMPRESS_HINT_INTERNET = 3
} CompressHint;

/** @def Maximum length of a datum definition string */
#define ECW_MAX_DATUM_LEN		16
#ifndef ECW_MAX_PROJECTION_LEN
/** @def Maximum length of a projection definition string */
#define ECW_MAX_PROJECTION_LEN	16
#endif

/** @def X dimension of the default (and preferred) block size */
#define X_BLOCK_SIZE	64
/** @def Y dimension of the default (and preferred) block size */
#define	Y_BLOCK_SIZE	64

/** 
 *	@struct
 *	This structure contains information used when compressing raster data 
 *	to an output ECW file.  Most of the values have sensible defaults so users
 *	need only customise those of significance to their application.  Certain 
 *	of these values must always be specified in the client code, namely 
 *	fTargetCompression, nInOutSizeX, nInOutSizeY, nInputBands, and pReadCallback.
 *
 *	Once compression is complete certain statistics are calculated and added 
 *	to this structure.  These include the time taken, the output file size in 
 *	bytes, the actual compression ratio achieved, and the MB/s throughput of the 
 *	process.
 */
typedef struct NCSEcwCompressClient {
	/*
	** These fields are populated by the compression client
	*/
	/** If this is specified but the output file is not, a default output filename will be created.
	 *	Otherwise this field is unused.
	 */
	char	szInputFilename[MAX_PATH];			
	/** An output filename must be specified if no input filename is specified */
	char	szOutputFilename[MAX_PATH];	
	/** The target compression ratio - must be specified */
	IEEE4			fTargetCompression;
	/**	The compression format to use.  See the related enumerated type definition */
	CompressFormat	eCompressFormat;
	/** A guideline for an appropriate compression scheme to use.  This currently has 
	 *	no effect, though the default value is COMPRESS_HINT_INTERNET.  Reserved for
	 *	future use, see the related enumerated type definition 
	 */
	CompressHint	eCompressHint;
	/** X dimension of the block size to use.  Can be 64, 128, 256, 512, 1024, or 2048.
	 *	The default for these is set to 64 which produces preferred performance over the internet.
	 */
	UINT32			nBlockSizeX;
	/** Y dimension of the block size to use.  Can be 64, 128, 256, 512, 1024, or 2048.
	 *	The default for these is set to 64 which produces preferred performance over the internet.
	 */
	UINT32			nBlockSizeY;				/* Y Block size (64, 128, 256, 512)					*/
	/** Number of cells of input data and compressed file in the X direction - must be specified */
	UINT32			nInOutSizeX;
	/** Number of cells of input data and compressed file in the Y direction - must be specified */
	UINT32			nInOutSizeY;
	/** Number of bands in the input data - must be specified */
	UINT32			nInputBands;
	/** Number of bands in the output file - should not generally be specified */
	UINT32			nOutputBands;
	/** Size of the input file in bytes - should not be specified, it will be determined automatically. */
	UINT64			nInputSize;
	/** Optional field specifying the cell size in the X direction in eCellSizeUnits */
	IEEE8			fCellIncrementX;
	/** Optional field specifying the cell size in the Y direction in eCellSizeUnits */
	IEEE8			fCellIncrementY;
	/** Optional field specifying the X world origin of the input data in eCellSizeUnits */
	IEEE8			fOriginX;
	/** Optional field specifying the Y world origin of the input data in eCellSizeUnits */
	IEEE8			fOriginY;
	/** Optional field specifying the units in which world cell sizes are specified, e.g. meters, feet */
	CellSizeUnits	eCellSizeUnits;
	/** ER Mapper GDT style datum string */
	char			szDatum[ECW_MAX_DATUM_LEN];
	/** ER Mapper GDT style projection string */
	char			szProjection[ECW_MAX_PROJECTION_LEN];
	/** Callback function used to obtain lines of band data from the input data - must be specified */
	BOOLEAN			(*pReadCallback) (struct NCSEcwCompressClient *pClient,
									  UINT32 nNextLine,
									  IEEE4 **ppInputArray);
	/** Optional status callback function to track the progress of the compression process */
	void			(*pStatusCallback) (struct NCSEcwCompressClient *pClient,
										UINT32 nCurrentLine);				
	/** Optional cancel callback function which can be used to cancel a compression process */
	BOOLEAN			(*pCancelCallback) (struct NCSEcwCompressClient *pClient);
	/** (void *) Pointer to any private data you need to access in the three callback functions */
	void			*pClientData;
	/** Created by NCSEcwCompressOpen() */
	struct			EcwCompressionTask *pTask;
	/*
	** The remaining fields are populated by NCSEcwCompressClose() 
	*/
	/** Actual compression rate achieved - ratio of input data size to output file size */
	IEEE4			fActualCompression;
	/** Time taken to perform the complete compression, in seconds */
	IEEE8			fCompressionSeconds;
	/** MB/s throughput during the compression process */
	IEEE8			fCompressionMBSec;
	/** Total size of the output file in bytes */
	UINT64			nOutputSize;
} NCSEcwCompressClient;

/** 
 *	Allocate a new CompressionClient structure and fill in defaults 
 *
 *	@return					A pointer to memory allocated to an NCSEcwCompressClient structure
 */
NCSEcwCompressClient * NCS_CALL NCSEcwCompressAllocClient(void);
/**	
 *	Having set compression parameters, initialise the compression process
 *
 *	@param[in,out]	pInfo				The NCSEcwCompressClient structure containing the compression parameters 
 *	@param[in]		bCalculateSizesOnly	Which output statistics to calculate 
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCS_CALL NCSEcwCompressOpen(NCSEcwCompressClient *pInfo, BOOLEAN bCalculateSizesOnly); 
/**	
 *	Start the compression process based on these parameters
 *
 *	@param[in,out]	pInfo				The NCSEcwCompressClient structure for this compression process
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCS_CALL NCSEcwCompress(NCSEcwCompressClient *pInfo);
/** 
 *	Having completed compression, close, calculate output statistics, and clean up
 *
 *	@param[in,out]	pInfo				The NCSEcwCompressClient structure for this compression process
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCS_CALL NCSEcwCompressClose(NCSEcwCompressClient *pInfo);
/**	
 *	Free the memory allocated to the NCSEcwCompressClient structure
 *
 *	@param[in,out]	pInfo				The structure to free
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCS_CALL NCSEcwCompressFreeClient(NCSEcwCompressClient *pInfo);

typedef struct {
	UINT8				nVersion;			// ECW file version == ERSWAVE_VERSION
	CellSizeUnits		eCellSizeUnits;		// Units used for pixel size
	IEEE8				fCellIncrementX;	// Increment in CellSizeUnits in X direction. May be negative. Will never be zero
	IEEE8				fCellIncrementY;	// Increment in CellSizeUnits in Y direction. May be negative. Will never be zero
	IEEE8				fOriginX;			// World X origin for top-left corner of top-left cell, in CellSizeUnits
	IEEE8				fOriginY;			// World Y origin for top-left corner of top-left cell, in CellSizeUnits
	char				*szDatum;			// ER Mapper style Datum name string, e.g. "RAW" or "NAD27". Will never be NULL
	char				*szProjection;		// ER Mapper style Projection name string, e.g. "RAW" or "WGS84". Will never be NULL

	BOOLEAN				bCompressedOffsetTable; // Is the block table compressed
} NCSEcwEditInfo;

NCSError NCS_CALL NCSEcwEditReadInfo(char *pFilename, NCSEcwEditInfo **ppInfo);
NCSError NCS_CALL NCSEcwEditWriteInfo(char *pFilename, NCSEcwEditInfo *pInfo, void (*pProgressFunc)(UINT64 nTotal, UINT64 nWritten, void *pClientData), BOOLEAN (*pCancelFunc)(void *pClientData), void *pClientData);
NCSError NCS_CALL NCSEcwEditFreeInfo(NCSEcwEditInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* NCSECWCOMPRESSCLIENT_H */
