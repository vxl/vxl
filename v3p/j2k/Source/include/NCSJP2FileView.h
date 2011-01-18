/********************************************************
** Copyright 2003 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/include/NCSJP2FileView.h $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2FileView class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 07Sep04 tfl  Added support for configuring georeferencing use
**			 [02] 11Nov04 tfl  Added rotation member to ensure rotation data persists
**			 [03] 19Jan06 tfl  Changed tiled views vector to a single view-strip member
**			 [04] 31Jan06 tfl  Added forward declaration of CNCSJPCResample
 *******************************************************/

#ifndef NCSJP2FILEVIEW_H
#define NCSJP2FILEVIEW_H

#ifndef NCSJPCDEFS_H
#include "NCSJPCDefs.h"
#endif // NCSJPCDEFS_H

#ifdef NCSJPC_ECW_SUPPORT
#ifndef NCSECWCLIENT_H
#include "NCSECWClient.h"
#endif // NCSECWCLIENT_H
#endif // NCSJPC_ECW_SUPPORT

#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif // NCSMUTEX_H
#ifndef NCSJPCBUFFER_H
#include "NCSJPCBuffer.h"
#endif // NCSJPCBUFFER_H
#ifndef NCSJP2BOX_H
#include "NCSJP2Box.h"
#endif // NCSJP2BOX_H

#include "NCSEvent.h"

class CNCSJPCResample; //[04]

