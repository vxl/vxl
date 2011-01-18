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
** FILE:   	ecw_open.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Open/read routines for ECW compressed image files
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns 01Mar98 Added NCS related enhancements
** [03] sns 02Apr99 adding Windows specific file IO logic instead of fopen() to bypass max 512 file limit
** [04] sns 02Apr99	Updated to use fast floating point conversions on x86 platforms
** [05] sns 07Apr99 Noise added to Intensity for YUV to add texture appeal
** [06] sjc 15Apr99 Changed to calloc() to fix UMR - causing random pixels in corners.
** [07] sns 09May99 back computed target compression ratio for V 1.0 files
** [08] sns 09May99 Updated texture noise: optional, and only for YUV and Greyscale
** [09] sns 10May99 Moved read routines into new file "ecw_read.c"
** [10] sns 31May99 Only adding texture noise if at larger QMF levels
** [11] sns 04Jun99	Updated to use an internal Random function
** [12] rar	17Jan01 Mac port changes
** [13] rar 18Sep01 Modified erw_decompress_open so that if the Block Table is uncompressed (RAW)
**                  it is not passed through unpack_data (which just does a memcpy anyway)
** [14] sjc 12Jun03 Randomize the start value for the random function to stop noticable artifacts with small tiled output.
** [15] jx  12Feb04 initialise nCounter to 0. nCounter is added to QmfRegionStruct to fix rounding error in erw_decompress_read_region_line_bil(). 
 *******************************************************/



#include "NCSEcw.h"

#if !defined(_WIN32_WCE)
	#ifdef WIN32 //[12]
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**
**	erw_decompress_open() at al.  Call with the name of the file to open.
**	Returns a pointer to the ERW structure, or NULL if an error occurred.
**
**	Input filename can be either the ERS or the ERW file - it does not matter
**
**	This routine is called once to open an ERW compressed file
**	to read it.  Once opened using erw_decompress_open(), it can be read multiple times
**	at different resolutions or geographic resolutions using erw_decompress_start_region().
**	So you can make multiple region calls (at the same time or sequentially) to a file
**	that has been opened.
**
**	The sequence of usage is:
**
**	erw_decompress_open()				- open a file
**	erw_decompress_start_region()		- define a region to be read at specified resolution
**	erw_decompress_read_region_line()	- read the next line from the region
**	erw_decompress_end_region()			- end reading a region.  Call at end of region, or when aborting read
**	erw_decompress_close()				- close a file
**
**
**	For maximum performance, you should minimize the number of
**	erw_decompress_open() calls. It is faster to have a single open then multiple
**	region calls for that single open.
**	[later note: files are now cached, so multiple open's don't have as much
**	overhead any more]
**
**	You can have multiple open_region() calls open for a single file, or open the file multiple
**	times. Either technique works and are just as efficient.
**
**	If you are aborting a read of a region, but plan to read another region from the
**	same file, just call erw_decompress_end_region(). If you are aborting all access
**	to the file, call erw_decompress_end_region() and then erw_decompress_close().
**
**	WARNING: do not close the file with one or more start_region() calls still open.
**	This will corrupt memory.  You should clean up all outstanding close_region()
**	calls before calling the shutdown stage.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/


/*
**	Open a QMF file for multiple region reads. File details can be
**	from a file, or directly from a memory image of the header
*/

