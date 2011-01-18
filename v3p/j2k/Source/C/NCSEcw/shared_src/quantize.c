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
** FILE:   	quantize.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Quantize and encode sub-band images in pyramid
**			(or decode / dequantize when decompressing)
**
**
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns 02Apr99	Updated to use fast floating point conversions on x86 platforms
** [03] sns 20May99 Performance tuning
** [04] rar 24-Jan-01 Mac port changes
** [05] sjc 30Apr03	Added low-memory compression
 *******************************************************/

#include "ECW.h"
#include "NCSThread.h"

#if (defined(DUMP_MEM_STATS)&&defined(_DEBUG))
#include <crtdbg.h>
#endif

#define NCS_NR_BLOCKS 64

//static NCSError qencode_qmf_sideband(QmfLevelStruct *p_qmf, Sideband sideband, INT16 *p_line);
static NCSError qencode_qmf_level_output_block(QmfLevelStruct *p_qmf );

//static NCSError unquantize_qmf_sideband(QmfLevelStruct *p_top_qmf, QmfLevelStruct *p_qmf,
//									BinIndexType *q_im, int *res,
//									Sideband sideband);

/*
**	Quantizes a single line from the specified QMF level and adds it to the quantized output buffer.
**	When the buffer is full, calls qencode_qmf_level_output_block() to output the block to disk.
**	Returns non-zero if there was an error.
**	If level 0, we compress the LL sideband as well, otherwise we just compress the
**	LH, HL and HH sideband images.
*/

NCSError build_qmf_level_qencode_line(QmfLevelStruct *p_qmf, UINT32 y, IEEE4 **p_p_ll_lines)
{
	UINT32		band;
	// initialize the encoder for the layer
	// now quantize and encode each sideband

	if(p_qmf->p_file_qmf->bLowMemCompress) {//[05]
		for(band=0; band < p_qmf->nr_bands; band++ ) {
			QmfLevelBandStruct *p_band = p_qmf->p_bands + band;
			Sideband	sideband;				// [03]
			//UINT64		offset = p_qmf->x_size * p_qmf->next_output_block_y_line * sizeof(UINT16);

			for(sideband = (p_qmf->level ? LH_SIDEBAND : LL_SIDEBAND); sideband <= HH_SIDEBAND; sideband++ ) {
			//	register	IEEE4	*p_line = (IEEE4*)NULL;
				register	INT16	*p_quantized;
			//	NCS_FILE_HANDLE file = NCS_NULL_FILE_HANDLE;
				NCSError eError = NCS_SUCCESS;
				UINT32 x;
				INT32 i;
				UINT8 *p_packed = NULL;
				// select the line to write

				switch(sideband) {
					default:
					case LL_SIDEBAND:
							p_quantized = (INT16*)p_p_ll_lines[band];
							for(x= 0, i = 0; x < p_qmf->x_size; x += MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x), i++) {
								/* Pack the block */
								
								eError = pack_data(p_qmf->p_file_qmf, 
											&p_packed, &(p_band->p_p_ll_lengths[p_qmf->next_output_block_y_line][i]),
											(UINT8*)(p_quantized + x), MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x) * sizeof(INT16),
												sizeof(INT16), FALSE);
								if(eError != NCS_SUCCESS)
									return(eError);
								p_band->p_p_p_ll_segs[p_qmf->next_output_block_y_line][i] = NCSMalloc(p_band->p_p_ll_lengths[p_qmf->next_output_block_y_line][i] + 1, FALSE);
								memcpy(p_band->p_p_p_ll_segs[p_qmf->next_output_block_y_line][i], p_packed, p_band->p_p_ll_lengths[p_qmf->next_output_block_y_line][i]);
								NCSFree(p_packed);
							}
						break;
					case LH_SIDEBAND:
							p_quantized = p_band->p_quantized_output_lh_block;
							for(x = 0, i = 0; x < p_qmf->x_size; x += (UINT32)MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x), i++) {
								/* Pack the block */
								eError = pack_data(p_qmf->p_file_qmf, 
											&p_packed, &(p_band->p_p_lh_lengths[p_qmf->next_output_block_y_line][i]),
											(UINT8*)(p_quantized + x), MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x) * sizeof(INT16),
												sizeof(INT16), FALSE);
								if(eError != NCS_SUCCESS)
									return(eError);
								p_band->p_p_p_lh_segs[p_qmf->next_output_block_y_line][i] = NCSMalloc(p_band->p_p_lh_lengths[p_qmf->next_output_block_y_line][i] + 1, FALSE);
								memcpy(p_band->p_p_p_lh_segs[p_qmf->next_output_block_y_line][i], p_packed, p_band->p_p_lh_lengths[p_qmf->next_output_block_y_line][i]);
					NCSFree(p_packed);
							}

						break;
					case HL_SIDEBAND:
							p_quantized = p_band->p_quantized_output_hl_block;
							for(x = 0, i = 0; x < p_qmf->x_size; x += MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x), i++) {
								/* Pack the block */
								eError = pack_data(p_qmf->p_file_qmf, 
											&p_packed, &(p_band->p_p_hl_lengths[p_qmf->next_output_block_y_line][i]),
											(UINT8*)(p_quantized + x), MIN(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x) * sizeof(INT16),
												sizeof(INT16), FALSE);
								if(eError != NCS_SUCCESS)
									return(eError);
								p_band->p_p_p_hl_segs[p_qmf->next_output_block_y_line][i] = NCSMalloc(p_band->p_p_hl_lengths[p_qmf->next_output_block_y_line][i] + 1, FALSE);
								memcpy(p_band->p_p_p_hl_segs[p_qmf->next_output_block_y_line][i], p_packed, p_band->p_p_hl_lengths[p_qmf->next_output_block_y_line][i]);
					NCSFree(p_packed);
						}

						break;
					case HH_SIDEBAND:
							p_quantized = p_band->p_quantized_output_hh_block;
							for(x = 0, i = 0; x < p_qmf->x_size; x += (UINT32)NCSMin(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x), i++) {
								/* Pack the block */
								eError = pack_data(p_qmf->p_file_qmf, 
											&p_packed, &(p_band->p_p_hh_lengths[p_qmf->next_output_block_y_line][i]),
											(UINT8*)(p_quantized + x), NCSMin(p_qmf->x_block_size * (UINT32)NCS_NR_BLOCKS, p_qmf->x_size - x) * sizeof(INT16),
												sizeof(INT16), FALSE);
								if(eError != NCS_SUCCESS)
									return(eError);
								p_band->p_p_p_hh_segs[p_qmf->next_output_block_y_line][i] = NCSMalloc(p_band->p_p_hh_lengths[p_qmf->next_output_block_y_line][i] + 1, FALSE);
								memcpy(p_band->p_p_p_hh_segs[p_qmf->next_output_block_y_line][i], p_packed, p_band->p_p_hh_lengths[p_qmf->next_output_block_y_line][i]);
NCSFree(p_packed);
						}

					break;
				}
				
				if(eError != NCS_SUCCESS) {
					return(eError);
				}
			}	/* end sideband */
		}	/* end band */
	}
	p_qmf->next_output_block_y_line += 1;

	if( p_qmf->next_output_block_y_line >= p_qmf->y_block_size
	 || y >= p_qmf->y_size-1 )		// at end of blocks or file, so write it
		return( qencode_qmf_level_output_block(p_qmf) );
	return(NCS_SUCCESS);
}