/**
 * CNCSJP2FileView class - the JP2 file view class.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.59.2.1 $ $Author: tom.lynch $ $Date: 2006/01/31 08:06:43 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJP2FileView: private CNCSThread {
public:
		/** Statistics enum */
	typedef enum {
			/** Global T1 Decoder US total */
		ST_CODEBLOCK_DECODER_US			= 0,	
			/** Global T1 Decoder samples total */
		ST_CODEBLOCK_DECODER_SAMPLES	= 1,	
			/** Global Codeblock read US total */
		ST_CODEBLOCK_READ_US			= 2,	
			/** Global Codeblock read samples total */
		ST_CODEBLOCK_READ_SAMPLES		= 3,
			/** DC Shift US total */
		ST_DCSHIFT_US					= 10,
			/** MCT US total */
		ST_MCT_US						= 20,
			/** DWT ROW US total */
		ST_DWT_ROW_US					= 30,
			/** DWT COL US total */
		ST_DWT_COL_US					= 31,
			/** DWT Samples total */
		ST_DWT_SAMPLES					= 32,
			/** SetView US total */
		ST_VIEW_SETVIEW_US				= 40,
			/** ReadLine US total */
		ST_VIEW_READLINE_US				= 41,
			/** Resample US total */
		ST_VIEW_RESAMPLE_US				= 42,
			/** Raw IO Read US total */
		ST_IO_READ_US					= 50,
			/** Raw IO Write US total */
		ST_IO_WRITE_US					= 51,
			/** Raw IO Seek US total */
		ST_IO_SEEK_US					= 52,
			/** Global Buffer Alloc US total */
		ST_BUF_ALLOC_US					= 60,	
			/** Global Buffer Free US total */
		ST_BUF_FREE_US					= 61,	
			/** Global Buffer bytes allocated total */
		ST_BUF_ALLOC_BYTES				= 62,	
			/** Global Buffer Copy US total */
		ST_BUF_COPY_US					= 63,	
			/** Global Buffer bytes copied total */
		ST_BUF_COPY_BYTES				= 64,	
			/** Global Buffer Clear US total */
		ST_BUF_CLEAR_US					= 65,	
			/** Global Buffer bytes cleared total */
		ST_BUF_CLEAR_BYTES				= 66,	
			/** File Compression Total MS */
		ST_COMPRESS_TOTAL_MS			= 80,	
			/** File Compression output size bytes total */
		ST_COMPRESS_OUTPUT_SIZE			= 81	
	} Statistic;

		/** JP2FileView parameters */
	typedef enum {
			/** Default, Baseline Profile 0 */
		JP2_COMPRESS_PROFILE_BASELINE_0		= 0, 
			/** Baseline Profile 1 */
		JP2_COMPRESS_PROFILE_BASELINE_1		= 1, 
			/** Profile 2 (unrestricted) */
		JP2_COMPRESS_PROFILE_BASELINE_2		= 2, 
			/** NITF NSIF BIIF NPJE Profile */
		JP2_COMPRESS_PROFILE_NITF_BIIF_NPJE = 3, 
			/** NITF NSIF BIIF EPJE Profile */
		JP2_COMPRESS_PROFILE_NITF_BIIF_EPJE = 4, 

			/** Resolution Levels.  UINT32 - Calculated so r=0 <= 64x64 */
		JP2_COMPRESS_LEVELS					= 20, 
			/** Quality Layers.  UINT32 - Default 1 */
		JP2_COMPRESS_LAYERS					= 21, 
			/** Precinct Width.  UINT32 - Default 64 or larger depending on file size */
		JP2_COMPRESS_PRECINCT_WIDTH			= 22, 
			/** Precinct Height.  UINT32 - Default 64 or larger depending on file size */
		JP2_COMPRESS_PRECINCT_HEIGHT		= 23, 
			/** Tile Width.  UINT32 - Default to image width given in SetFileInfo() */
		JP2_COMPRESS_TILE_WIDTH				= 24, 
			/** Tile Height.  UINT32 - Default to image height given in SetFileInfo() */
		JP2_COMPRESS_TILE_HEIGHT			= 25, 
			/** Output SOP.  bool - Default false */
		JP2_COMPRESS_INCLUDE_SOP			= 26, 
			/** Output EPH.  bool - Default true */
		JP2_COMPRESS_INCLUDE_EPH			= 27, 
			/** bool - Only write JPC codestream, no JP2 file wrapper */
		JP2_COMPRESS_CODESTREAM_ONLY		= 28, 
			/** bool - Enable/disable compression threaded read - Default true */
		JP2_COMPRESS_MT_READ				= 29, 

			/** LRCP progression */
		JP2_COMPRESS_PROGRESSION_LRCP		= 30, 
			/** RLCP progression */
		JP2_COMPRESS_PROGRESSION_RLCP		= 31, 
			/** v3.1 SDK Default: RPCL progression */
		JP2_COMPRESS_PROGRESSION_RPCL		= 32, 

			/** Control the precedence of georeferencing metadata from world files and embedded GML XML boxes and PCS UUID boxes */
		JP2_GEODATA_USAGE					= 40, 
			/** Control the precision employed by registration info (rounding errors can cause ridiculous values) */
		JP2_GEODATA_PRECISION_EPSILON       = 41,
			/** UINT32 - Defaults to all */
		JP2_DECOMPRESS_LAYERS				= 100, 
			/** IEEE4 - defaults to 0.0, range 0.0 <= r < 1.0 */
		JPC_DECOMPRESS_RECONSTRUCTION_PARAMETER = 101,
			/** Auto scale data up to read cell size, eg scale 1bit to 8bit when reading NCSCT_UINT8 */
		JPC_DECOMPRESS_AUTOSCALE_UP			= 102
	} Parameter;
		/**
		 * CNCSJP2FileViewVector class - vector of current JP2FileView instances.
		 * Also deals with progressive mode.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.59.2.1 $ $Author: tom.lynch $ $Date: 2006/01/31 08:06:43 $ 
		 */
	class NCSJPC_EXPORT_ALL CNCSJP2FileViewVector: public std::vector<CNCSJP2FileView *>, public CNCSThread {
	public:
			/** Default constructor */
		virtual ~CNCSJP2FileViewVector();
			/** Event - fired when something happens */
		CNCSEvent	m_Event;
			/** Progressive Mode Thread worker function */
		virtual void Work(void *pData);
	};

		/** The number of cells across the dataset */
	INT32 m_nWidth;				
		/** The number of lines down the dataset */
	INT32 m_nHeight;			
		/** The number of bands in the dataset */
	INT32 m_nNumberOfBands;	
		/** The actual compression ratio */	
	IEEE8 m_dCompressionRate;	
		/** The target compression ratio */
	IEEE8 m_dTargetCompressionRate;
		/** The X cell size in m_CellSizeUnits */
	IEEE8 m_dCellIncrementX;	
		/** The Y cell size in m_CellSizeUnits */
	IEEE8 m_dCellIncrementY;	
		/** The top left X origin of the dataset (world) */
	IEEE8 m_dOriginX;			
		/** The top left Y origin of the dataset (world) */
	IEEE8 m_dOriginY;			

		/** The GDT datum of the dataset */
	char *m_pDatum;				
		/** The GDT projection of the dataset */
	char *m_pProjection;		
		/** A pointer to the currently open filename */
	char *m_pFilename;			
		/** Cell units, meters, degrees or feet */
	CellSizeUnits m_CellSizeUnits;
		/** Cell type image should be read as */
	NCSEcwCellType m_eCellType;		
		/** ColorSpace of image */
	NCSFileColorSpace m_eColorSpace;		
		/** BandInfo for each band */
	std::vector<NCSFileBandInfo> m_Bands;
		/** The clockwise rotation of the image in degrees [02] */
	IEEE8 m_dCWRotationDegrees; 

		/** The number of bands in the current view */
	INT32 m_nSetViewNrBands;		
		/** A pointer to the band list for the current view */
	INT32 *m_pnSetViewBandList;		
		/** The current view width */
	INT32 m_nSetViewWidth;			
		/** The current view height */
	INT32 m_nSetViewHeight;			
		/** The current view world top left X */
	IEEE8 m_dSetViewWorldTLX;		
		/** The current view world top left Y */
	IEEE8 m_dSetViewWorldTLY;		
		/** The current view world bottom right X */
	IEEE8 m_dSetViewWorldBRX;		
		/** The current view world bottom right Y */
	IEEE8 m_dSetViewWorldBRY;		
		/** The current view dataset top left X */
	INT32 m_nSetViewDatasetTLX;		
		/** The current view dataset top left Y */
	INT32 m_nSetViewDatasetTLY;		
		/** The current view dataset bottom right X */
	INT32 m_nSetViewDatasetBRX;		
		/** The current view dataset bottom right Y */
	INT32 m_nSetViewDatasetBRY;		

		/** Default constructor, initialises members */
	CNCSJP2FileView();
		/** Virtual Destructor */
	virtual ~CNCSJP2FileView();