QmfLevelStruct *erw_decompress_open( char *p_input_filename,
			UINT8	*pMemImage,			// if non-NULL, open the memory image not the file
			BOOLEAN bReadOffsets,
			BOOLEAN bReadMemImage )
{
	Byte	the_byte;
	UINT8	temp_byte;
	UINT16	temp_int16;
	UINT32	temp_int;
	int		error, b;

	ECWFILE	hEcwFile = NULL_ECWFILE;		// [03]
	BOOLEAN	bEcwFileOpen = FALSE;	// [03]

	UINT16	num_levels = 0;
	UINT16  level = 0;
	UINT32	x_size = 0;
	UINT32  y_size = 0;
	FLOAT scale_factor = 1.0f;
	UINT16	x_block_size = 0;
	UINT16  y_block_size = 0;
	UINT8	qmf_version = 0;
	BlockingFormat	qmf_blocking_format = BLOCKING_LEVEL;
	CompressFormat	qmf_compress_format = COMPRESS_NONE;
	UINT8	qmf_nr_sidebands = 0;
	UINT16	qmf_nr_bands = 0;
	QmfLevelStruct	*p_top_qmf, *p_smaller_qmf, *p_qmf, *p_file_qmf;
	UINT8	*pIntoMemImage;	// pointer into current part of MemImage being read

	// VERSION 2.0 INFORMATION
	UINT16	nCompressionRate = 1;
	CellSizeUnits	eCellSizeUnits = ECW_CELL_UNITS_METERS;
	IEEE8	fCellIncrementX = 1.0;
	IEEE8	fCellIncrementY = 1.0;
	IEEE8	fOriginX		  = 0.0;
	IEEE8	fOriginY		  = 0.0;
	char	szDatum[ECW_MAX_DATUM_LEN]			  = "RAW";	// start with a default Datum
	char	szProjection[ECW_MAX_PROJECTION_LEN]  = "RAW";	// start with a default Projection


	// If reading from memory, set the pIntoMemImage pointer
	if( pMemImage ) {
		pIntoMemImage = pMemImage;
		bReadOffsets = FALSE;
		bReadMemImage = FALSE;
	}
	else
		pIntoMemImage = NULL;

	/*
	**	Read the ECW file, and set up values.
	**	IMPORTANT!! there are two different set ups here, and in the QMF levels further down.
	**	If you change the header, you must change BOTH the memory and the file reader version.
	**	Look for all locations marked [HEADER] and update them as required
	*/
	if( pIntoMemImage ) {
		// [HEADER] read main header from memory image
		error = 0;
		temp_byte = *pIntoMemImage++;
		if (temp_byte != (the_byte = ECW_HEADER_ID_TAG)) { 
//			ERS_setup_error(ERS_RASTER_ERROR,"The file is not a valid ECW file.");
			error = 1;
		}
		else {
			qmf_version = *pIntoMemImage++;
			temp_byte = *pIntoMemImage++;				qmf_blocking_format = temp_byte;
			temp_byte = *pIntoMemImage++;				qmf_compress_format = temp_byte;
			temp_byte = *pIntoMemImage++;				num_levels = (UINT16) temp_byte;
			temp_byte = *pIntoMemImage++;				qmf_nr_sidebands = temp_byte;
			temp_int = sread_int32(pIntoMemImage);
			pIntoMemImage += 4;							x_size = (int) temp_int;
			temp_int = sread_int32(pIntoMemImage);
			pIntoMemImage += 4;							y_size = (int) temp_int;
			temp_int16 = sread_int16(pIntoMemImage);
			pIntoMemImage += 2;							qmf_nr_bands = temp_int16;
			temp_int16 = sread_int16(pIntoMemImage);
			pIntoMemImage += 2;							scale_factor = (IEEE4) temp_int16;
			x_block_size = sread_int16(pIntoMemImage);	pIntoMemImage += 2;
			y_block_size = sread_int16(pIntoMemImage);	pIntoMemImage += 2;
			if( ERSWAVE_VERSION < qmf_version ) {
//				ERS_setup_error(ERS_RASTER_ERROR,
//					"This compressed file is a more recent version than this software, and can not be read. Get the latest software version from www.ermapper.com.");
				error = 1;
			}

			// some more sanity checking
			if( num_levels == 0 || qmf_nr_bands == 0 || scale_factor == (IEEE4) 0.0 ) {
//				ERS_setup_error(ERS_RASTER_ERROR,"The compressed ECW file has an invalid header.");
				error = 1;
			}


			// VERSION 2.0 INFORMATION
			if( qmf_version > 1 ) {
				temp_int16 = sread_int16(pIntoMemImage);
				pIntoMemImage += 2;							nCompressionRate = temp_int16;
				temp_byte  = *pIntoMemImage++;				eCellSizeUnits	 = temp_byte;
				sread_ieee8( &fCellIncrementX, pIntoMemImage);	pIntoMemImage += 8;
				sread_ieee8( &fCellIncrementY, pIntoMemImage);	pIntoMemImage += 8;
				sread_ieee8( &fOriginX,		 pIntoMemImage);	pIntoMemImage += 8;
				sread_ieee8( &fOriginY,		 pIntoMemImage);	pIntoMemImage += 8;

				strcpy(szDatum, (const char*)pIntoMemImage);			 pIntoMemImage += ECW_MAX_DATUM_LEN;
				strcpy(szProjection,(const char*)pIntoMemImage);		 pIntoMemImage += ECW_MAX_PROJECTION_LEN;	
			}
		}
		if( error )
			return(NULL);
	}
	else {
		// [HEADER] read main header from file
		if( EcwFileOpenForRead(p_input_filename, &hEcwFile) )
			bEcwFileOpen = FALSE;
		else
			bEcwFileOpen = TRUE;

		error = 0;
		if( bEcwFileOpen ) {
			EcwFileReadUint8(hEcwFile, &temp_byte);
			if (temp_byte != (the_byte = ECW_HEADER_ID_TAG)) { 
//				ERS_setup_error(ERS_RASTER_ERROR,"The file is not a valid ECW file.");
				error = 1;
			}
			else {
				EcwFileReadUint8(hEcwFile, &qmf_version);
				EcwFileReadUint8(hEcwFile, &temp_byte);				qmf_blocking_format = temp_byte;
				EcwFileReadUint8(hEcwFile, &temp_byte);				qmf_compress_format = temp_byte;
				EcwFileReadUint8(hEcwFile, &temp_byte);				num_levels = (UINT16) temp_byte;
				EcwFileReadUint8(hEcwFile, &temp_byte);				qmf_nr_sidebands = temp_byte;
				EcwFileReadUint32(hEcwFile, &temp_int);				x_size = (int) temp_int;
				EcwFileReadUint32(hEcwFile, &temp_int);				y_size = (int) temp_int;
				EcwFileReadUint16(hEcwFile, &temp_int16);			qmf_nr_bands = temp_int16;
				EcwFileReadUint16(hEcwFile, &temp_int16);			scale_factor = (IEEE4) temp_int16;
				EcwFileReadUint16(hEcwFile, &x_block_size);
				EcwFileReadUint16(hEcwFile, &y_block_size);
				if( ERSWAVE_VERSION < qmf_version ) {
//					ERS_setup_error(ERS_RASTER_ERROR,
//						"This compressed file is a more recent version than this software, and can not be read. Get the latest software version from www.ermapper.com.");
					error = 1;
				}

				// some more sanity checking
				if( num_levels == 0 || qmf_nr_bands == 0 || scale_factor == (IEEE4) 0.0 ) {
//					ERS_setup_error(ERS_RASTER_ERROR,"The compressed ECW file has an invalid header.");
					error = 1;
				}

				// VERSION 2.0 INFORMATION
				if( qmf_version > 1 ) {
					EcwFileReadUint16(hEcwFile, &temp_int16);			nCompressionRate = temp_int16;
					EcwFileReadUint8(hEcwFile, &temp_byte);				eCellSizeUnits = temp_byte;
					EcwFileReadIeee8(hEcwFile,  &fCellIncrementX);
					EcwFileReadIeee8(hEcwFile,  &fCellIncrementY);
					EcwFileReadIeee8(hEcwFile,  &fOriginX);
					EcwFileReadIeee8(hEcwFile,  &fOriginY);
					EcwFileRead(hEcwFile, szDatum, ECW_MAX_DATUM_LEN);
					EcwFileRead(hEcwFile, szProjection, ECW_MAX_PROJECTION_LEN);
				}
			}
		}
		else {
//			ERS_setup_error(ERS_RASTER_ERROR,"Unable to open compressed file.");
			error = 1;
		}
		if( !bEcwFileOpen || error ) {
			if( bEcwFileOpen )
				EcwFileClose(hEcwFile);
			return(NULL);
		}
	}

	/*
	**	The ERW file appears to be valid.
	**	Now create the fake file QMF level of the tree
	*/
	p_file_qmf = new_qmf_level(x_block_size, y_block_size, num_levels, x_size, y_size, qmf_nr_bands, NULL, NULL, FALSE);
	if( !p_file_qmf ) {
		if( bEcwFileOpen )
			EcwFileClose(hEcwFile);
		return(NULL);
	}
	p_file_qmf->version = qmf_version;
	p_file_qmf->blocking_format = qmf_blocking_format;
	p_file_qmf->compress_format = qmf_compress_format;
	p_file_qmf->nr_sidebands = qmf_nr_sidebands;
	p_file_qmf->scale_factor = scale_factor;
	p_file_qmf->nr_levels = (UINT8) num_levels;		// [02]

	/*
	**	Now cycle through the levels of the ERW file, getting each level information
	*/
	p_top_qmf = p_smaller_qmf = p_qmf = NULL;
	for(level = 0; level < num_levels; level++ ) {
		UINT8	qmf_level;
		UINT32	qmf_x_size;
		UINT32	qmf_y_size;
		UINT32	band;

		/*
		**	Read level number, X size, Y size
		*/
		if( pIntoMemImage ) {
			// [HEADER] read a QMF header from memory image
			qmf_level = *pIntoMemImage++;									// level number
			qmf_x_size = sread_int32(pIntoMemImage); pIntoMemImage += 4;	// level X size
			qmf_y_size = sread_int32(pIntoMemImage); pIntoMemImage += 4;	// level Y size
		}
		else {
			// [HEADER] read a QMF header from file
			if( (error = EcwFileReadUint8(hEcwFile, &qmf_level)) != 0 )				// level number
				break;
			if( (error = EcwFileReadUint32(hEcwFile, &qmf_x_size)) != 0 )				// level X size
				break;
			if( (error = EcwFileReadUint32(hEcwFile, &qmf_y_size)) != 0 )				// level Y size
				break;
		}


		error = 1;
		if( qmf_level != level
		 || qmf_x_size >= x_size
		 || qmf_y_size >= y_size ) {
			error = 1;			// Error: compressed file error. Unexpected level number
			break;
		}
		p_qmf = new_qmf_level( p_file_qmf->x_block_size,  p_file_qmf->y_block_size,
							   qmf_level, qmf_x_size, qmf_y_size, qmf_nr_bands, p_smaller_qmf, NULL, FALSE);
		if( !p_qmf )
			break;
		if( !p_top_qmf )
			p_top_qmf = p_qmf;	/* first level, so ensure we point to it */

		p_qmf->p_top_qmf		= p_top_qmf;
		p_qmf->version			= p_file_qmf->version;
		p_qmf->blocking_format	= p_file_qmf->blocking_format;
		p_qmf->compress_format	= p_file_qmf->compress_format;
		p_qmf->nr_sidebands		= p_file_qmf->nr_sidebands;
		p_qmf->scale_factor		= p_file_qmf->scale_factor;
		p_qmf->nr_levels		= (UINT8) num_levels;	// [02]

		p_qmf->nr_x_blocks = QMF_LEVEL_NR_X_BLOCKS(p_qmf);
		p_qmf->nr_y_blocks = QMF_LEVEL_NR_Y_BLOCKS(p_qmf);

		p_qmf->p_file_qmf = p_file_qmf;
		if( p_smaller_qmf )
			p_smaller_qmf->p_larger_qmf = p_qmf;
		p_smaller_qmf = p_qmf;

		/*
		**	Read binsizes for this level, one value per band (e.g. 1 band = greyscale; 3 = RGB or YIQ)
		*/
		if( pIntoMemImage ) {
			// [HEADER] read a QMF binsizes from memory image
			for( band = 0; band < qmf_nr_bands; band++ ) {
				p_qmf->p_band_bin_size[band] = sread_int32( pIntoMemImage );
				pIntoMemImage += 4;
			}
		}
		else {
			// [HEADER] read a QMF binsizes from file
			for( band = 0; band < qmf_nr_bands; band++ ) {
				if( (error = EcwFileReadUint32(hEcwFile, &p_qmf->p_band_bin_size[band])) != 0 )
					break;
			}
		}
		error = 0;
	}

	if( error ) {
		if( bEcwFileOpen )
			EcwFileClose(hEcwFile);
		delete_qmf_levels(p_top_qmf);
		return(NULL);
	}
	/* [07] for v 1.0 files, calculate what target compression ratio was based on scale factor */
	if( qmf_version == 1 ) {
		UINT32	nBinSize = p_qmf->p_band_bin_size[0];
		if( p_qmf->compress_format == COMPRESS_YUV )
			nBinSize = nBinSize / 2;		/* YUV has Y at 1/2 desired compression rate */
		if( p_qmf->scale_factor >= 1 )
			nCompressionRate = (UINT16)(p_qmf->p_band_bin_size[0] / (UINT32) p_qmf->scale_factor);
	}

	/* Attach the file qmf to the end of the tree, currently pointed to by p_qmf */
	p_qmf->p_larger_qmf = p_file_qmf;
	p_file_qmf->p_smaller_qmf = p_qmf;
	p_file_qmf->p_top_qmf = p_top_qmf;

	/*
	**	[02] read file memory image if it is required
	*/
	if( bReadMemImage) {
		UINT64	nOffsetPos;

		EcwFileGetPos(hEcwFile, &nOffsetPos );

		p_top_qmf->nHeaderMemImageLen = (UINT32) nOffsetPos;
		p_top_qmf->pHeaderMemImage = (UINT8 *) NCSMalloc(p_top_qmf->nHeaderMemImageLen, FALSE);

		if(!p_top_qmf->pHeaderMemImage) {
			return NULL;
		}
		nOffsetPos = 0;
		EcwFileSetPos(hEcwFile, nOffsetPos);

		if( EcwFileRead(hEcwFile, p_top_qmf->pHeaderMemImage, p_top_qmf->nHeaderMemImageLen) ) {
			EcwFileClose(hEcwFile);
			delete_qmf_levels(p_top_qmf);
			return(NULL);
		}

		nOffsetPos = p_top_qmf->nHeaderMemImageLen;
		EcwFileSetPos(hEcwFile, nOffsetPos);
	}
	/*
	**	Unpack block offsets from the file. Once this is done, the first block will
	**	be the next location on disk, so we also record that position for later use.
	**	Note that all block offsets for all levels are stored, so we grab them all,
	**	and write them into the p_top_qmf pointer.  The allocation logic already
	**	pointed larger QMF sections their portion of this array.
	**
	*/
//	if( bReadOffsets ) {	// [02]
	if( !pIntoMemImage ) {
		while( TRUE ) {			// we only loop once. This is handy so we can break on errors
			UINT8	*p_packed = (UINT8*)NULL;
			UINT8	*p_unpacked;
			UINT32	unpacked_length;
			UINT32	packed_length;
			UINT8 eFormat = (UINT8)ENCODE_RAW;

			if( (error = EcwFileReadUint32(hEcwFile, &packed_length )) != 0 )
				break;
			unpacked_length = get_qmf_tree_nr_blocks(p_top_qmf) * sizeof(p_top_qmf->p_block_offsets[0]);
			error = 1;
			if( packed_length > unpacked_length + 1 )		// sanity check before the malloc
				break;

			
			if( EcwFileRead(hEcwFile, &eFormat, 1) ) {
				error = TRUE;
				break;
			}
			if(eFormat == ENCODE_RAW) {
				UINT64 nPos = 0;
				p_top_qmf->bRawBlockTable = TRUE;
				p_top_qmf->p_block_offsets = (UINT64 *)NULL;

				EcwFileGetPos(hEcwFile, &nPos);
				error = EcwFileSetPos(hEcwFile, nPos + packed_length - 1);
			} else if(bReadOffsets) {
				p_packed = (UINT8 *) NCSMalloc(packed_length + sizeof(UINT64) - 1, FALSE);
//			p_packed = (UINT8 *) NCSMalloc(packed_length, FALSE);

				if( !p_packed )
					break;

				memset(p_packed, 0, sizeof(UINT64) - 1);

				*(p_packed + sizeof(UINT64) - 1) = eFormat;

				if( EcwFileRead(hEcwFile, p_packed + sizeof(UINT64), packed_length - 1) ) {
					error = TRUE;
					if(p_packed)
						NCSFree(p_packed);
					break;
				}
												//[13]
				p_unpacked = NULL;	// [02]
				error = unpack_data(&p_unpacked, p_packed + sizeof(UINT64) - 1, unpacked_length, 1);
				
				if( p_packed )
					NCSFree(p_packed);
				if( error )
					break;
	
#ifdef NCSBO_MSBFIRST
				NCSByteSwapRange64((UINT64*)p_unpacked, (UINT64 *)p_unpacked, unpacked_length / 8);
#endif
				p_top_qmf->bRawBlockTable = FALSE;
				p_top_qmf->p_block_offsets = (UINT64 *)p_unpacked;
			} else {
				UINT64 nPos = 0;
				p_top_qmf->bRawBlockTable = FALSE;
				p_top_qmf->p_block_offsets = (UINT64 *)NULL;

				EcwFileGetPos(hEcwFile, &nPos);
				error = EcwFileSetPos(hEcwFile, nPos + packed_length - 1);
			}
			break;			// we only want to run this while once
		}
		EcwFileGetPos(hEcwFile, &(p_top_qmf->file_offset));
	}

	// Now set up File Info structure
	// must set this up so it is always valid for higher levels to rely on
	p_top_qmf->pFileInfo = (ECWFileInfoEx *) NCSMalloc(sizeof(ECWFileInfoEx), FALSE);
	if( !p_top_qmf->pFileInfo ) {
		if( bEcwFileOpen )
			EcwFileClose(hEcwFile);
		delete_qmf_levels(p_top_qmf);
		return(NULL);
	}

	p_top_qmf->pFileInfo->nSizeX = p_top_qmf->p_file_qmf->x_size;
	p_top_qmf->pFileInfo->nSizeY = p_top_qmf->p_file_qmf->y_size;
	p_top_qmf->pFileInfo->nBands = p_top_qmf->p_file_qmf->nr_bands;

	// FIXME!! Add in this information
	p_top_qmf->pFileInfo->nCompressionRate = nCompressionRate;
	p_top_qmf->pFileInfo->eCellSizeUnits = eCellSizeUnits;
	
	p_top_qmf->pFileInfo->fCellIncrementX = fCellIncrementX;
	p_top_qmf->pFileInfo->fCellIncrementY = fCellIncrementY;
	p_top_qmf->pFileInfo->fOriginX		  = fOriginX;
	p_top_qmf->pFileInfo->fOriginY		  = fOriginY;
	p_top_qmf->pFileInfo->szDatum		  = NCSMalloc(ECW_MAX_DATUM_LEN, FALSE);
	if(!p_top_qmf->pFileInfo->szDatum) {
		return NULL;
	}
	strcpy(p_top_qmf->pFileInfo->szDatum,szDatum);
	p_top_qmf->pFileInfo->szProjection	  = NCSMalloc(ECW_MAX_PROJECTION_LEN, FALSE);
	if(!p_top_qmf->pFileInfo->szProjection) {
		return NULL;
	}
	strcpy(p_top_qmf->pFileInfo->szProjection,szProjection);

	p_top_qmf->pFileInfo->pBands = (NCSFileBandInfo*)NCSMalloc(sizeof(NCSFileBandInfo) * p_top_qmf->pFileInfo->nBands, TRUE);
	if(p_top_qmf->compress_format == COMPRESS_YUV) {
		p_top_qmf->pFileInfo->eColorSpace =	NCSCS_sRGB;
	} else {
		p_top_qmf->pFileInfo->eColorSpace = p_top_qmf->compress_format;
	}
	for(b = 0; b < p_top_qmf->pFileInfo->nBands;  b++) {
		p_top_qmf->pFileInfo->pBands[b].nBits = 8;
		p_top_qmf->pFileInfo->pBands[b].bSigned = FALSE;

		switch(p_top_qmf->compress_format) {
			case COMPRESS_UINT8:
					p_top_qmf->pFileInfo->pBands[b].szDesc = NCSStrDup(NCS_BANDDESC_Greyscale);
				break;
			case COMPRESS_MULTI:
				break;
			case COMPRESS_YUV:
					switch(b) {
						case 0:	
								p_top_qmf->pFileInfo->pBands[b].szDesc = NCSStrDup(NCS_BANDDESC_Red);
							break;
						case 1:	
								p_top_qmf->pFileInfo->pBands[b].szDesc = NCSStrDup(NCS_BANDDESC_Green);
							break;
						case 2:	
								p_top_qmf->pFileInfo->pBands[b].szDesc = NCSStrDup(NCS_BANDDESC_Blue);
							break;
					}
				break;
		}
		if(!p_top_qmf->pFileInfo->pBands[b].szDesc) {
			char buf[32];
			sprintf(buf, NCS_BANDDESC_Band, b);
			p_top_qmf->pFileInfo->pBands[b].szDesc = NCSStrDup(buf);
		}
	}
	p_top_qmf->pFileInfo->eCellType = NCSCT_UINT8;

	// Allocate decompression specific buffers, now we have read the offsets in
	if(allocate_qmf_buffers(p_top_qmf, FALSE) != NCS_SUCCESS) {
		error = 1;
	}

	if( error ) {
		if( bEcwFileOpen )
			EcwFileClose(hEcwFile);
		delete_qmf_levels(p_top_qmf);
		return(NULL);
	}

	p_top_qmf->hEcwFile = hEcwFile;
	p_top_qmf->bEcwFileOpen = bEcwFileOpen;		// [03] point to the file
	return(p_top_qmf);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_start_region().  Initiate reading a region from a compressed file.
**	The caller specifies the region to read, and the resolution to read it at.
**
**	Call with a pointer to the ERW file to read from, and details about the region
**	to read.
**
**	Returns a pointer to a region structure, or NULL if an error occurred.
**
**	After starting a region, do multiple calls to erw_decompress_read_region_line()
**	to read the lines, then a call to erw_decompress_end_region() to finish reading.
**
**	IMPLEMENTATION:
**	1) Finding the largest level to use
**		The recursive level logic needs a bit of explanation.  First, we find the level
**		we need to provide the level of detail (LOD) that they require. This is simply
**		the level that, for that level's width from that level's start_[x|y] to that
**		level's end_[x|y] is no less than half the requested number_[x|y]. It is no
**		less than half, because a given level generates 2x the level values as output
**		during pyramid reconstruction.
**
**	2) Defining the boundaries of the smaller levels.
**		Once we know the level to generate output for, we then have to work out the start
**		and end x|y values for each smaller level. This is not quite as simple as you might
**		think, as a single output value is generated from two smaller level values.
**		Which input values are used depends on the output value location, specifically
**		if it is odd or even (this is because we use a 3 tap reconstruction filter;
**		see collapse_pyr.c for details).
**		
**		For a given output location O[N], in either X or Y direction, the two input locations needed
**		are I[(N-1)/2] and I[(N-1)/2]+1.  For example, X value O[0] needs I[-1] and I[0],
**		X value O[1] needs I[0] and I[1], X value O[66] needs I[63] and I[62], O[101] needs I[50] and I[51]
**		so on.
**
**		In a two dimensional view, output O[x,y] therefore requires 4 input values:
**		(1)	I[(x-1)/2,(y-1)/2]		(2)	I[(x-1)/2+1,(y-1)/2]	
**		(3)	I[(x-1)/2,(y-1)/2+1]	(4)	I[(x-1)/2+1,(y-1)/2+1]	
**
**	3) There is a difference between finding the level to display, and then levels under that level
**		Note that we use simple N/2 logic to find the level large enough to display (#1) above
**		before using the more complex (#2) above to calculate required smaller level sizes.
**
**	4) The smallest level may be larger than the level of detail required
**		In the normal situation, the level required is subsamples (expanded in detail) to
**		provide the output view. However, it may be that the detail needed is smaller than
**		that provided by the smallest level, in which case supersampling is carried out
**		(more values are decompressed than are actually handed back to the calling routine)
**
**	5) Input value rolling
**		Two inputs are needed to generate two outputs, and input is rolled once for every
**		2nd output generated. This results in output being 2x larger than input, which
**		is what we need.  The input is rolled after even output [x,y] values. This is
**		because of the ((n-1)/2) and ((n-1)/2)+1 input requirement. For example, consider:
**		output X or Y		Needs these 2 inputs
**			10				4 5
**			11				5 6
**			12				5 6
**			13				6 7
**		So it can be seen that *after* even X or Y values we roll the inputs.
**		The initial values set up are such that the first two I values are valid for the first
**		O value. The first roll may happen straight after processing that O value (if O is
**		even), or might happen after processing two O values (if the first O is odd).
**
**	6) Reflection handling
**		The above ignores one final problem: Edges.  Consider a level that is 94 X wide
**		(or 94 Y down) in size. To generate the O[93] value (as this is relative to 0, this
**		is value # 94), we need input values I[46] and I[47].  But input size is only 47 for
**		this smaller level (size level L-1 is level L wize (n+1/2)).  So the I[47] value
**		does not exist.  In the same way, value I[-1] does not exist, but is needed to generate
**		O[0], which requires I[-1] and I[0].
**	
**		In both of these cases, we add in REFLECTION handling.  There are 4 possible reflection
**		values, reflect_start_x, reflect_end_x, reflect_start_y, reflect_end_y. These will be
**		0 (no reflection) or 1 (reflection required on that side).
**
**		It is important to note that the level_[start|end|size]_[x|y] values DO NOT include
**		the reflection counts. So true size of a level, including reflected values,
**		is always (for example):
**			1 + reflect_start_x + reflect_end_x + level_end_x - level_start_x;
**		This means that when a level is asked to return data, it is returned to line + reflect_start,
**		and reflection is patched up after the data is retrieved.
**		One final note of interest:  Consider a level that has output_start_x as follows. This
**		is how the reflection will work for this and smaller levels:
**		output_start_x		level_start_x		relect_start_x
**			3					1					0		# because output 3 needs values 1 and 2
**			1					0					0		# because output 1 needs values 0 and 1
**			0					0					1		# because output 0 needs values 0 and 0
**		So the important point here is that reflection is based on the current level value, AND on
**		the output value required.
**
**	7) True size versus reflected size, and buffer offsets
**		Reflection means we have two start/end/size values for X and Y - with reflection, and
**		without reflection. The _reading_ of line0 and line1 needs to work with all data,
**		including any reflection.  The _writing_ to line0 and line1 needs to be excluding
**		reflection, which is added in after the data is written.  Thus, the values
**		level_[start|end|size]_[x|y] are ALWAYS EXCLUDING the reflect_[start|end]_[x|y] values,
**		so include those values when needed. However, the pointer to the start of line0 and line1
**		always INCLUDE the reflect_[start|end]_[x|y] values. What this means in english:
**		-	To allocate these buffers, use level_size_x + reflect_start_x + reflect_end_x
**		-	To read from the buffers, use line0[0] etc as the first value (which will be the first reflection
**			value if it exists).
**		-	To write to the buffers, use line0 + reflect_start_x as the first location to write to,
**			and only write level_size_x values (which will exclude the reflection).
**	   Just to be safe, buffers are always allocated as level_size_x + 2 just in case.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

