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
** FILE:   	NCSECWClient.wc
** CREATED:	08 May 1999
** AUTHOR: 	TIS
** PURPOSE:	Public Client Interface library to Image Web Server ECW image decompression library
** EDITS:
** [01] tis	01-May-99 Created file
** [02] sjc 09-Jul-99 added SetFileViewEx() call.
** [03] sjc 30-Sep-99 added NCScbmCloseFileViewEx() call.
** [04] sjc 26-Oct-00 Added NCSecwSetIOCallbacks()
** [05] ddi 14-Nov-00 Added NCScbmReadViewLineIEEE4() and NCScbmReadViewLineUINT16()
** [06] sjc 22-Jan-02 New config get/set routines and enum
** [07] sjc 04-Feb-02 Added MAXOPEN
** [08] sjc 20-Feb-02 Added a bunch of tuneable parameters
** [09] sjc 30-Apr-03 Added low-mem compression config option
** [10] rar 02-Sep-03 Added option to try ecwp re-connection if connection lost
** [11] tfl 02-Jul-04 Moved declaration of NCSEcwCellType from here to NCSTypes.h
** [12] tfl 05-Jul-04 Added #define of allowed JPEG2000 file extensions
** [13] tfl 06-Jan-05 Added and fixed C exports of GDT related functions
** [14] tfl 12-Dec-05 lint fixes
** [15] tfl 12-Dec-05 Fixes to confusing parameter names (nTopX --> nTLX etc.)
** [16] tfl 20-Jan-06 Added NCSCFG_MAX_PROGRESSIVE_VIEW_SIZE parameter
** NOTES
**
**	(1)	This file must be kept in synch with NCSECW.H
** 
**
********************************************************/

/** @file NCSECWClient.h */

#ifndef NCSECWCLIENT_H
#define NCSECWCLIENT_H

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif
#ifndef NCSERRORS_H
#include "NCSErrors.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 
 *	@enum
 *  Enumerated type for the return status from read line routines.
 *	The application should treat CANCELLED operations as non-fatal,
 *	in that they most likely mean this view read was cancelled for
 *	performance reasons.
 */
typedef enum {
	/** Successful read */
	NCSECW_READ_OK			= 0,
	/** Read failed due to an error */
	NCSECW_READ_FAILED		= 1,
	/** Read was cancelled, either because a new SetView arrived or a 
	    library shutdown is in progress */
	NCSECW_READ_CANCELLED	= 2	
} NCSEcwReadStatus;

/** 
 *	@enum
 *  Enumerated type for the cell sizes supported by the SDK.
 */
typedef enum {
	/** Invalid cell units */
	ECW_CELL_UNITS_INVALID	=	0,
	/** Cell units are standard meters */
	ECW_CELL_UNITS_METERS	=	1,
	/** Degrees */
	ECW_CELL_UNITS_DEGREES	=	2,
	/** US Survey feet */
	ECW_CELL_UNITS_FEET		=	3,
	/** Unknown cell units */
	ECW_CELL_UNITS_UNKNOWN	=	4
} CellSizeUnits;

/*[11] NCSEcwCellType declaration moved to NCSTypes.h */

/** 
 *	@enum
 *  Enumerated type for file types supported by the SDK.
 */
typedef enum {
	/** Unknown file type or no file open */
	NCS_FILE_UNKNOWN	=	0,
	/** The file is an ECW */
	NCS_FILE_ECW	=	1,
	/** The file is a Jpeg 2000 File */
	NCS_FILE_JP2	=	2
} NCSFileType;


/** 
 *	@enum
 *	Enumerated type for all the possible argument types
 *	for the SDK configuration function NCSEcwSetConfig
 */