/*
**	Encode the blocks for this Y set of block that have been buffered,
**	and write to disk, for the indicated level. Also update offset pointer for the block.
**	As we have to write the length of each band/subband first, we compute this offset and skip it,
**	write all the subbands, then go back and write the initial offsets, then seek again
**	to the end of the block. This avoids having to keep the (potentially very large) number of
**	compressed bands * subbands in memory.
**	We always write (BANDS * SUBBANDS)-1 offsets, as the offset to the last 
**	In other words, if there are 4 subbands, we write three offsets. If there are 3 subbands,
**	we write 2 offsets.
*/
static NCSError qencode_qmf_level_output_block(QmfLevelStruct *p_qmf )
{
	UINT32	x_block;
	UINT32	y_block;
	UINT32	y_block_size;		/* length of y block, <= p_qmf->y_block_size */
	
	y_block = ((p_qmf->next_output_line + p_qmf->y_block_size + 8) / p_qmf->y_block_size) - 1;
	y_block_size = p_qmf->next_output_block_y_line;
{
	int i;
	INT32 max_blocks = NCSMin(NCS_NR_BLOCKS, p_qmf->nr_x_blocks);

	UINT16	**p_p_ll = NULL;
	UINT16	**p_p_lh = NULL;
	UINT16	**p_p_hl = NULL;
	UINT16	**p_p_hh = NULL;

	if(p_qmf->p_file_qmf->bLowMemCompress) {
		if(p_qmf->level == 0) {
			p_p_ll = NCSMalloc(p_qmf->nr_bands * sizeof(UINT16*), FALSE);
		}
		p_p_lh = NCSMalloc(p_qmf->nr_bands * sizeof(UINT16*), FALSE);
		p_p_hl = NCSMalloc(p_qmf->nr_bands * sizeof(UINT16*), FALSE);
		p_p_hh = NCSMalloc(p_qmf->nr_bands * sizeof(UINT16*), FALSE);
		for(i = 0; i < p_qmf->nr_bands; i++) {
			if(p_qmf->level == 0) {
				p_p_ll[i] = NCSMalloc(p_qmf->x_block_size * y_block_size * max_blocks * sizeof(UINT16), FALSE);
			}
			p_p_hl[i] = NCSMalloc(p_qmf->x_block_size * y_block_size * max_blocks * sizeof(UINT16), FALSE);
			p_p_lh[i] = NCSMalloc(p_qmf->x_block_size * y_block_size * max_blocks * sizeof(UINT16), FALSE);
			p_p_hh[i] = NCSMalloc(p_qmf->x_block_size * y_block_size * max_blocks * sizeof(UINT16), FALSE);
		}
	}
	for( x_block = 0; x_block < p_qmf->nr_x_blocks; x_block++ ) {
		UINT32	x_block_byte_size;		/* size in BYTES across this X block, <= p_qmf->x_block_size */ 
		UINT16	band;
		Sideband	sideband;
		UINT64	block_length;
#ifdef LINUX
		UINT64 block_pos_start, block_pos_end;
#else
		fpos_t block_pos_start, block_pos_end;
#endif

		// we have to record the start of the block location so we can come back and write
		// the band/subband offsets after writing the compressed subbands out.
		block_pos_start = NCSFileTellNative(p_qmf->tmp_file);
		// now offset by the space reserved for the offsets, and start writing from there.
		block_pos_end = block_pos_start + 
				(sizeof(UINT32) * ((p_qmf->nr_bands * (p_qmf->level ? MAX_SIDEBAND-1 : MAX_SIDEBAND)) - 1));
		if( NCSFileSeekNative(p_qmf->tmp_file, block_pos_end, NCS_FILE_SEEK_START) != block_pos_end ) {
			return(NCS_FILE_SEEK_ERROR);
		}

		// Now work out the size of this block, in symbols, and in byte size
		if( x_block != (p_qmf->nr_x_blocks - 1) )
			x_block_byte_size = p_qmf->x_block_size;
		else
			x_block_byte_size = (p_qmf->x_size - (x_block * p_qmf->x_block_size));
		x_block_byte_size *= sizeof(INT16);

		for( band = 0; band < p_qmf->nr_bands; band++ ) {
			QmfLevelBandStruct *p_band = p_qmf->p_bands + band;
			/*
			**	Encode each sideband for this block
			*/
			for(sideband = (p_qmf->level ? LH_SIDEBAND : LL_SIDEBAND); sideband <= HH_SIDEBAND; sideband++ ) {
				UINT8	*p_unpacked, *p_a_block;
				UINT16	*p_line = NULL;
				UINT32	y_block;
				NCSError eError = NCS_SUCCESS;
				p_a_block = p_qmf->p_a_block;

				// write out the line
				switch(sideband) {
					default:
					case LL_SIDEBAND:
							if(p_qmf->p_file_qmf->bLowMemCompress) { 
								p_line = p_p_ll[band];
							} else {
								p_line = (UINT16*)p_band->p_quantized_output_ll_block;
							}
					break;
					case LH_SIDEBAND:
							if(p_qmf->p_file_qmf->bLowMemCompress) { 
								p_line = p_p_lh[band];
							} else {
								p_line = (UINT16*)p_band->p_quantized_output_lh_block;
							}
					break;
					case HL_SIDEBAND:
							if(p_qmf->p_file_qmf->bLowMemCompress) { 
								p_line = p_p_hl[band];
							} else {
								p_line = (UINT16*)p_band->p_quantized_output_hl_block;
							}
					break;
					case HH_SIDEBAND:
							if(p_qmf->p_file_qmf->bLowMemCompress) { 
								p_line = p_p_hh[band];
							} else {
								p_line = (UINT16*)p_band->p_quantized_output_hh_block;
							}
					break;
				}
				/* offset across into the correct X block */
				if(p_qmf->p_file_qmf->bLowMemCompress) {//[05]
					if(x_block % max_blocks == 0) {
					} else {
						p_line = p_line + ((x_block % max_blocks) * p_qmf->x_block_size);
					}
				} else {
					p_line = p_line + (x_block * p_qmf->x_block_size);
				}
				p_unpacked = p_a_block;
				y_block = y_block_size;
				/* Move the block into a single array */
				while( y_block-- ) {
					if(p_qmf->p_file_qmf->bLowMemCompress) {//[05]
						i = y_block_size - (y_block + 1);
						if(x_block % max_blocks == 0) {
							INT32 nBlocks = NCSMin(max_blocks, (INT32)(p_qmf->nr_x_blocks - x_block));
							INT32 nReadSize = nBlocks * p_qmf->x_block_size * sizeof(UINT16);
		//					INT64 offset = NCSFileTellNative(file) + p_qmf->x_size * sizeof(INT16);// - x_block_byte_size;
							int	rval;
							if(x_block + nBlocks == p_qmf->nr_x_blocks) {
								nReadSize -= ((p_qmf->nr_x_blocks * p_qmf->x_block_size) - p_qmf->x_size) * sizeof(UINT16);
							}
							switch(sideband) {
								default:
								case LL_SIDEBAND:
										rval = unpack_data((UINT8**)&p_line,
															p_band->p_p_p_ll_segs[i][x_block / max_blocks], nReadSize,
															1);
										NCSFree(p_band->p_p_p_ll_segs[i][x_block / max_blocks]);
										p_band->p_p_p_ll_segs[i][x_block / max_blocks]=NULL;
									break;
								case LH_SIDEBAND:
										rval = unpack_data((UINT8**)&p_line,
															p_band->p_p_p_lh_segs[i][x_block / max_blocks], nReadSize,
															1);
											NCSFree(p_band->p_p_p_lh_segs[i][x_block / max_blocks]);
										p_band->p_p_p_lh_segs[i][x_block / max_blocks]=NULL;
								break;
								case HL_SIDEBAND:
										rval = unpack_data((UINT8**)&p_line,
															p_band->p_p_p_hl_segs[i][x_block / max_blocks], nReadSize,
															1);
											NCSFree(p_band->p_p_p_hl_segs[i][x_block / max_blocks]);
										p_band->p_p_p_hl_segs[i][x_block / max_blocks]=NULL;
								break;
								case HH_SIDEBAND:
										rval = unpack_data((UINT8**)&p_line,
															p_band->p_p_p_hh_segs[i][x_block / max_blocks], nReadSize,
															1);
											NCSFree(p_band->p_p_p_hh_segs[i][x_block / max_blocks]);
										p_band->p_p_p_hh_segs[i][x_block / max_blocks]=NULL;
							break;
							}
					
							if(eError != NCS_SUCCESS) {
								return(eError);
							}
//							if(offset != NCSFileSeekNative(file, offset, NCS_FILE_SEEK_START)) {
	//							return(NCS_FILE_SEEK_ERROR);
	//						}
						}
						memcpy(p_unpacked, p_line, x_block_byte_size);
						p_line += p_qmf->x_block_size * max_blocks;		/* offset to next X location */
					} else {
						memcpy(p_unpacked, p_line, x_block_byte_size);
						p_line += p_qmf->x_size;		/* offset to next X location */
					}
					p_unpacked += x_block_byte_size;
				}

				{
					UINT8	*p_packed;
					/* Pack the block */
					eError = pack_data(p_qmf->p_file_qmf, 
								  &p_packed, &(p_band->packed_length[sideband]),
								  p_a_block, x_block_byte_size * y_block_size,
									sizeof(INT16), TRUE);
					if(eError != NCS_SUCCESS)
						return(eError);

					/* write the block and free the compressed subband memory */
					if( (eError = NCSFileWrite(p_qmf->tmp_file, p_packed, p_band->packed_length[sideband], NULL)) != NCS_SUCCESS) {
						NCS_SAFE_FREE(p_packed);
						return(eError);
					}
					NCS_SAFE_FREE(p_packed);
				}

			}
		}	/* end band loop */

		/*
		**	Now go back, write offsets, then seek to the end of the block ready for the next block
		*/
		block_pos_end = NCSFileTellNative(p_qmf->tmp_file);
		NCSFileSeekNative(p_qmf->tmp_file, block_pos_start, NCS_FILE_SEEK_START);
		block_length = 0;
		for( band = 0; band < p_qmf->nr_bands; band++ ) {
			QmfLevelBandStruct *p_band = p_qmf->p_bands + band;
			for(sideband = (p_qmf->level ? LH_SIDEBAND : LL_SIDEBAND); sideband <= HH_SIDEBAND; sideband++ ) {
				block_length += p_band->packed_length[sideband];
				/*
				**	The last subband in the last band does not get an offset output
				*/
				if( (band != (p_qmf->nr_bands - 1)) || (sideband != HH_SIDEBAND) ) {
					NCSError eError;
					if( (eError = NCSFileWriteUINT32_MSB(p_qmf->tmp_file, (UINT32) block_length)) != NCS_SUCCESS ) {
						return(eError);
					}
				}
			}
		} /* end band loop to write subband offsets */
		if(NCSFileSeekNative(p_qmf->tmp_file, block_pos_end, NCS_FILE_SEEK_START) != block_pos_end) {
			return(NCS_FILE_SEEK_ERROR);
		}
		
		/*
		** For now, we record the length of this packed block. (block has all bands, all sidebands for this block)
		** Later on, during file write, we will go back and convert this to
		** be an offset and not a length
		** The block length is updated to include the sideband offset pointers
		*/

		block_length += 	(sizeof(UINT32) * ((p_qmf->nr_bands * (p_qmf->level ? MAX_SIDEBAND-1 : MAX_SIDEBAND)) - 1));
		{
			NCSError eError;
			INT64 nSeekOffset = p_qmf->p_top_qmf->file_offset + (p_qmf->first_block_offset + p_qmf->next_block_offset) * sizeof(UINT64);
	
			if(nSeekOffset != NCSFileSeekNative(p_qmf->p_top_qmf->outfile, nSeekOffset, NCS_FILE_SEEK_START)) {
				return(NCS_FILE_SEEK_ERROR);
			}
			eError = NCSFileWrite(p_qmf->p_top_qmf->outfile, &block_length, sizeof(UINT64), NULL);
			if(eError != NCS_SUCCESS) {
				return(eError);
			}
		}
		//p_qmf->p_block_offsets[p_qmf->next_block_offset] = block_length;

		/* go on to the next block */
		p_qmf->next_block_offset += 1;
	}	/* next block loop */
	p_qmf->next_output_block_y_line = 0;		// get read for the next block

#if (defined(DUMP_MEM_STATS)&&defined(_DEBUG))
	{
		FILE *pFile;

		if(pFile = fopen("c:\\temp\\ecwstats.log", "a+")) {
			_CrtMemState State;

			_CrtMemCheckpoint(&State);

			fprintf(pFile, "Free Blocks:   %ld blocks, %ld bytes", 
							State.lCounts[_FREE_BLOCK], 
							State.lSizes[_FREE_BLOCK]);
			fprintf(pFile, "Normal Blocks: %ld blocks, %ld bytes", 
							State.lCounts[_NORMAL_BLOCK], 
							State.lSizes[_NORMAL_BLOCK]);
			fprintf(pFile, "CRT Blocks:    %ld blocks, %ld bytes", 
							State.lCounts[_CRT_BLOCK], 
							State.lSizes[_CRT_BLOCK]);
			fprintf(pFile, "Ignore Blocks: %ld blocks, %ld bytes", 
							State.lCounts[_IGNORE_BLOCK], 
							State.lSizes[_IGNORE_BLOCK]);
			fprintf(pFile, "Client Blocks: %ld blocks, %ld bytes", 
							State.lCounts[_CLIENT_BLOCK], 
							State.lSizes[_CLIENT_BLOCK]);
			fprintf(pFile, "Total Allocated: %ld", State.lTotalCount);
			fprintf(pFile, "Max. Allocated:  %ld", State.lHighWaterCount);
			fprintf(pFile, "%%%ld Heap Utilisation", 100 - ((State.lSizes[_FREE_BLOCK] * 100) / State.lTotalCount));
			fclose(pFile);
		}
	}
#endif
	for(i = 0; i < p_qmf->nr_bands; i++) {
		if(p_p_ll) NCSFree(p_p_ll[i]);
		if(p_p_lh) NCSFree(p_p_lh[i]);
		if(p_p_hl) NCSFree(p_p_hl[i]);
		if(p_p_hh) NCSFree(p_p_hh[i]);
	}
	NCSFree(p_p_ll);
	NCSFree(p_p_lh);
	NCSFree(p_p_hl);
	NCSFree(p_p_hh);
#ifdef NCS_THREAD_PACK
	for(i = 0; i < nThreads; i++) {
		while(!NCSThreadIsRunning(&pThreads[i])) {
			NCSThreadYield();
		}
	}
	bRun = FALSE;
	for(i = 0; i < nThreads; i++) {
		while(NCSThreadIsRunning(&pThreads[i])) {
			NCSThreadYield();
		}
	}
	NCSFree(pThreads);
	NCSQueueDestroy(pUnpackedQ);
	NCSQueueDestroy(pPackedQ);
#endif // NCS_THREAD_PACK
}

	return(NCS_SUCCESS);
}