#ifdef NCS_BUILD_UNICODE
		/** 
		 * Open the JP2 file for Parsing/UnParsing.
		 * @param		pURLPath	Full path name of JP2 file to open
		 * @param		bProgressiveDisplay		Open for preogressive reading.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError Open(wchar_t *pURLPath, bool bProgressiveDisplay = false, bool bWrite = false);
#endif
		/** 
		 * Open the JP2 file for Parsing/UnParsing.
		 * @param		pURLPath	Full path name of JP2 file to open
		 * @param		bProgressiveDisplay		Open for preogressive reading.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError Open(char *pURLPath, bool bProgressiveDisplay = false, bool bWrite = false);
		/** 
		 * Open the JP2 file for Parsing/UnParsing on the specified stream.  The stream object
		 * may be a custom subclass of CNCSJPCIOStream with overridden IO methods.  You should
		 * not usually maintain a reference or pointer to the stream object after calling this 
		 * method due to the way the SDK handles the destruction of NCSJP2FileView objects, which
		 * may lead to uncertainty as to the validity of the stream object over time.
		 * @param		pStream		Input stream on which to open the file
		 * @param		bProgressiveDisplay		Open for preogressive reading.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError Open(CNCSJPCIOStream *pStream, bool bProgressiveDisplay = false);
		/** 
		 * Close the JP2 file
		 * @param		bFreeCache	Free the cached JP2 file data.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError Close ( bool bFreeCache = false );

		/** 
		 * Set a view into the JP2 file for reading.
		 * @param		nBands			Number of bands in pBandList to read
		 * @param		pBandList		Array of band indices to read.
		 * @param		nWidth			Width of the view in pixels
		 * @param		nHeight			Height of the view in pixels
		 * @param		nDatasetTLX		Top left X dataset coordinate of view
		 * @param		nDatasetTLY		Top left Y dataset coordinate of view
		 * @param		nDatasetBRX		Bottom right X dataset coordinate of view
		 * @param		nDatasetBRY		Bottom right Y dataset voordinate of view
		 * @param		dWorldTLX		Top left X world coordinate of view (informative only)
		 * @param		dWorldTLY		Top left Y world coordinate of view (informative only)
		 * @param		dWorldBRX		Bottom right X world coordinate of view (informative only)
		 * @param		dWorldBRY		Bottom right Y world coordinate of view (informative only)
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError SetView(UINT32 nBands, UINT32 *pBandList, 
					   		  UINT32 nDatasetTLX, UINT32 nDatasetTLY,
							  UINT32 nDatasetBRX, UINT32 nDatasetBRY,
							  UINT32 nWidth, UINT32 nHeight,
							  IEEE8 dWorldTLX = 0.0, IEEE8 dWorldTLY = 0.0,
							  IEEE8 dWorldBRX = 0.0, IEEE8 dWorldBRY = 0.0);

		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(UINT8 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(UINT16 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(UINT32 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(UINT64 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(INT8 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(INT16 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(INT32 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(INT64 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(IEEE4 **ppOutputLine);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(IEEE8 **ppOutputLine);
		/** 
		 * Read the next line in RGB UINT8 triplet format from the current view into the file.
		 * @param		pRGBTripler			Pointer to UINT8 buffer to receive RGB data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineRGB(UINT8 *pRGBTriplet);
		/** 
		 * Read the next line in BGR UINT8 triplet format from the current view into the file.
		 * @param		pBGRTripler			Pointer to UINT8 buffer to receive BGR data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBGR(UINT8 *pBGRTriplet);
		/** 
		 * Read the next line in RGBA UINT32 format from the current view into the file.
		 * @param		pRGBA				Pointer to UINT32 buffer to receive RGBA data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineRGBA(UINT32 *pRGBA);
		/** 
		 * Read the next line in BGRA UINT32 format from the current view into the file.
		 * @param		pBGRA				Pointer to UINT32 buffer to receive BGRA data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBGRA(UINT32 *pBGRA);
		/** 
		 * Read the next line in ARGB UINT32 format from the current view into the file.
		 * @param		pARGB				Pointer to UINT32 buffer to receive ARGB data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineARGB(UINT32 *pARGB);
		/** 
		 * Read the next line in ABGR UINT32 format from the current view into the file.
		 * @param		pABGR				Pointer to UINT32 buffer to receive ABGR data
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineABGR(UINT32 *pABGR);
		/** 
		 * Read the next line in BIL format from the current view into the file.
		 * @param		eType				Output buffer type
		 * @param		nBands				Number of output bands
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @param		pLineSteps			Line steps, in CELLS.
		 * @return      NCSEcwReadStatus	Read status code
		 */
	virtual NCSEcwReadStatus ReadLineBIL(NCSEcwCellType eType, UINT16 nBands, void **ppOutputLine, UINT32 *pLineSteps = NULL);

		/** 
		 * Write the next line in BIL format into the JP2 file.
		 * @param		eType				Output buffer type
		 * @param		nBands				Number of output bands
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @param		pLineSteps			Line steps, in CELLS.
		 * @return      CNCSError			Write status code
		 */
	virtual CNCSError WriteLineBIL(NCSEcwCellType eType, UINT16 nBands, void **ppOutputLine, UINT32 *pLineSteps = NULL);

		/** 
		 * More data is available and a refresh update should be done.
		 * @param		pSetViewInfo		Pointer to SetViewInfo containing details on view the update is for
		 * @return      NCSEcwReadStatus	Return the Read status code from the ReadLine*() call.
		 */
	virtual NCSEcwReadStatus RefreshUpdateEx(NCSFileViewSetInfo *pViewSetInfo);

		/** 
		 * Start the compression.
		 * In progressive (pull) mode scanlines will be sequentially
		 * read by the overloaded WriteReadLine() method
		 * @param		eType				Output buffer type
		 * @param		nBands				Number of output bands
		 * @param		ppOutputLine		Array of buffer pointers, one buffer for each band
		 * @param		pLineSteps			Line steps, in CELLS.
		 * @return      CNCSError			Write status code
		 */
	virtual CNCSError Write();	
		/** 
		 * Read input line for compression.
		 * In progressive (pull) mode scanlines will be sequentially
		 * read by the overloaded WriteReadLine() method
		 * @param		nNextLine			Next input line to read
		 * @param		ppInputArray		Array of buffer pointers, one buffer for each band
		 * @return      CNCSError			Write status code
		 */
	virtual CNCSError WriteReadLine(UINT32 nNextLine, void **ppInputArray);	
		/** 
		 * Compression status callback.
		 * you overload this if you want a status callback to show progress during compression
		 * @param		nCurrentLine		Current input line processed
		 */
	virtual void WriteStatus(UINT32 nCurrentLine);
		/** 
		 * Compression cancel callback.
		 * you overload this if you want to be able to cancel during compression
		 * @return      bool				Return true to cancel, else false to continue
		 */
	virtual bool	WriteCancel(void);

		/** 
		 * Get current ViewSetinfo structure.
		 * @return      NCSFileViewSetInfo*	Return pointer to the current SetViewInfo.
		 */
	virtual /*const*/ NCSFileViewSetInfo *GetFileViewSetInfo();
		/** 
		 * Get FileInfo structure.
		 * @return      NCSFileViewFileInfo*	Return pointer to the FileInfo.
		 */
	virtual /*const*/ NCSFileViewFileInfoEx *GetFileInfo();
		/** 
		 * Set FileInfo structure.
		 * @param		Info		New fileinfo - used to specify file info for compression
		 * @return      CNCSError	Return pointer to the FileInfo.
		 */
	virtual CNCSError SetFileInfo(NCSFileViewFileInfoEx &Info);

		/**
		  * Get the next UUID box with the specified UUID from the file.
		  * @param		uuid			UUID of UUID box to find.
		  * @param		pLast			Last UUID box found, else NULL for first
		  * @return		CNCSJP2Box*		Pointer to box if found, else NULL;
		  */
	virtual CNCSJP2Box *GetUUIDBox(NCSUUID uuid, CNCSJP2Box *pLast = NULL);
		/**
		  * Get the next XML box from the file.
		  * @param		pLast			Last XML box found, else NULL for first
		  * @return		CNCSJP2Box*		Pointer to box if found, else NULL;
		  */
	virtual CNCSJP2Box *GetXMLBox(CNCSJP2Box *pLast = NULL);
		/**
		  * Get the next box of the specified type from the file.
		  * @param		nTBox			Box type to find
		  * @param		pLast			Last box found, else NULL for first
		  * @return		CNCSJP2Box*		Pointer to box if found, else NULL;
		  */
	virtual CNCSJP2Box *GetBox(UINT32 nTBox, CNCSJP2Box *pLast = NULL);
		/**
		  * Add a box to be written to the file on compression.
		  * The specified box's UnParse() method will be called to write the box into the file
		  * @param		pBox			Box to be written to file
		  * @return		CNCSError		returned error or NCS_SUCCESS;
		  */
	virtual CNCSError AddBox(CNCSJP2Box *pBox);


	/**
		  * Get a GDT (ER Mapper) Projection/Datum pair for the given EPSG code, if available
		  * @param		nEPSGCode		EPSG Code to find projection/datum for
		  * @param		ppProjection	Returned GDT Projection name - free with NCSFree()
		  * @param		ppDatum			Returned GDT Datum name - free with NCSFree()
		  * @return		CNCSError		Error code;
		  */
	static CNCSError GetProjectionAndDatum(const INT32 nEPSGCode, char **ppProjection, char **ppDatum);
		/**
		  * Get an EPSG code from the given GDT (ER Mapper) Projection/Datum pair if available
		  * @param		pProjection		GDT Projection name
		  * @param		pDatum			GDT Datum name
		  * @param		nEPSGCode		Returned EPSG Code or 0 on error
		  * @return		CNCSError		Error code;
		  */
	static CNCSError GetEPSGCode(char *pProjection, char *pDatum, INT32 *nEPSGCode);
		/**
		  * Get the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
		  * @return the custom path being used, or NULL
		  */
	static char *GetGDTPath();
		/**
		  * Set the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
		  * @param szPath the new GDT data path
		  */
	static void SetGDTPath(const char *szPath);
		/**
		  * Refresh the custom GDT data path, doing a search and validity check on 
		  * the current path value and several common GDT data locations
		  */
	static void DetectGDTPath();

		/** 
		  * Set refresh callback funnction 
		  * @param		pCallback		Refresh callback function to use
		  * @return		CNCSError		Error code;
		  */
	CNCSError SetRefreshCallback(NCSEcwReadStatus (*pCallback)(NCSFileView*));

		/**
		  * Set Compress Client - Internal func for "C" API support only
		  * @param		pCompressClient	ECW Compress Client struct
		  * @return		CNCSError		Error code;
		  */		 
	CNCSError SetCompressClient(struct NCSEcwCompressClient *pCompressClient);

		/** 
		 * Get underlying CNCSJP2File pointer.
		 * @return      CNCSJP2File*	Return pointer to the CNCSJP2File instance.
		 */
	class CNCSJP2File *GetFile();