typedef enum {
	/** BOOLEAN value, whether to texture image */
	NCSCFG_TEXTURE_DITHER			= 0,
	/** BOOLEAN value, whether to reopen file for each view opened */
	NCSCFG_FORCE_FILE_REOPEN		= 1,
	/** UINT32 value, target maximum memory cache size */
	NCSCFG_CACHE_MAXMEM				= 2,
	/** UINT32 value, target maximum number of files to have open to use for cache */ /*[07]*/
	NCSCFG_CACHE_MAXOPEN			= 3,
	/** NCSTimeStampMs value, time an ecwp:// blocking read will wait before returning - default 10000 ms */ /*[08]*/
	NCSCFG_BLOCKING_TIME_MS			= 4,
	/** NCSTimeStampMs value, time delay between blocks arriving and the next refresh callback - default 500 ms */
	NCSCFG_REFRESH_TIME_MS			= 5,
	/** NCSTimeStampMs value, minimum time delay between last cache purge and the next one - default 1000 ms */ /*[08]*/
	NCSCFG_PURGE_DELAY_MS			= 6,
	/** NCSTimeStampMs value, time delay between last view closing and file being purged from cache - default 1800000ms */ /*[08]*/
	NCSCFG_FILE_PURGE_DELAY_MS		= 7,
	/** NCSTimeStampMs value, minimum time delay between last view closing and file being purged from cache - default 30000 ms */ /*[08]*/
	NCSCFG_MIN_FILE_PURGE_DELAY_MS	= 8,
	/** (char *) value, "server" name of ECWP proxy server */
	NCSCFG_ECWP_PROXY				= 9,
	/** BOOLEAN value, whether to force a low-memory compression */ /*[09]*/
	NCSCFG_FORCE_LOWMEM_COMPRESS	= 10,
	/** BOOLEAN value, whether to try to reconnect if ECWP connection is lost */ /*[10]*/
	NCSCFG_TRY_ECWP_RECONNECT		= 11,
	/** BOOLEAN value, whether to manage ICC profiles on file read */
	NCSCFG_JP2_MANAGE_ICC			= 12,
	/** UINT32 value, number of cahce bytes to use for JP2 file I/O caching - default 2^15 == 32768 */
	NCSCFG_JP2_FILEIO_CACHE_MAXMEM	= 13,
	/** UINT32 value, maximum size for progressive views */
	NCSCFG_MAX_PROGRESSIVE_VIEW_SIZE = 14
} NCSEcwConfigType;					/*[06]*/

/**
 *	@enum
 *	Enumerated type to describe the usage and precedence of geographic metadata in conjunction with JPEG 2000.
 */
typedef enum {
		/** Do not use geodata */
		JP2_GEODATA_USE_NONE				= 0,		//000000000
		/** Use GeoTIFF UUID box metadata only */
		JP2_GEODATA_USE_PCS_ONLY			= 1,		//000000001
		/** Use GML header box metadata only */
		JP2_GEODATA_USE_GML_ONLY			= 2,		//000000010
		/** Use world files only */
		JP2_GEODATA_USE_WLD_ONLY			= 4,		//000000100
		/** Use the GML box then the GeoTIFF box */
		JP2_GEODATA_USE_GML_PCS				= 10,		//000001010
		/** Use the world file then the GeoTIFF box */
		JP2_GEODATA_USE_WLD_PCS				= 12,		//000001100
		/** Use the GeoTIFF box then the GML box */
		JP2_GEODATA_USE_PCS_GML				= 17,		//000010001
		/** Use the world file then the GML box */
		JP2_GEODATA_USE_WLD_GML				= 20,		//000010100
		/** Use the GeoTIFF box then the world file */
		JP2_GEODATA_USE_PCS_WLD				= 33,		//000100001
		/** Use the GML box then the world file */
		JP2_GEODATA_USE_GML_WLD				= 34,		//000100010
		/** Use the world file, then the GML box, then the GeoTIFF box */
		JP2_GEODATA_USE_WLD_GML_PCS			= 84,		//001010100
		/** Use the GML box, then the world file, then the GeoTIFF box */
		JP2_GEODATA_USE_GML_WLD_PCS			= 98,		//001100010
		/** Use the world file, then the GeoTIFF box, then the GML box */
		JP2_GEODATA_USE_WLD_PCS_GML			= 140,		//010001100
		/** Use the GeoTIFF box, then the world file, then the GML box */
		JP2_GEODATA_USE_PCS_WLD_GML			= 161,		//010100001
		/** Use the GML box, then the GeoTIFF box, then the world file */
		JP2_GEODATA_USE_GML_PCS_WLD			= 266,		//100001010
		/** Use the GeoTIFF box, then the GML box, then the world file */
		JP2_GEODATA_USE_PCS_GML_WLD			= 273		//100010001
} GeodataUsage;

/** 
 *	@struct
 *	Structure containing file metadata for the ECW interface.
 *	SDK method NCScbmGetViewFileInfo returns a pointer to this file info structure for a view
 */
