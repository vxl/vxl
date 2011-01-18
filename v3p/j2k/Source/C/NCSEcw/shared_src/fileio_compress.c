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
** FILE:   	fileio_compress.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Compress File IO routines
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns	20mar99	Updated to NCS capabilities
** [03] sns 07Apr99 Correcting RGB to YUV conversion (B value was incorrect)
** [04] sjc 29Apr99 Fixed memory corruption with fspec handling.
** [05] sjc 29Apr99 Changed to calloc() to clear up UMR in ep_dshdr().
** [06] sns 20May99 For greyscale, single band, doing faster version of read.
** [07] md  14Jun99 band order fix when saving rgb alg to multi
** [08] sjc 04Dec00 Write out block table unpacked if pack fails
** [09] rar 24-Jan-01 Mac port changes
 *******************************************************/

#include "ECW.h" 
#include "NCSEcw.h"

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Compress File IO routines.
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**	writes the preamble header in the compressed ERW file,
**	then update the lengths for each block to be an offset
**
**
**	Make sure the header file format documented in erswave.h is
**	kept in synch with any changes here.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

NCSError write_compressed_preamble(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile)
{
	UINT8			num_levels;
	QmfLevelStruct	*p_qmf;
	UINT32			band;
//#ifdef ECW_USE_COMPRESSED_BLOCK_TABLE
//	UINT8			*p_packed;
//#endif // ECW_USE_COMPRESSED_BLOCK_TABLE
//	UINT32			packed_length;
//UINT32			unpacked_length;
//	UINT64			*p_offsets;//, current_offset;
	NCSError		eError = NCS_SUCCESS;

	num_levels = 0;
	p_qmf = p_top_qmf;
	while(p_qmf->p_larger_qmf) {
		num_levels++;
		p_qmf = p_qmf->p_larger_qmf;
	}
	p_top_qmf->outfile = outfile;

	// p_qmf is now pointing to fake file QMF level
	/*
	**	Write the header
	*/
	if((eError = NCSFileWriteUINT8_MSB(outfile, ECW_HEADER_ID_TAG)) != NCS_SUCCESS)						// UINT8 ECW_HEADER_ID_TAG
		return(eError);
	if((eError = NCSFileWriteUINT8_MSB(outfile, p_top_qmf->version)) != NCS_SUCCESS) 						// UINT8 ERSWAVE_VERSION
		return(eError);
	if((eError = NCSFileWriteUINT8_MSB(outfile,(UINT8) p_top_qmf->blocking_format)) != NCS_SUCCESS)		// UINT8 BLOCKING_LEVEL
		return(eError);
	if((eError = NCSFileWriteUINT8_MSB(outfile,(UINT8) p_top_qmf->compress_format)) != NCS_SUCCESS)		// UINT8 COMPRESS_[UINT8|YIQ]
		return(eError);
	if((eError = NCSFileWriteUINT8_MSB(outfile,(UINT8) num_levels)) != NCS_SUCCESS)						// UINT8 Number of levels
		return(eError);
	if((eError = NCSFileWriteUINT8_MSB(outfile,(UINT8) p_top_qmf->nr_sidebands)) != NCS_SUCCESS)		// UINT8 Number of sidebands
		return(eError);
	if((eError = NCSFileWriteUINT32_MSB(outfile,p_qmf->x_size)) != NCS_SUCCESS)							// UINT32 X size
		return(eError);
	if((eError = NCSFileWriteUINT32_MSB(outfile,p_qmf->y_size)) != NCS_SUCCESS)							// UINT32 Y size
		return(eError);
	if((eError = NCSFileWriteUINT16_MSB(outfile,p_top_qmf->nr_bands)) != NCS_SUCCESS)					// UINT16 Number of bands
		return(eError);
	if((eError = NCSFileWriteUINT16_MSB(outfile,1)) != NCS_SUCCESS)										// UINT16 scale factor (no longer used)
		return(eError);
	if((eError = NCSFileWriteUINT16_MSB(outfile,p_top_qmf->x_block_size)) != NCS_SUCCESS)				// UINT16 X block size
		return(eError);
	if((eError = NCSFileWriteUINT16_MSB(outfile,p_top_qmf->y_block_size)) != NCS_SUCCESS)				// UINT16 Y block size
		return(eError);

	// [02] Write version 2.0 information
	{
		ECWFileInfo	*pFileInfo = (ECWFileInfo*)p_top_qmf->pFileInfo;
		UINT8 szBuf[ECW_MAX_DATUM_LEN];

		if((eError = NCSFileWriteUINT16_MSB(outfile, (UINT16) pFileInfo->nCompressionRate)) != NCS_SUCCESS)	// UINT16 Compression Factor
			return(eError);
		if((eError = NCSFileWriteUINT8_MSB(outfile, (UINT8) pFileInfo->eCellSizeUnits)) != NCS_SUCCESS)				// UINT8  CellSizeUnits
			return(eError);
		if((eError = NCSFileWriteIEEE8_LSB(outfile, pFileInfo->fCellIncrementX)) != NCS_SUCCESS)	// IEEE8 fCellIncrementX
			return(eError);
		if((eError = NCSFileWriteIEEE8_LSB(outfile, pFileInfo->fCellIncrementY)) != NCS_SUCCESS)	// IEEE8 fCellIncrementY
			return(eError);
		if((eError = NCSFileWriteIEEE8_LSB(outfile, pFileInfo->fOriginX)) != NCS_SUCCESS)			// IEEE8 fOriginX
			return(eError);
		if((eError = NCSFileWriteIEEE8_LSB(outfile, pFileInfo->fOriginY)) != NCS_SUCCESS)			// IEEE8 fOriginY
			return(eError);
		memset(szBuf, 0, sizeof(szBuf));
		strncpy((char*)szBuf, pFileInfo->szDatum, ECW_MAX_DATUM_LEN);
		if((eError = NCSFileWrite(outfile, szBuf, ECW_MAX_DATUM_LEN, NULL)) != NCS_SUCCESS)
			return(eError);
		memset(szBuf, 0, sizeof(szBuf));
		strncpy((char*)szBuf, pFileInfo->szProjection, ECW_MAX_DATUM_LEN);
		if((eError = NCSFileWrite(outfile, szBuf, ECW_MAX_PROJECTION_LEN, NULL)) != NCS_SUCCESS )
			return(eError);
	}

	// Now write the level information for each level.
	// We write this at the front, not with each level, so we can
	// read it quickly during file open.
	p_qmf = p_top_qmf;
	while(p_qmf->p_larger_qmf) {	// we don't write the fake file QMF level
		// write level number and size
		if((eError = NCSFileWriteUINT8_MSB(outfile, (UINT8) p_qmf->level)) != NCS_SUCCESS)	// level UINT8	level number
			return(eError);
		if((eError = NCSFileWriteUINT32_MSB(outfile, p_qmf->x_size)) != NCS_SUCCESS)						// level UINT32 x_size
			return(eError);
		if((eError = NCSFileWriteUINT32_MSB(outfile, p_qmf->y_size)) != NCS_SUCCESS)						// level UINT32 y_size
			return(eError);
		// write bin sizes for each sideband image
		for( band = 0; band < p_qmf->nr_bands; band++ ) {
				 if((eError = NCSFileWriteUINT32_MSB(outfile, p_qmf->p_band_bin_size[band])) != NCS_SUCCESS) //level IEEE4	binsize
					 return(eError);
		}
		p_qmf = p_qmf->p_larger_qmf;
	}
	if((eError = NCSFileWriteUINT32_MSB(outfile, get_qmf_tree_nr_blocks(p_top_qmf) * sizeof(UINT64) + 1)) != NCS_SUCCESS) {						// level UINT32 packed blocks length
		return(eError);
	}
	if((eError = NCSFileWriteUINT8_MSB(outfile, ENCODE_RAW)) != NCS_SUCCESS) {
		return(eError);
	}
		// Store location of first block offset in table
	p_top_qmf->file_offset = NCSFileTellNative(outfile);
#ifdef NOTDEF
	// Before writing the block offsets, we have to convert from block lengths
	// to block sizes. We can just run through the list to work this out. Note
	// there is one more block in the array than actually exists; this records
	// the relative end of the file. We can't write the true offset from the
	// start of the file, because we don't know that yet (until after this
	// comprssed array is written). So we don't bother, and let the reader
	// observe the first block location after reading header information.
	unpacked_length = get_qmf_tree_nr_blocks(p_top_qmf);	// includes +1 block
	p_offsets = p_top_qmf->p_block_offsets;
	p_offsets[unpacked_length - 1] = 0;
	current_offset = 0;										// relative to start of blocks, not start of file
	while( unpacked_length-- ) {
		UINT64 length;
		length = *p_offsets;
		*p_offsets++ = current_offset;
		current_offset += length;
	}

	// now write the block offsets for all blocks, all levels.
	// These are ordered in recursive order as per compression
	// These are output as a compressed set, as they have to be 64 bit values,
	// so will have a lot of zeros, and can compress well
	unpacked_length = get_qmf_tree_nr_blocks(p_top_qmf);
#if defined(NCSBO_MSBFIRST)
	NCSByteSwapRange64(p_top_qmf->p_block_offsets, 
					   p_top_qmf->p_block_offsets, 
					   unpacked_length);
#endif

#ifdef ECW_USE_COMPRESSED_BLOCK_TABLE
	eError = pack_data(p_top_qmf,
						&p_packed, &packed_length,
						(UINT8 *) p_top_qmf->p_block_offsets,
						unpacked_length * sizeof(p_top_qmf->p_block_offsets[0]), 
						8);
	if( eError == NCS_SUCCESS ) {
		//
		// Packed OK, write out block table.
		// 
		if( (eError = NCSFileWriteUINT32_MSB(outfile, packed_length)) != NCS_SUCCESS) {						// level UINT32 packed blocks length
			NCS_SAFE_FREE(p_packed);
			return(eError);
		}
		if( (eError = NCSFileWrite(outfile, p_packed, packed_length, NULL)) != NCS_SUCCESS) {
			NCS_SAFE_FREE(p_packed);
			return(eError);
		}

		NCS_SAFE_FREE(p_packed);
	} else {
#endif // ECW_USE_COMPRESSED_BLOCK_TABLE
		//
		// [08] Pack failed for some reason, write unpacked block table out instead.
		// On average will make only %0.15 difference anyway, since
		// block table usually only compresses ~%30.
		// Usually fails because after a large compression we can't alloc enough
		// contiguous memory for the packed table.
		//
		packed_length = unpacked_length * sizeof(p_top_qmf->p_block_offsets[0]);
		
		if( write_int32(packed_length + 1, p_outfile) ) {						// level UINT32 packed blocks length
			return(NCS_FILEIO_ERROR);
		}
		if( write_int8(ENCODE_RAW, p_outfile) ) {
			return(NCS_FILEIO_ERROR);
		}
		if( fwrite(p_top_qmf->p_block_offsets, 1, packed_length, p_outfile) != packed_length) {
			return(NCS_FILEIO_ERROR);
		}
		eError = NCS_SUCCESS;
#ifdef ECW_USE_COMPRESSED_BLOCK_TABLE
	}
#endif // ECW_USE_COMPRESSED_BLOCK_TABLE
#endif
	return(eError);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**	convert the block table to LSB offsets
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

NCSError convert_block_table(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile)
{
	UINT32			unpacked_length;
	UINT64			current_offset = 0;
	NCSError		eError = NCS_SUCCESS;
	UINT32 i;

	unpacked_length = get_qmf_tree_nr_blocks(p_top_qmf);	// includes +1 block
	
	for(i = 0; i < unpacked_length - 1; i++) {
		UINT64 length;
		UINT64 offset = current_offset;
		if(NCSFileSeekNative(outfile, p_top_qmf->file_offset + i * sizeof(UINT64), NCS_FILE_SEEK_START) != (INT64)(p_top_qmf->file_offset + i * sizeof(UINT64))) {
			eError = NCS_FILE_SEEK_ERROR;
			break;
		}
		eError = NCSFileRead(outfile, &length, sizeof(UINT64), NULL);
		if(eError != NCS_SUCCESS) {
			break;
		}
		if(NCSFileSeekNative(outfile, p_top_qmf->file_offset + i * sizeof(UINT64), NCS_FILE_SEEK_START) != (INT64)(p_top_qmf->file_offset + i * sizeof(UINT64))) {
			eError = NCS_FILE_SEEK_ERROR;
			break;
		}
#ifdef NCSBO_MSBFIRST
		NCSByteSwapRange64(&offset, 
						   &offset, 
						   1);
#endif
		eError = NCSFileWrite(outfile, &offset, sizeof(UINT64), NULL);
		if(eError != NCS_SUCCESS) {
			break;
		}
		current_offset += length;
	}
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange64(&current_offset, 
					   &current_offset, 
					   1);
#endif
	eError = NCSFileWrite(outfile, &current_offset, sizeof(UINT64), NULL);
	return(eError);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**	writes the level to the compressed ERW file
**
**	It does this by simply copying the data in the temporary level
**	files over to the main file. Each temporary file is deleted once copied,
**	so we only need extra space for one single level more than compressed
**	image size.
**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

NCSError write_compressed_level(QmfLevelStruct *p_qmf, NCS_FILE_HANDLE outfile)
{
	UINT8		*copy_buffer;
	
#ifdef LINUX
	UINT64		offset;
#else
	fpos_t		offset;
#endif
	
	NCSError	eError = NCS_SUCCESS;
	UINT64		file_size;
	UINT64		copy_length;

	offset = 0;
	
	/*
	**	Copy each of the sidebands over. We need some scratch RAM
	**	while doing this.
	**	For the smallest level, we also write the LL sideband
	*/

	copy_buffer = (UINT8 *) NCSMalloc(MAX_FILE_COPY_MEMORY, FALSE);
	if( !copy_buffer )
		return(NCS_COULDNT_ALLOC_MEMORY);

//	fflush(p_qmf->tmp_file);
	// work out how much was output
	file_size = NCSFileTellNative(p_qmf->tmp_file);
	
	// go back to the start of the file
	if( NCSFileSeekNative(p_qmf->tmp_file, offset, NCS_FILE_SEEK_START) == offset) {
		while(file_size) {
			if( file_size <= MAX_FILE_COPY_MEMORY )
				copy_length = file_size;
			else
				copy_length = MAX_FILE_COPY_MEMORY;

			if( (eError = NCSFileRead(p_qmf->tmp_file, copy_buffer, (UINT32)copy_length, NULL)) != NCS_SUCCESS) {
				break;
			}
			eError = NCSFileWrite(outfile, copy_buffer, (INT32)copy_length, NULL);
			if( eError != NCS_SUCCESS ) {
				break;
			}
			file_size -= copy_length;
		}
	}
	// clean up temporary file
	NCSFileClose(p_qmf->tmp_file);
	
	if( p_qmf->tmp_fname )
		NCSDeleteFile( p_qmf->tmp_fname );

	NCS_SAFE_FREE((char *) p_qmf->tmp_fname);
	p_qmf->tmp_file = NCS_NULL_FILE_HANDLE;
	p_qmf->tmp_fname = NULL;

	NCS_SAFE_FREE(copy_buffer);
	return(eError);
}



void free_qmf_block(QmfLevelStruct *p_qmf, UINT8 *p_block)
{
	p_qmf;//Keep compiler happy
	NCS_SAFE_FREE((char *) p_block);
}