#ifdef NCSJPC_ECW_SUPPORT
		/** 
		 * Get underlying NCSFileView pointer, where it exists.
		 * @return      NCSFileView*	Return pointer to the NCSFileView instance.
		 */
	NCSFileView *GetNCSFileView();
#endif

		/** 
		 * Get a pointer to the underlying CNCSJPCIOStream.
		 * This can be used to read specific JP2 UUID/XML boxes etc.
		 * This may also be useful if the underlying stream object belongs to a 
		 * custom subclass of CNCSJPCIOStream that you have passed to the overloaded
		 * method CNCSFile::Open(CNCSJPCIOStream *pStream, bool bProgressiveDisplay).
		 * @return      CNCSJPCIOStream*	Return pointer to the CNCSJPCIOStream, else NULL.
		 */
	CNCSJPCIOStream *GetStream();

		/**
		  * Find the CNCSJP2FileVIew instance pointer for a given NCSFileView structure
		  * @param		pNCSFileView		NCSFileView to find
		  * @return		CNCSJP2FileView*	CNCSJP2FileView instance if found, else NULL
		  */		 
	static CNCSJP2FileView *FindJP2FileView(NCSFileView *pNCSFileView);

		/**
		  * Get a statistic of NCSTimeStampMs type
		  * @param		eType				Statistic to find
		  * @param		Val					Statistic value
		  */		 
	void GetStatistic(Statistic eType, NCSTimeStampMs &Val);
		/**
		  * Get a statistic of UINT64 type
		  * @param		eType				Statistic to find
		  * @param		Val					Statistic value
		  */		 
	void GetStatistic(Statistic eType, UINT64 &Val);
		/**
		  * Reset the specified statistic to the default value
		  * @param		eType				Statistic to reset
		  */		 
	void ResetStatistic(Statistic eType);
		/**
		  * Reset all statistics to their default values
		  */		 
	void ResetStatistics();

		/**
		  * Set a parameter on the view.  These over-ride default settings, such as compression output profile,
		  * progression order, tiling etc
		  * @param		eType			Parameter type to set.
		  */
	void SetParameter(Parameter eType);
		/**
		  * Set a parameter on the view.  These over-ride default settings, such as compression output profile,
		  * progression order, tiling etc
		  * @param		eType			Parameter type to set.
		  * @param		bBool			Boolean parameter true/false
		  */
	void SetParameter(Parameter eType, bool bBool);
		/**
		  * Set a parameter on the view.  These over-ride default settings, such as compression output profile,
		  * progression order, tiling etc
		  * @param		eType			Parameter type to set.
		  * @param		nValue			UINT32 parameter value to set
		  */
	void SetParameter(Parameter eType, UINT32 nValue);
		/**
		  * Set a parameter on the view.  These over-ride default settings, such as compression output profile,
		  * progression order, tiling etc
		  * @param		eType			Parameter type to set.
		  * @param		fValue			IEEE4 parameter value to set
		  */
	void SetParameter(Parameter eType, IEEE4 fValue);
		/**
		  * Set the usage of geographical metadata when reading and writing JPEG 2000 files.
		  * @param		nGeodataUsage	GeodataUsage enum value specifying the desired metadata types
		  */
	static void SetGeodataUsage(GeodataUsage nGeodataUsage);

		/**
		  * Get the percent complete (from time of setview == 0%)
		  * @return		INT32			Returned Percent complete
		  */		 
	INT32 GetPercentComplete();

		/**
		  * Get the percent complete as a percentage of total blocks in the current view.
		  * @return		INT32			Returned Percent complete
		  */		 
	INT32 GetPercentCompleteTotalBlocksInView();

		/**
		  * Get the EPSG code associated with this view's projection and datum, if any.
		  * @return		INT32			EPSG code, or 0 if none applies.
		  */
	INT32 GetEPSGCode();

		/**
		  * Internal shutdown function.
		  */		 
	static void Shutdown();

		/**
		  * Call this function to enable unlimited compression.
		  * NOTE: Verify you are in compliance with the appropriate license agreements.
		  * Calling this function signifies you accept the terms of the appropriate license.
		  */		 
	static void SetKeySize(void);

		/**
		  * Get the type of the currently open file
		  * @return		NCSFileType			The file type.
		  */
	NCSFileType GetFileType();

		/**
		  * Get the Mime type of the currently open file
		  * @return		The Mime Type string or NULL if error.
		  */
	char *GetFileMimeType();
	
		/**
		  * Return the Mime type of a file given the file type.
		  * @param		NCSFileType			The file type.
		  * @return		The Mime Type string or NULL if error.
		  */
	static char *GetFileMimeType(NCSFileType fileType);