typedef struct {
	/** Dataset cells in X direction */
	UINT32	nSizeX;
	/** Dataset cells in Y direction */
	UINT32	nSizeY;			
	/** Number of bands in the file, e.g. 3 for a RGB file */
	UINT16	nBands;
	/** Target compression rate, e,g, 20 == 20:1 compression.  May be zero */
	UINT16	nCompressionRate;
	/** Units used for pixel size */
	CellSizeUnits	eCellSizeUnits;
	/** Increment in eCellSizeUnits in X direction.  May be negative.  Never zero */
	IEEE8	fCellIncrementX;
	/** Increment in eCellSizeUnits in Y direction.  May be negative.  Never zero */
	IEEE8	fCellIncrementY;
	/** World X origin for top left corner of top left cell, in eCellSizeUnits */
	IEEE8	fOriginX;
	/** World Y origin for top left corner of top left cell, in eCellSizeUnits */
	IEEE8	fOriginY;
	/** ER Mapper style Datum name string, e.g. "RAW" or "NAD27".  Never NULL */
	char	*szDatum;
	/** ER Mapper style Projection name string, e.g. "RAW" or "GEODETIC".  Never NULL */
	char	*szProjection;
} NCSFileViewFileInfo;

/** 
 *	@struct
 *	Struct containing metadata for a specific band in the file.
 */
typedef struct {
	/** Bit depth used in band, including sign bit */
	UINT8	nBits;
	/** Whether band data is signed */
	BOOLEAN	bSigned;
	/** ASCII description of band, e.g. "Red" or "Band1" */
	char	*szDesc;
} NCSFileBandInfo;

/** 
 *	@defgroup banddescs
 *	These are the allowable ASCII descriptions in the current implementation.
 *  @{
 */
/** @def */
#define NCS_BANDDESC_Red							"Red"
/** @def */
#define NCS_BANDDESC_Green							"Green"
/** @def */
#define NCS_BANDDESC_Blue							"Blue"
/** @def */
#define NCS_BANDDESC_All							"All"
/** @def */
#define NCS_BANDDESC_RedOpacity						"RedOpacity"
/** @def */
#define NCS_BANDDESC_GreenOpacity					"GreenOpacity"
/** @def */
#define NCS_BANDDESC_BlueOpacity					"BlueOpacity"
/** @def */
#define NCS_BANDDESC_AllOpacity						"AllOpacity"
/** @def */
#define NCS_BANDDESC_RedOpacityPremultiplied		"RedOpacityPremultiplied"
/** @def */
#define NCS_BANDDESC_GreenOpacityPremultiplied		"GreenOpacityPremultiplied"
/** @def */
#define NCS_BANDDESC_BlueOpacityPremultiplied		"BlueOpacityPremultiplied"
/** @def */
#define NCS_BANDDESC_AllOpacityPremultiplied		"AllOpacityPremultiplied"
/** @def */
#define NCS_BANDDESC_Greyscale						"Grayscale"
/** @def */
#define NCS_BANDDESC_GreyscaleOpacity				"GrayscaleOpacity"
/** @def */
#define NCS_BANDDESC_GreyscaleOpacityPremultiplied	"GrayscaleOpacityPremultiplied"
/** @def */
#define NCS_BANDDESC_Band							"Band #%d"
/*@}*/

/** 
 *	@enum
 *	The color space used by a compressed file.
 *	For compatibility with ECW, these values cannot be changed or reordered.
 */
typedef enum {
	/** No color space */
	NCSCS_NONE						= 0,
	/** Greyscale image */
	NCSCS_GREYSCALE					= 1,	// Greyscale
	/** Luminance-chrominance color space */
	NCSCS_YUV						= 2,	// YUV - JPEG Digital, JP2 ICT
	/** Multiband image */
	NCSCS_MULTIBAND					= 3,	// Multi-band imagery
	/** sRGB color space */
	NCSCS_sRGB						= 4,	// sRGB
	/** Modified luminance-chrominance color space */
	NCSCS_YCbCr						= 5		// YCbCr - JP2 ONLY, Auto-converted to sRGB
} NCSFileColorSpace;

/** 
 *	@struct
 *	Extended file metadata structure for the JPEG 2000 interface.
 *	This structure is derived from a compressed JPEG 2000 file.
 *	It is important to note that the information contained within it 
 *	is informative, not normative.  For example, although the file may 
 *	contain metadata that indicates the image it contains is to be rotated,
 *	the SDK will not rotate that image itself.
 *	The SDK function NCScbmGetViewFileInfoEx() returns a pointer to this 
 *	file info structure for a given view.
 */