QmfRegionStruct *erw_decompress_start_region( QmfLevelStruct *p_top_qmf,
				 UINT32 nr_bands_requested,		// number of bands to read
				 UINT32 *band_list,				// index into actual band numbers from source file
				 UINT32 start_x, UINT32 start_y,
				 UINT32 end_x, UINT32 end_y,
				 UINT32 number_x, UINT32 number_y)
{
	UINT32	x_size, y_size;
	QmfRegionLevelStruct	*p_level;
	QmfRegionStruct			*p_region;
	QmfLevelStruct			*p_qmf;
	UINT32	output_level_start_y, output_level_end_y, output_level_size_y;
	UINT32	output_level_start_x, output_level_end_x, output_level_size_x;
	UINT32		band;
	Sideband	sideband;

	if(!p_top_qmf) {
//		ERS_setup_error(ERS_RASTER_ERROR,
//					"\nerw_decompress_start_region: was given a NULL compressed file structure pointer");
		return(NULL);
	}

	x_size = p_top_qmf->p_file_qmf->x_size;
	y_size = p_top_qmf->p_file_qmf->y_size;

	// Range check region. Must be inside image, and number to extract can not be
	// larger than start/end region size. So if you want to read from [2,3] to [10,15],
	// then number_x must be <= (1 + 10 - 2) and number_y must be <= [1 + 15 - 3]
	if( start_x > end_x || start_y > end_y
	 || end_x >= x_size || end_y >= y_size
	 || number_x > (1 + end_x - start_x)
	 || number_y > (1 + end_y - start_y)
	 || number_x == 0 || number_y == 0) {
//		ERS_setup_error(ERS_RASTER_ERROR,
//					"\nerw_decompress_start_region: invalid region size or extents were selected");
		return(NULL);
	}
	// Sanity check band list
	if( nr_bands_requested > p_top_qmf->nr_bands ) {
//		ERS_setup_error(ERS_RASTER_ERROR,
//					"\nerw_decompress_start_region: More bands were requested than exist in the file");
		return(NULL);
	}
	for( band = 0; band < nr_bands_requested; band++ ) {
		if( band_list[band] >= p_top_qmf->nr_bands ) {
//			ERS_setup_error(ERS_RASTER_ERROR,
//					"\nerw_decompress_start_region: an illegal range of bands was selected to read");
			return(NULL);
		}
	}

	/*
	**	Compute output size, and find the largest level that contains the needed level of detail
	*/

	// work out level we need to go down to for data. In other words, we only
	// extract enough data to cover the number_x and number_y size requested.
	// It is possible that they asked for more detail in one dimension than in
	// another dimension (perhaps because of odd aspect pixels), so we check
	// both width and height to decide which level to use

	// these decide the edges of the level to use
	output_level_start_x = start_x; output_level_end_x = end_x;
	output_level_start_y = start_y; output_level_end_y = end_y;
	output_level_size_x  = 1 + output_level_end_x - output_level_start_x;		// start_y and end_y are stated in file units
	output_level_size_y  = 1 + output_level_end_y - output_level_start_y;		// so are x values
	// start searching at the level below the file level	
	p_qmf = p_top_qmf->p_file_qmf->p_smaller_qmf;
	// search for the smallest level that covers the number_[x|y] requested
	// Note that the level_height/width are the output size generated from
	// this level (e.g. 2 x this level size)
	while((output_level_size_y > number_y * 2) && (output_level_size_x > number_x * 2) ) {
		if( !p_qmf->p_smaller_qmf )
			break;	// force usage of the top level
		// scale down to the next level. We always treat odd numbers as scaling down to smaller
		output_level_start_x /= 2;
		output_level_start_y /= 2;
		output_level_end_x = output_level_end_x / 2;
		output_level_end_y = output_level_end_y / 2;
		// These must be recalculated not scaled, as the width/height might go to zero
		output_level_size_x  = 1 + output_level_end_x - output_level_start_x;		// start_y and end_y are stated in file units
		output_level_size_y  = 1 + output_level_end_y - output_level_start_y;		// so are x values
		p_qmf = p_qmf->p_smaller_qmf;
	}

	// We have found the level. However (particularly for the top level), we
	// may we have to super sample
	// the level, extracting just the values they want, while processing the entire
	// level region at the region resolution. We can make an exception where they
	// only want one pixel as we know there will be only one supersample operation,
	// so we just grab the first pixel. Where this pixel is gathered from is
	// debatable.  We pick the center pixel. The result is a big performance improvement.
	// This means single point point profiling, of course, will be much faster.
	if( number_x == 1 ) {
		output_level_start_x += ((output_level_end_x - output_level_start_x)/2);
		output_level_end_x = output_level_start_x;
		output_level_size_x = 1;
	}
	if( number_y == 1 ) {
		output_level_start_y += ((output_level_end_y - output_level_start_y)/2);
		output_level_end_y = output_level_start_y;
		output_level_size_y = 1;
	}


	// Set up the region info and allocate the structure pointers
	p_region = (QmfRegionStruct *) NCSMalloc(sizeof(QmfRegionStruct), FALSE);
	if( !p_region )
		return(NULL);

//	p_region->random_value = 1;			// [11] start seed with initial random value
	p_region->random_value = 0xd4c5c239 ^ (UINT32)((output_level_start_x + output_level_start_y * p_qmf->x_size)/* * NCSGetTimeStampMs()*/);//[14]
	p_region->p_top_qmf = p_top_qmf;
	p_region->p_largest_qmf = p_qmf;	// largest level we have to go to
	p_region->start_x = start_x;
	p_region->start_y = start_y;
	p_region->end_x = end_x;
	p_region->end_y = end_y;
	p_region->number_x = number_x;
	p_region->number_y = number_y;
	p_region->read_line = TRUE;			/* have to read the first line */
	p_region->p_p_ll_line = NULL;
	p_region->p_ll_buffer = NULL;
	// Later on, this will enable us to read a sub-set of the bands in the file
	// for now, we MUST read all bands, and extract the subset desired
	// FIXME! be more intelligent in reading bands.  Only YIQ mode (from RGB) needs
	// to read all bands regardless of the actual bands requested. Other band
	// combinations should be able to be read based on just those bands requested.
	// pack.c code will need to be checked - there is some code there that might have
	// to be updated to extract only those bands requested.  For now, read all bands,
	// then pass back only those requested
	p_region->used_bands = p_qmf->p_file_qmf->nr_bands;
	p_region->nr_bands_requested = nr_bands_requested;
	p_region->band_list = band_list;	// caller must alloc the band list, and free it AFTER the region close

	/*
	** [08] If texture noise is acceptable, and if compression rate high enough to use it,
	**		and if not MULTI mode (e.g. only greyscale or YUV), then turn on texture noise
	** [10] Only add texture noise if one of the larger QMF levels
	*/
	if( pNCSEcwInfo->bNoTextureDither == FALSE ) {
		// Only turn on texture noise if compression rate was high enough to warrant it
		if( ((p_top_qmf->compress_format == COMPRESS_UINT8 && p_top_qmf->pFileInfo->nCompressionRate > 4)
 		    || (p_top_qmf->compress_format == COMPRESS_YUV && p_top_qmf->pFileInfo->nCompressionRate > 9))
		 && ((p_region->p_largest_qmf->level > 2) || (p_top_qmf->nr_levels < 3)) /* [10] */ ) {
			p_region->bAddTextureNoise = TRUE;
			// [11] no longer using srand
		}
		else		
			p_region->bAddTextureNoise = FALSE;
	}
	else
		p_region->bAddTextureNoise = FALSE;

	// now set up increment information
	p_region->start_line = (IEEE4) output_level_start_y;	/**[06]**/
	p_region->current_line = (IEEE4) output_level_start_y;
	p_region->increment_y = (IEEE4) output_level_size_y / (IEEE4) number_y;
	p_region->increment_x = (IEEE4) output_level_size_x / (IEEE4) number_x;
	p_region->nCounter = 0;

	p_region->p_p_ll_line = (IEEE4 **)
		NCSMalloc(sizeof(IEEE4 *) * p_region->used_bands, FALSE);	// Ptr to bands for line of output
	p_region->p_ll_buffer = (IEEE4 *)
		NCSMalloc(sizeof(IEEE4) * output_level_size_x * p_region->used_bands, FALSE);	// final output line, created by generated qmf level
	// now set up region level buffer structures
	p_region->p_levels = (QmfRegionLevelStruct *)
		NCSMalloc(sizeof(QmfRegionLevelStruct) * (p_qmf->level+1) , FALSE);

	if( !(p_region->p_p_ll_line) || !(p_region->p_ll_buffer) || !p_region->p_levels ) {
		if( p_region->p_p_ll_line ) 
			NCSFree(p_region->p_p_ll_line);
		if( p_region->p_ll_buffer )
			NCSFree(p_region->p_ll_buffer);
		if( p_region->p_levels )
			NCSFree(p_region->p_levels);
		if( p_region )
			NCSFree(p_region);
//		ERS_setup_error(ERS_RASTER_ERROR,
//					"\nerw_decompress_start_region: unable to allocate level buffers");
		return(NULL);
	}
	// set up pointers to the output buffer
	for( band = 0; band < p_qmf->p_file_qmf->nr_bands; band++ )
		p_region->p_p_ll_line[band] = p_region->p_ll_buffer + (band * output_level_size_x);

	// set buffer pointers to NULL in case we need to free later; need to know what has been allocated so far
	p_qmf = p_region->p_largest_qmf;
	while(p_qmf) {
		p_region->p_levels[p_qmf->level].buffer_ptr = NULL;
		p_region->p_levels[p_qmf->level].p_x_blocks = NULL;
		p_region->p_levels[p_qmf->level].have_blocks = FALSE;
		p_region->p_levels[p_qmf->level].used_bands = p_region->used_bands;

		p_region->p_levels[p_qmf->level].p_p_line0 = NULL;
		p_region->p_levels[p_qmf->level].p_p_line1 = NULL;
		p_qmf = p_qmf->p_smaller_qmf;
	}

	p_region->pNCSFileView = NULL;	// Back pointer to the NCS FileView, set up by the NCScbmSetFileView

	// NOTE: From now on, we can use use the close_region call during error handling,
	// as the region has been set up well enough to be handled by that call


	// Define and allocate level values and memory for each level.
	// We run two sets of values:
	// output_level_???? and level_????.  The output values are
	// the start/end/size for the output from this level, and
	// the level values are the start/end/size for this level data

	p_qmf = p_region->p_largest_qmf;
	while(p_qmf) {
		UINT32	level_start_y, level_end_y, level_size_y;
		UINT32	level_start_x, level_end_x, level_size_x;
		UINT32	reflect_start_x, reflect_start_y, reflect_end_x, reflect_end_y;
		UINT32	start_x_block;
		UINT32	last_x_block;

		p_level = &(p_region->p_levels[p_qmf->level]);
		p_level->p_region	= p_region;
		p_level->p_qmf		= p_qmf;
		// set up level information from previous level's output information
		// Must handle reflection where the start or end might be at zero.
		// We only check start reflection based on start_[x|y], as if end_[x|y] is zero
		// and needs left/top reflection, then so will start values be zero. Same logic
		// in reverse for the end - sorting end values out must sort out high start values
		// also.
		if( output_level_start_x ) {
			level_start_x	= (output_level_start_x - 1)/2;		// (N-1)/2
			reflect_start_x = 0;
		}
		else {
			level_start_x = 0;
			reflect_start_x = 1;
		}
		if( output_level_end_x < (p_qmf->p_larger_qmf->x_size-1) ) {
			if( output_level_end_x )
				level_end_x	= (output_level_end_x   - 1)/2+1;	// (N-1)/2+1
			else
				level_end_x = 0;
			reflect_end_x = 0;
		}
		else {
			level_end_x = p_qmf->x_size - 1;
			reflect_end_x = 1;
		}

		if( output_level_start_y ) {
			level_start_y	= (output_level_start_y - 1)/2;		// (N-1)/2
			reflect_start_y = 0;
		}
		else {
			level_start_y = 0;
			reflect_start_y = 1;
		}
		if( output_level_end_y < (p_qmf->p_larger_qmf->y_size-1) ) {
			if( output_level_end_y )
				level_end_y	= (output_level_end_y - 1)/2+1;	// (N-1)/2+1
			else
				level_end_y = 0;
			reflect_end_y = 0;
		}
		else {
			level_end_y = p_qmf->y_size - 1;
			reflect_end_y = 1;
		}

		// these values EXCLUDE the reflection size
		level_size_x	= 1 + level_end_x - level_start_x;
		level_size_y	= 1 + level_end_y - level_start_y;

		// set up values start / end X values etc.

		p_level->reflect_start_x	= (UINT8)reflect_start_x;
		p_level->reflect_end_x		= (UINT8)reflect_end_x;
		p_level->reflect_start_y	= (UINT8)reflect_start_y;
		p_level->reflect_end_y		= (UINT8)reflect_end_y;

		// track level information EXCLUDING reflections
		p_level->level_start_x	= level_start_x;
		p_level->level_end_x	= level_end_x;
		p_level->level_size_x	= level_size_x;
		p_level->level_start_y	= level_start_y;
		p_level->level_end_y	= level_end_y;
		p_level->level_size_y	= level_size_y;

		p_level->output_level_start_x	= output_level_start_x;
		p_level->output_level_end_x		= output_level_end_x;
		p_level->output_level_size_x	= output_level_size_x;
		p_level->output_level_start_y	= output_level_start_y;
		p_level->output_level_end_y		= output_level_end_y;
		p_level->output_level_size_y	= output_level_size_y;
		if( p_level->p_qmf->x_size <= level_end_x
		 || p_level->p_qmf->y_size <= level_end_y ) {

//			ERS_setup_error(ERS_RASTER_ERROR,"\nerw_decompress_start_Region: level size is smaller than expected");
			erw_decompress_end_region(p_region);
			return(NULL);
		}			

		// Work out starting and ending block number in the X direction set of blocks, then
		// set up enough pointers to X blocks as needed for a line in this level
		start_x_block	= level_start_x / p_level->p_qmf->x_block_size;
		last_x_block	= level_end_x   / p_level->p_qmf->x_block_size;
		p_level->start_x_block = start_x_block;
		p_level->x_block_count = 1 + last_x_block - start_x_block;

		// set up the pre and post block skip calculations for later quick access
		// This is because the desired X start might be larger than the start of the block,
		// and ditto for the end number.
		p_level->first_block_skip = level_start_x - (start_x_block * p_level->p_qmf->x_block_size);
		// last X block might be smaller than the multiple of x_block_size * (last_x_block+1)
		{
			UINT32	last_pixel;
			last_pixel = (last_x_block+1) * p_level->p_qmf->x_block_size - 1;
			if( last_pixel >= p_level->p_qmf->x_size )
				last_pixel = p_level->p_qmf->x_size - 1;
			p_level->last_block_skip = last_pixel - level_end_x;
		}
		// now go and init the line decompression blocks
		if( unpack_init_lines(p_level) ) {
			erw_decompress_end_region(p_region);
			return(NULL);
		}

		// set up multi line ring buffer
		p_level->start_line = level_start_y;		/**[06]**/
		p_level->current_line = level_start_y;
		p_level->start_read_lines = 2 - reflect_start_y;	/**[06]**/
		p_level->read_lines = 2 - reflect_start_y;	// number of lines to read at first, ignoring reflection
		// allocate the line buffers. There are 2 for each sideband, line0 and line1,
		// and this is for each band in the file.
		// NOTE WELL: The buffer for each line must handle -1 and +1 reflection (for edges)
		// or for data gathering outside the area of interest, so these lines are allocated
		// with TWO more values than needed for the level.
		p_level->buffer_ptr =
			(IEEE4 *) NCSMalloc(sizeof(IEEE4) * p_qmf->nr_bands * (level_size_x+2) * 2 * MAX_SIDEBAND, TRUE);	// add 2 to line lengths **[06]** - changed to calloc()
		if( !p_level->buffer_ptr ) {
			//ERS_setup_error(ERS_RASTER_ERROR,"\nerw_decompress_start_region: out of memory while allocating level buffers");
			erw_decompress_end_region(p_region);
			return(NULL);
		}
		// point into the buffers. We keep line types together, e.g. line0[LL] and line1[LL]
		// This improves CPU caching of lines, resulting in faster execution.
		p_level->p_p_line0 = (IEEE4 **) NCSMalloc( sizeof(IEEE4 *) * p_level->used_bands * MAX_SIDEBAND, FALSE);
		p_level->p_p_line1 = (IEEE4 **) NCSMalloc( sizeof(IEEE4 *) * p_level->used_bands * MAX_SIDEBAND, FALSE);
		p_level->p_p_line1_ll_sideband = (IEEE4 **) NCSMalloc( sizeof(IEEE4 *) * p_level->used_bands, FALSE);
		if( !p_level->p_p_line0 || !p_level->p_p_line1 || !p_level->p_p_line1_ll_sideband) {
			//ERS_setup_error(ERS_RASTER_ERROR,"\nerw_decompress_start_region: out of memory while allocating level band buffers");
			erw_decompress_end_region(p_region);
		}
		for( band = 0; band < p_level->used_bands; band++ ) {
			for( sideband = LL_SIDEBAND; sideband < MAX_SIDEBAND; sideband++ ) {
				p_level->p_p_line0[DECOMP_INDEX] = DECOMP_LEVEL_LINE01(p_level, band, sideband, 0);
				p_level->p_p_line1[DECOMP_INDEX] = DECOMP_LEVEL_LINE01(p_level, band, sideband, 1);
			}
			p_level->p_p_line1_ll_sideband[band] = p_level->p_p_line1[band * MAX_SIDEBAND + LL_SIDEBAND] +
												   p_level->reflect_start_x;
		}
		// and set up output values for the next smaller level
		output_level_start_x = level_start_x;	output_level_end_x = level_end_x;
		output_level_start_y = level_start_y;	output_level_end_y = level_end_y;
		output_level_size_x  = level_size_x;
		output_level_size_y	 = level_size_y;

		p_qmf = p_qmf->p_smaller_qmf;
	}

	return(p_region);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_end_region().  Finished with and deallocate
**	the region structure. Can be called at the true end,
**	or if reading is to be aborted.
**
**	No errors are returned.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

void erw_decompress_end_region( QmfRegionStruct *p_region )
{
	int level;
	QmfRegionLevelStruct	*p_level;
	
	if( !p_region )
		return;
	level = p_region->p_largest_qmf->level;
	while(level >= 0) {
		p_level = &p_region->p_levels[level];
		unpack_free_lines(p_level);		// handles freeing anything under the x_blocks pointer
		NCS_SAFE_FREE((char *) p_region->p_levels[level].buffer_ptr);
		NCS_SAFE_FREE((char *) p_level->p_p_line0);
		NCS_SAFE_FREE((char *) p_level->p_p_line1);
		NCS_SAFE_FREE((char *) p_level->p_p_line1_ll_sideband);
		p_level->p_p_line1 = NULL;
		--level;
	}

	NCS_SAFE_FREE((char *) p_region->p_p_ll_line);
	NCS_SAFE_FREE((char *) p_region->p_ll_buffer);
	NCS_SAFE_FREE((char *) p_region->p_levels);
	NCS_SAFE_FREE((char *) p_region);

}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_close(). Closes a ERW file.
**
**	No errors are returned.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

void erw_decompress_close( QmfLevelStruct *p_top_qmf )
{
	delete_qmf_levels(p_top_qmf);
}