protected:
		/** Timestamp representing lat time a blocking read was called */
	NCSTimeStampMs	m_tsLastBlockTime;
		/** Next line in view to read */
	UINT32		m_nNextLine;
	
		/**
		  * Fire a refresh update,
		  * @param		_Lock				Global lock to release during call.
		  * @return		NCSEcwReadStatus	RefreshUpdate Read status
		  */
	NCSEcwReadStatus FireRefreshUpdate(CNCSJPCGlobalLock &_Lock);
		/** 
		 * Open the JP2 file for Parsing/UnParsing.
		 * @param		pURLPath	Full path name of JP2 file to open
		 * @param		pStream		IO Stream to open on
		 * @param		bProgressiveDisplay		Open for preogressive reading.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError OpenForWrite(char *pURLPath, CNCSJPCIOStream *pStream, bool bProgressiveDisplay = false);
	
friend class CNCSJPCResample;
		/** Add a dynamically allocated node to the dynamic node list so it is released */
	void AddDynamicNode(class CNCSJPCNode *pNode) { m_DynamicNodes.push_back(pNode); };

protected:
friend class CNCSJP2FileViewVector;
		/** Display update is progressive for this view */
	bool m_bIsProgressive;
		/** SetView is valid */
	bool m_bHaveValidSetView;
		/** Set view mode is world? */
	bool m_bSetViewModeIsWorld;
		/** File is open */
	bool m_bIsOpen;
		/** File is open */
	bool m_bIsWrite;
		/** Autoscale data up on read */
	bool m_bAutoScaleUp;

		/** CNCSJP2File */
	CNCSJP2File *m_pFile;
		/** ECW SetViewInfo Structure */
	NCSFileViewSetInfo m_CurrentView;
		/** Do we have a pending setview */
	bool	m_bPendingView;
		/** Progressive pending View */
	NCSFileViewSetInfo m_PendingView;
		/** JP2 Compression Output node */
	class CNCSJPCNode *m_pOutputNode;