typedef struct {		
	/** Dataset cells in X direction */	
	UINT32	nSizeX;
	/** Dataset cells in X direction */
	UINT32	nSizeY;
	/** Number of bands in the file, e.g. 3 for a RGB file */
	UINT16	nBands;	
	/** Target compression rate, e,g, 20 == 20:1 compression.  May be zero */
	UINT16	nCompressionRate;
	/** Units used for pixel size */
	CellSizeUnits	eCellSizeUnits;	
	/** Increment in eCellSizeUnits in X direction.  May be negative.  Never zero */
	IEEE8	fCellIncrementX;
	/** Increment in eCellSizeUnits in Y direction.  May be negative.  Never zero */
	IEEE8	fCellIncrementY;
	/** World X origin for top left corner of top left cell, in eCellSizeUnits */
	IEEE8	fOriginX;
	/** World Y origin for top left corner of top left cell, in eCellSizeUnits */
	IEEE8	fOriginY;
	/** ER Mapper style Datum name string, e.g. "RAW" or "NAD27".  Never NULL */
	char	*szDatum;	
	/** ER Mapper style Projection name string, e.g. "RAW" or "GEODETIC".  Never NULL */
	char	*szProjection;
	/** Clockwise rotation of image in degrees */
	IEEE8	fCWRotationDegrees;
	/** Color space of image */
	NCSFileColorSpace eColorSpace;
	/** Cell type of image samples */
	NCSEcwCellType eCellType;
	/** A pointer to an array of band information structures for each band of the image */
	NCSFileBandInfo *pBands;
} NCSFileViewFileInfoEx;

/** 
 *	@struct
 *	Information about an open view into a compressed image file.
 *	This structure contains updated information about the extents and processing status
 *	of an open file view.  NCScbmGetViewSetInfo() will return a pointer to this structure 
 *	for a file view
 */
typedef struct {
	/** Client data */
	void	*pClientData;
	/** Number of bands to read */
	UINT32 nBands;				
	/** Array of band indices being read from the file - the size of this array is nBands */
	UINT32 *pBandList;
	/** Top left of the view in image coordinates */
	UINT32 nTopX, nLeftY;
	/** Bottom right of the view in image coordinates */
	UINT32 nBottomX, nRightY;
	/** Size of the view in pixels */
	UINT32 nSizeX, nSizeY;			
	/** Number of file blocks within the view area */
	UINT32 nBlocksInView;
	/** Number of these file blocks that are currently available */
	UINT32 nBlocksAvailable;
	/** Blocks of the file that were available at the time of the corresponding SetView */
	UINT32 nBlocksAvailableAtSetView;
	/** Number of blocks that were missed during the read of this view */
	UINT32 nMissedBlocksDuringRead;
	/** Top left of the view in world coordinates (if using SetViewEx) */
	IEEE8  fTopX, fLeftY;
	/** Bottom right of the view in world coordinates (if using SetViewEx) */ /*[02]*/
	IEEE8  fBottomX, fRightY;	
} NCSFileViewSetInfo;

typedef struct qmf_level_struct QmfLevelStruct;

/*
** The public library is included into the private library to pick up the public
** structures. If being included, we don't use the public function call definitions
*/

#ifndef ERS_WAVELET_DATASET_EXT
/** 
 *	@def 
 *	Allowed file extensions for ECW files
 */
#define ERS_WAVELET_DATASET_EXT	".ecw"		/* compressed wavelet format file extension */
#endif
#ifndef ERS_JP2_DATASET_EXTS 
/**	
 *	@def
 *	Allowed file extensions for JPEG 2000 files
 */
#define ERS_JP2_DATASET_EXTS	{ ".jp2" , ".j2k" , ".j2c" , ".jpc", ".jpx", ".jpf" } /*[12]*/
#endif

#ifndef ERS_JP2_WORLD_FILE_EXTS
#define ERS_JP2_WORLD_FILE_EXTS	{ ".j2w" , ".jxw" , ".jfw" }  
#endif

#ifndef ERSWAVE_VERSION_STRING
/**	
 *	@def
 *	Current version of the ECW format
 */
#define ERSWAVE_VERSION_STRING	"2.0"		/* should be in sync with the above */
#endif
#ifndef NCS_ECW_PROXY_KEY
#define NCS_ECW_PROXY_KEY		"ECWP Proxy"
#endif // NCS_ECW_PROXY_KEY

/** 
 *	@typedef
 *	This type definition promotes properly transparent usage of the SDK structures.
 */
typedef struct NCSFileStruct NCSFile;
/** 
 *	@typedef
 *	This type definition promotes properly transparent usage of the SDK structures.
 */
typedef struct NCSFileViewStruct NCSFileView;

/*
**	Note:  routines return non-zero if there was an error
*/

/*
**	Do not call these unless directly including the NCSECW code rather than linking against the DLL
*/
/** 
 *	Initialise the SDK libraries.  Should not be called directly unless you are directly including 
 *	the SDK code rather than linking against the DLL. 
 */
extern void NCS_CALL NCSecwInit(void);		// DO NOT call if linking against the DLL
/** 
 *	Initialise the SDK libraries.  Should not be called directly unless you are directly including 
 *	the SDK code rather than linking against the DLL. 
 */
extern void NCS_CALL NCSecwShutdown(void);	// DO NOT call if linking against the DLL

/*[04]*/
/** 
 *	Set custom functions to be used by the SDK library to open, close, read, seek and tell input files.
 *	This can be used to manage "wrapper" files which encapsulate the ordinary compressed files 
 *  handled by the SDK.
 *
 *	@param[in]	pOpenCB		Callback function for opening input files
 *	@param[in]	pCloseCB	Callback function for closing input files
 *	@param[in]	pReadCB		Callback function for reading input files
 *	@param[in]	pSeekCB		Callback function for seeking input files
 *	@param[in]	pTellCB		Callback function for telling input files
 *	@return					NCSError value, NCS_SUCCESS or the code of any applicable error
 */
extern NCSError NCSecwSetIOCallbacks(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
									  NCSError (NCS_CALL *pCloseCB)(void *pClientData),
									  NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
									  NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
									  NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset));

/*
**	This is used for multi-client load testing and texture management. Never call this routine.
*/
extern void NCS_CALL NCSecwConfig(BOOLEAN bNoTextureDither, BOOLEAN bForceFileReopen);

/** 
 *	Reports if this is a local or remote file, and breaks URL down into sections
 *
 *	@param[in]	szUrlPath			The URL to be broken down and analysed
 *	@param[out]	ppProtocol			The protocol of the URL (pointer to char *)
 *	@param[out]	pnProtocolLength	The length of the protocol
 *	@param[out]	ppHost				The hostname specified in the URL
 *	@param[out] pnHostLength		The length of the specified hostname
 *	@param[out]	ppFilename			The filename specified by the URL
 *	@param[out]	pnFilenameLength	The length of the specified filename
 *	@return							BOOLEAN value, if this is a local file
 */
extern BOOLEAN NCS_CALL NCSecwNetBreakdownUrl( char *szUrlPath,
						   char **ppProtocol,	int *pnProtocolLength,
						   char **ppHost,		int *pnHostLength,
						   char **ppFilename,	int *pnFilenameLength);

/** 
 *	Opens a file view.  After calling this function, call GetViewFileInfo to obtain file metadata
 *
 *	@param[in]	szUrlPath			The location of the file on which to open a view
 *	@param[out]	ppNCSFileView		The NCSFileView structure to initialise
 *	@param[in]	pRefreshCallback	The refresh callback with which to handle progressive reads (may be NULL for the blocking interface)
 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmOpenFileView(char *szUrlPath, NCSFileView **ppNCSFileView,
					   NCSEcwReadStatus (*pRefreshCallback)(NCSFileView *pNCSFileView));

/**	
 *	Closes a file view.  This can be called at any time after NCScbmOpenFileView is called to clean up an open file view.
 *
 *	@param[in]	pNCSFileView		The file view to close
 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmCloseFileView(NCSFileView *pNCSFileView);

/**	
 *	Closes a file view.  This can be called at any time after NCScbmOpenFileView is called to clean up an open file view.
 *
 *	This version allows the developer to forcibly close a file and free the resources allocated to it.
 *	@param[in]	pNCSFileView		The file view to close
 *	@param[in]	bFreeCachedFile		Whether to force the freeing of the file's memory cache
 *	@return							NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmCloseFileViewEx(NCSFileView *pNCSFileView, BOOLEAN bFreeCachedFile);	/*[03]*/


/** 
 *	Populates a structure with information about an open image file.  Use this version when dealing with ECW files only.
 *
 *	@param[in]	pNCSFileView			The file view open on the file whose metadata is being obtained
 *	@param[out]	ppNCSFileViewFileInfo	A pointer to a pointer to the NCSFileViewFileInfo struct to populate with the metadata
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmGetViewFileInfo(NCSFileView *pNCSFileView, NCSFileViewFileInfo **ppNCSFileViewFileInfo);

/** 
 *	Populates a structure with information about an open image file.  This is the extended version which will return additional 
 *
 *	metadata for a JPEG 2000 file that is irrelevant in the case of ECW input.  Note the different parameter type.
 *	@param[in]	pNCSFileView			The file view open on the file whose metadata is being obtained
 *	@param[out]	ppNCSFileViewFileInfo	A pointer to a pointer to the NCSFileViewFileInfoEx struct to populate with the metadata
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmGetViewFileInfoEx(NCSFileView *pNCSFileView, NCSFileViewFileInfoEx **ppNCSFileViewFileInfo);

/** 
 *	Obtains information about the current request to set the view, including statistics about the data being processed
 *
 *	@param[in]	pNCSFileView			The open file view the extents of which are currently being set
 *	@param[out]	ppNCSFileViewSetInfo	The information about the view being set
 *	@return								NCSError value, either NCS_SUCCESS or an applicable error code
 */