private:
	/**
	 * ParameterValuePair class - holds compression parameters prior to CNCSJP2File instance being created on the Open()
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.59.2.1 $ $Author: tom.lynch $ $Date: 2006/01/31 08:06:43 $ 
	 */
	class NCSJPC_EXPORT_ALL ParameterValuePair {
	public:
			/** Parameter name */
		Parameter m_eParam;
			/** Parameter value */
		void	*m_pValue;

			/** Default constructor */
		ParameterValuePair();
			/** Copy constructor */
		ParameterValuePair(const ParameterValuePair &Src);
			/** No-parameter constructor */
		ParameterValuePair(Parameter eParam);
			/** bool-parameter constructor */
		ParameterValuePair(Parameter eParam, bool bValue);
			/** UINT32-paramater constructor */
		ParameterValuePair(Parameter eParam, UINT32 nValue);
			/** IEEE4-parameter constructor */
		ParameterValuePair(Parameter eParam, IEEE4 fValue);
			/** Virtual destructor */
		virtual ~ParameterValuePair();
	};

		/** Number of blocks in cache at the last SetView() */
	UINT32	m_nBlocksAtLastRefreshUpdate;
		/** Current in RefreshUpdate() callback */
	bool	m_bInRefreshUpdate;
		/** Cancel the read */
	bool	m_bCancelRead;
		/** Performing low-memory JP2 compression */
	bool	m_bLowMemCompress;
		/** Timestamp of last refreshupdate for this view */
	NCSTimeStampMs m_tsLastRefreshUpdate;

		/** parameters held until open() */
	std::vector<ParameterValuePair> m_Parameters;
		/** Copy the parameters from the m_Parameters vector to the CNCSJP2File instance */
	void SetFileParameters();
		/** List of other JP2 box(es) to be included in the output file */
	CNCSJP2Box::CNCSJP2BoxList m_OtherBoxes;	

		/** Used when SetView is >4000 in either dimension - too large to cache */
	bool	m_bTiledView;
		/** This member stores a view "tile" (horizontal strip) when view > 4K x 4K */
	CNCSJP2FileView *m_pTiledView;
		/** Set the tiled views */
	CNCSError SetTiledView(NCSFileViewSetInfo &View, UINT32 nReadLine);

		/** Static list of file views */
	static CNCSJP2FileViewVector sm_Views;
		/** Dynamic Nodes to free */
	CNCSJPCNodeVector m_DynamicNodes;

		// The following private members are here to support the "C" API only.
		/** Progressive read callback for ECW decompression */
	NCSEcwReadStatus (*m_pRefreshCallback)(NCSFileView *pNCSFileView);
		/** ECW Compress Client for "C" compression API */
	struct NCSEcwCompressClient *m_pCompressClient;

		/** Compression Start timestamp */
	NCSTimeStampMs	m_tsCompressStart;
		/** Compression End timestamp */
	NCSTimeStampMs	m_tsCompressEnd;

		/** JP2 compression progressive buffer queue */
	std::vector<CNCSJPCBufferVector*> m_CompressReadQueue;
		/** Read event */
	CNCSEvent	m_CompressReadEvent;
		/** Compression free buffer queue */
	std::vector<CNCSJPCBufferVector*> m_CompressFreeQueue;
		/** Compression free event */
	CNCSEvent	m_CompressFreeEvent;
		/** Compression queue mutex */
	CNCSMutex		m_CompressQueueMutex;
		/** Compression Error status */
	CNCSError		m_CompressError;
		/** Multi-thread compression */
	bool			m_bCompressMTRead;