extern NCSError NCS_CALL NCScbmGetViewInfo(NCSFileView *pNCSFileView, NCSFileViewSetInfo **ppNCSFileViewSetInfo);

/** 
 *	Sets the extents and band content of an open file view, and the output view size.  This function can be called at 
 *	any time after a successful call to NCScbmOpenFileView.  In progressive mode, multiple calls to NCScbmSetFileView 
 *	can be made, even if previous SetViews are still being processed, enhancing client interaction with the view.  After 
 *	the call to NCScbmSetFileView, the band list array pBandList can be freed if so desired.  It is used only during the 
 *  processing of the call, and not afterward.
 *
 *	@param[in]	pNCSFileView			The open file view to set
 *	@param[in]	pBandList				An array of integers specifying which bands of the image to read, and in which order
 *	@param[in]	nTLX					Left edge of the view in dataset cells
 *	@param[in]	nTLY					Top edge of the view in dataset cells
 *	@param[in]	nBRX				Right edge of the view in dataset cells
 *	@param[in]	nBRY					Bottom edge of the view in dataset cells
 *	@param[in]	nSizeX					Width of the view to be constructed from the image subset
 *	@param[in]	nSizeY					Height of the view to be constructed from the image subset
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmSetFileView(NCSFileView *pNCSFileView,
										   UINT32 nBands,					
										   UINT32 *pBandList,				
										   UINT32 nTLX, UINT32 nTLY,	
										   UINT32 nBRX, UINT32 nBRY,
										   UINT32 nSizeX, UINT32 nSizeY);	

/** 
 *	Sets the extents and band content of an open file view, and the output view size.  This function can be called at 
 *	any time after a successful call to NCScbmOpenFileView.  In progressive mode, multiple calls to NCScbmSetFileView 
 *	can be made, even if previous SetViews are still being processed, enhancing client interaction with the view.  After 
 *	the call to NCScbmSetFileView, the band list array pBandList can be freed if so desired.  It is used only during the 
 *  processing of the call, and not afterward.  This version also allows the calling program to specify world coordinates 
 *	for the view.
 *
 *	@param[in]	pNCSFileView			The open file view to set
 *	@param[in]	pBandList				An array of integers specifying which bands of the image to read, and in which order
 *	@param[in]	nTLX					Left edge of the view in dataset cells
 *	@param[in]	nTLY					Top edge of the view in dataset cells
 *	@param[in]	nBRX				Bottom edge of the view in dataset cells
 *	@param[in]	nBRY					Right edge of the view in dataset cells
 *	@param[in]	nSizeX					Width of the view to be constructed from the image subset
 *	@param[in]	nSizeY					Height of the view to be constructed from the image subset
 *	@param[in]	fWorldTLX					Left edge of the view in world coordinates
 *	@param[in]	fWorldTLY					Top edge of the view in world coordinates
 *	@param[in]	fWorldBRX				Right edge of the view in world coordinates
 *	@param[in]	fWorldBRY					Bottom edge of the view in world coordinates
 *	@return								NCSError value, NCS_SUCCESS or any applicable error code
 */
extern NCSError NCS_CALL NCScbmSetFileViewEx(NCSFileView *pNCSFileView,		/*[02]*/
											 UINT32 nBands,					
											 UINT32 *pBandList,				
											 UINT32 nTLX, UINT32 nTLY,	
											 UINT32 nBRX, UINT32 nBRY,
											 UINT32 nSizeX, UINT32 nSizeY,	
											 IEEE8 fWorldTLX, IEEE8 fWorldTLY,		
											 IEEE8 fWorldBRX, IEEE8 fWorldBRY);	

/** 
 *	Read line by line in BIL format.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[out]	ppOutputLine			The buffer into which to read the interleaved band information
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBIL( NCSFileView *pNCSFileView, UINT8 **ppOutputLine);

/** 
 *	Read line by line in BIL format to different data types.  This extended version allows the client 
 *	program to read in view lines made up of cells with sample bitdepth other than 8-bit.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[in]	eType					The cell type of the view lines being read
 *	@param[out]	ppOutputLine			The buffer into which to read the interleaved band information
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBILEx( NCSFileView *pNCSFileView, NCSEcwCellType eType, void **ppOutputLine);	/**[05]**/

/** 
 *	Read line by line in RGB format.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[out]	pRGBTriplets			The buffer into which to read the red-green-blue sample triplets.
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineRGB( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets);

/** 
 *	Read line by line in BGR format.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[out]	pBGRTriplets			The buffer into which to read the blue-red-green sample triplets.
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBGR( NCSFileView *pNCSFileView, UINT8 *pBGRTriplets);

/**
 *	Read line by line in RGBA format.  Samples are read into a buffer of UINT32 values, each value comprising
 *	the four bytes of a red-green-blue-alpha sample.  Alpha values will be zero if the input file is in ECW
 *	format as this format does not 'understand' alpha channels.  SDK programmers wanting to compress and 
 *	decompress data in four bands are advised to use multiband compression and NCScbmReadViewLineBil(Ex) to 
 *	handle their data.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[out]	pRGBA					The buffer of packed UINT32 values.
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineRGBA( NCSFileView *pNCSFileView, UINT32 *pRGBA);

/**	
 *	Read line by line in BGRA format.  Samples are read into a buffer of UINT32 values, each value comprising
 *	the four bytes of a red-green-blue-alpha sample.  Alpha values will be zero if the input file is in ECW
 *	format as this format does not 'understand' alpha channels.  SDK programmers wanting to compress and 
 *	decompress data in four bands are advised to use multiband compression and NCScbmReadViewLineBil(Ex) to 
 *	handle their data.
 *
 *	@param[in]	pNCSFileView			The open file view from which to read view lines
 *	@param[out]	pBGRA					The buffer of packed UINT32 values.
 *	@return								NCSEcwReadStatus value, NCSECW_READ_OK or any applicable error code
 */
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBGRA( NCSFileView *pNCSFileView, UINT32 *pBGRA);

/*
** Used for testing purposes, to simulate the Inverse DWT process
*/
NCSEcwReadStatus NCS_CALL NCScbmReadViewFake( NCSFileView *pNCSFileView);	// a fake reader, for performance testing

/** 
 *	Return the major and minor versions of this SDK.
 *
 *	@param[out]	nMajor		The major version
 *	@param[out] nMinor		The minor version
 */
void NCSGetLibVersion( INT32 *nMajor, INT32 *nMinor );

/**	
 *	Set configuration parameters for the SDK functions
 *
 *	@param[in]	eType		The configuration parameter to set, to be followed in the variable argument list by its desired value
 *	@return					NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCSecwSetConfig(NCSEcwConfigType eType, ...);

/**	
 *	Get configuration parameters for the SDK functions
 *
 *	@param[in]	eType		The configuration parameter to obtain, to be followed in the variable argument list by a value buffer
 *	@return					NCSError value, NCS_SUCCESS or any applicable error code
 */
NCSError NCSecwGetConfig(NCSEcwConfigType eType, ...);

/**	
 *	Purge the memory cache being used by the SDK for the current file view.
 *
 *	@param[in]	pView		The file view which should have its cache purged
 */

extern void NCScbmPurgeCache(NCSFileView *pView);

// JPC wrapper entry points
extern void NCS_CALL NCSecwInit_ECW(void);
extern void NCS_CALL NCSecwShutdown_ECW(void);
extern NCSError NCSecwSetIOCallbacks_ECW(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
									  NCSError (NCS_CALL *pCloseCB)(void *pClientData),
									  NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
									  NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
									  NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset));
extern void NCS_CALL NCSecwConfig_ECW(BOOLEAN bNoTextureDither, BOOLEAN bForceFileReopen);
extern BOOLEAN NCS_CALL NCSecwNetBreakdownUrl_ECW( char *szUrlPath,
						   char **ppProtocol,	int *pnProtocolLength,
						   char **ppHost,		int *pnHostLength,
						   char **ppFilename,	int *pnFilenameLength);
extern NCSError NCS_CALL NCScbmOpenFileView_ECW(char *szUrlPath, NCSFileView **ppNCSFileView,
					   NCSEcwReadStatus (*pRefreshCallback)(NCSFileView *pNCSFileView));
extern NCSError NCS_CALL NCScbmCloseFileView_ECW(NCSFileView *pNCSFileView);
extern NCSError NCS_CALL NCScbmCloseFileViewEx_ECW(NCSFileView *pNCSFileView, BOOLEAN bFreeCachedFile);	/**[03]**/
extern NCSError NCS_CALL NCScbmGetViewFileInfo_ECW(NCSFileView *pNCSFileView, NCSFileViewFileInfo **ppNCSFileViewFileInfo);
extern NCSError NCS_CALL NCScbmGetViewFileInfoEx_ECW(NCSFileView *pNCSFileView, NCSFileViewFileInfoEx **ppNCSFileViewFileInfo);
extern NCSError NCS_CALL NCScbmGetViewInfo_ECW(NCSFileView *pNCSFileView, NCSFileViewSetInfo **ppNCSFileViewSetInfo);
extern NCSError NCS_CALL NCScbmSetFileView_ECW(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
				UINT32 nTLX, UINT32 nTLY,	// Top-Left in image coordinates
				UINT32 nBRX, UINT32 nBRY,// Bottom-Left in image coordinates
				UINT32 nSizeX, UINT32 nSizeY);	// Output view size in window pixels