#ifdef NCSJPC_ECW_SUPPORT
		/** ECW File reading struct instance */
	NCSFileView	*m_pECWFileView;
		/** Static RefreshUpdate callback hook */
	static NCSEcwReadStatus sRefreshCallback(NCSFileView *pNCSFileView);

		/** ECW File writing struct instance */
	struct EcwCompressionTask *m_pECWCompressionTask;

		/** Static Compression read thread function hook */
	static void sECWCompressThreadReadFunc(void *pData);
		/** Static Compression read callback hook */
	static NCSError	sECWCompressRead(void *pClient, UINT32 nNextLine, IEEE4 **ppInputArray);	
		/** Static Compression status callback hook */
	static void sECWCompressStatus(void *pClient, UINT32 nCurrentLine);
		/** Statuc Compression Cancel callback hook */
	static BOOLEAN	sECWCompressCancel(void *pClient);
#endif // NCSJPC_ECW_SUPPORT

		/** JP2 resampler */
	CNCSJPCResample	*m_pResampler;

		/** Scanline input buffer */
	CNCSJPCBuffer m_InputBuf;

		/** Update the public file & view members */
	bool UpdateFileViewInfo(void);

		/** Delete all dynamic nodes */
	void DeleteDynamicNodes(void);

		/** Update the view statistics */
	void UpdateViewStats();

		/** Thread Work func for JP2 progressive compression */
	void Work(void *pDate);
};

#endif // !NCSJP2FILEVIEW_H