extern NCSError NCS_CALL NCScbmSetFileViewEx_ECW(NCSFileView *pNCSFileView,		/*[02]*/
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
			    UINT32 nTLX, UINT32 nTLY,	// Top-Left in image coordinates
				UINT32 nBRX, UINT32 nBRY,// Bottom-Right in image coordinates
				UINT32 nSizeX, UINT32 nSizeY,	// Output view size in window pixels
				IEEE8 fWorldTLX, IEEE8 fWorldTLY,		// Top-Left in world coordinates
				IEEE8 fWorldBRX, IEEE8 fWorldBRY);	// Bottom-Right in world coordinates
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBIL_ECW( NCSFileView *pNCSFileView, UINT8 **p_p_output_line);
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBILEx_ECW( NCSFileView *pNCSFileView, NCSEcwCellType eType, void **p_p_output_line);	/**[05]**/
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineRGB_ECW( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets);
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBGR_ECW( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets);
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineRGBA_ECW( NCSFileView *pNCSFileView, UINT32 *pRGBA);
NCSEcwReadStatus NCS_CALL NCScbmReadViewLineBGRA_ECW( NCSFileView *pNCSFileView, UINT32 *pBGRA);
NCSEcwReadStatus NCS_CALL NCScbmReadViewFake_ECW( NCSFileView *pNCSFileView);	// a fake reader, for performance testing
NCSError NCSecwSetConfig_ECW(NCSEcwConfigType eType, ...);
NCSError NCSecwGetConfig_ECW(NCSEcwConfigType eType, ...);
extern NCSError NCSecwOpenFile_ECW(NCSFile **ppNCSFile,
									char *szInputFilename,		// input file name or network path
									BOOLEAN bReadOffsets,		// TRUE if the client wants the block Offset Table
									BOOLEAN bReadMemImage);		// TRUE if the client wants a Memory Image of the Header
extern int	NCSecwCloseFile_ECW( NCSFile *pNCSFile);
extern int	NCSecwReadLocalBlock_ECW( NCSFile *pNCSFile, UINT64 nBlockNumber, UINT8 **ppBlock, UINT32 *pBlockLength);
#ifdef MACINTOSH
Handle NCScbmReadFileBlockLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength );
#else
UINT8	*NCScbmReadFileBlockLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength );
#endif
extern BOOLEAN NCScbmGetFileBlockSizeLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength, UINT64 *pBlockOffset );
extern struct tagNCSEcwStatistics *NCSecwGetStatistics_ECW( void );
UINT32 get_qmf_tree_nr_blocks_ECW( QmfLevelStruct *p_top_qmf );

extern void NCSecwGlobalLock(void);
extern BOOLEAN NCSecwGlobalTryLock(void);
extern void NCSecwGlobalUnLock(void);

void NCSInitFileInfoEx(NCSFileViewFileInfoEx *pDst);
void NCSFreeFileInfoEx(NCSFileViewFileInfoEx *pDst);
void NCSCopyFileInfoEx(NCSFileViewFileInfoEx *pDst, NCSFileViewFileInfoEx *pSrc);
BOOLEAN NCSIsFileInfoExGeoreferenced(NCSFileViewFileInfoEx *pInfo);
void NCSStandardizeFileInfoEx(NCSFileViewFileInfoEx *pDst);
/**start [13]*/
void NCSSetGDTPath(char *szPath);
char *NCSGetGDTPath(void);
void NCSDetectGDTPath(void);
void NCSSetJP2GeodataUsage(GeodataUsage nGeodataUsage);
NCSError NCSGetEPSGCode(char *szDatum, char *szProjection, INT32 *pnEPSG);
NCSError NCSGetProjectionAndDatum(INT32 nEPSG, char **pszProjection, char **pszDatum);
/**end [13]*/

NCSFileType NCScbmGetFileType( NCSFileView *pNCSFileView );
char *NCScbmGetFileMimeType( NCSFileView *pNCSFileView );

/** Internal initialisation functions.  These should not be called by user applications.*/
void NCSecwInitInternal();
void NCSecwShutdownInternal();

#ifdef __cplusplus
}
#endif

#endif	// NCSECWCLIENT_H
