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
** FILE:   	qmf_util.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	QMF tree utilities
** COMMENTS:
**
**	Creates a QMF tree structure to be used for compression
**	or decompression.
**
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns 10dec98 handling small file error in GUI
** [03] sns	01mar99	Added NCS specific code
** [04] sns 26mar99 Adding variable level binsizes to preserve detail at largest level
** [05] sns 02Apr99 adding Windows specific file IO logic instead of fopen() to bypass max 512 file limit
** [06] sns 20May99 Added more compression specific functionality
** [07] sjc 20Aug99 Fixed memory leak on p_bands
** [08] sjc 06Sep99 Huffman optimisations
** [09] sjc 30Apr00	Merged Mac SDK port
** [10] sns 14Jul00	Better bin size calculation for larger files
** [11] rar 17-Jan-01 Mac port changes
** [12] rar 24-Jan-01 Mac port changes
** [13] md  06Sep01 Undid [10] - too much quality degradation
** [14] rar 18Sep01 Modified erw_decompress_open so that if the Block Table is uncompressed (RAW)
**                  it is not passed through unpack_data (which just does a memcpy anyway).
** [15] sjc 30Apr03 Added low-memory compression
 *******************************************************/

#include "NCSEcw.h"
#ifdef NOTDEF
[13]
#include <math.h>		// [10] needed for pow() function
#endif

/*
**	Static functions
*/


/**************************************************************************
**	QMF level structure allocatation and deletion routines. These
**	are used to allocate new QMF level structures, allocate their working
**	buffers, and to free the tree of levels up when finished or when an
**	error occurs.
**
**	These routines are the central allocation/management routines; look
**	here first for setup details (such as binsize allocations).
**
**************************************************************************/

// setup_qmf_tree() now only used to build the compression tree

#if defined(ECW_COMPRESS)
/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Allocate a new qmf level.
**
**	Now ONLY used for building a QMF tree for compression
**
** Constructs a complete QMF tree ready for processing wavelet based images
**
** You hand it the tree x_size, y_size, bands, and other information.
** It then constructs a tree ready for compression or decompression.
** Set compression = TRUE to indicate the tree will be used for compression
**
**	Inputs:
**	*num_levels			- will be returned with the number of levels in this tree (COMPRESS)
**						- Must contain the tree levels (DECOMPRESS)
**	x_size, y_size		- size of the ultimate file
**	bands				- number of bands
**	compression_factor	- factor to compress to (COMPRESS)
**	compress_type		- style of compressionion
**	p_input_vds			- pointer to the VDS to read for input (COMPRESS)
**	compress			- TRUE if compress, FALSE if decompress
**
**	Returns the pointer to the top of the tree if all went well, otherwise NULL.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

NCSError setup_qmf_tree(QmfLevelStruct **pp_qmf,
						struct EcwCompressionTask *pCompressionTask,
						UINT32 nBlockSizeX, UINT32 nBlockSizeY, 
						UINT16 *num_levels, UINT32 x_size, UINT32 y_size, UINT32 nr_bands,
						UINT32 nInputBands, CompressHint eCompressHint,
						IEEE4 compression_factor, CompressFormat compress_format, int compress )
{
	QmfLevelStruct	*p_qmf, *p_top_qmf;
	UINT32	level_x_size, level_y_size;
	UINT32	band;
	IEEE4	qmf_bin_size;
#ifdef NOTDEF
[13]
**	IEEE4	qmf_rescale_factor;		// [10]
#endif
	NCSError eError;

#ifdef NOTDEF
	INT32 i;
#endif

	INT64 nMemRequired = 0;//[15]
	BOOLEAN bForceLowMemCompress = FALSE;//[15]

	eCompressHint;nInputBands;//Keep compiler happy

	*pp_qmf = NULL;

	if( x_size < MIN_QMF_SIZE*2 || y_size < MIN_QMF_SIZE * 2 ) {
		return(NCS_INPUT_SIZE_TOO_SMALL);
	}

	/*
	**	Build tree in reverse order, then reverse level numbers once built
	*/
	*num_levels = 0;
	p_qmf = p_top_qmf = new_qmf_level(nBlockSizeX, nBlockSizeY,
		*num_levels, x_size, y_size, nr_bands, NULL, NULL, compress);
	if( !p_top_qmf )
		return(NCS_COULDNT_ALLOC_MEMORY);
	p_qmf->compress_format = compress_format;
	p_qmf->compression_factor = compression_factor;
	p_qmf->p_file_qmf = p_top_qmf;		// top currently points to the fake file QMF level

	nMemRequired += sizeof(IEEE4) *	2 * (FILTER_SIZE+1)	* x_size * nr_bands; //[15] Input LL buffers
	nMemRequired += sizeof(UINT16) * nBlockSizeY * x_size * nr_bands * 3; //[15] Output quantized SB buffers
	nMemRequired += sizeof(IEEE4) * x_size * MAX_THREAD_LINES * nr_bands;//[15] input thread queue buffers
	nMemRequired += sizeof(IEEE4) * x_size * 3;//[15] Output LH,HL,HH scanline buffer
	nMemRequired += sizeof(IEEE4) * x_size * nr_bands; //[15] Top LL input scanline buffer

//	fprintf(stdout, "%I64d\r\n", nMemRequired);
//	fflush(stdout);

	NCSecwGetConfig(NCSCFG_FORCE_LOWMEM_COMPRESS, &bForceLowMemCompress);//[15]
	if(nMemRequired >= NCSPhysicalMemorySize() / 2 || bForceLowMemCompress) {//[15]ut
		p_qmf->p_file_qmf->bLowMemCompress = TRUE;
	}
//p_qmf->p_file_qmf->bLowMemCompress = TRUE;
	// set up QMF structure, working from largest to smallest
	level_x_size = x_size;
	level_y_size = y_size;
	while( (level_x_size >= (MIN_QMF_SIZE * 2)) && (level_y_size >= (MIN_QMF_SIZE * 2)) ) {
		level_x_size = (level_x_size + 1)/2;	// must ensure we preserve odd numbers of lines
		level_y_size = (level_y_size + 1)/2;
		(*num_levels)++;
		p_qmf = new_qmf_level(nBlockSizeX, nBlockSizeY,
			*num_levels, level_x_size, level_y_size, nr_bands, NULL, p_qmf, compress);
		if( !p_qmf ) {
			delete_qmf_levels(p_top_qmf);
			return(NCS_COULDNT_ALLOC_MEMORY);
		}
		// handy to have these at each level
		p_qmf->compress_format = compress_format;
		p_qmf->compression_factor = compression_factor;

		p_qmf->p_file_qmf = p_top_qmf;		// top currently points to the fake file QMF level
	}

	if( *num_levels > MAX_LEVELS ) { 
		printf("Error: cannot build pyramid to more than %d levels.\n", MAX_LEVELS); 
		delete_qmf_levels(p_top_qmf);
		return(NCS_INPUT_SIZE_EXCEEDED);
	}


	// Reverse order, so now top is smallest. Level 0 is smallest level.
	p_top_qmf = p_qmf;
	{
		int level = 0;
		while(p_qmf) {
			p_qmf->level = (UINT16)level++;
		//	p_qmf->eCompressHint = eCompressHint;	// [06]
		//	p_qmf->nInputBands = nInputBands;
			p_qmf->pCompressionTask = pCompressionTask;
			p_qmf->p_top_qmf = p_top_qmf;
			p_qmf = p_qmf->p_larger_qmf;
		}
	}

	// Allocate all the buffers
	// Also sets up internal calculation values, file structures, initial
	// LL pointers and the like for the QMF compression tree
	eError = allocate_qmf_buffers(p_top_qmf, compress);//TRUE);
	if(eError != NCS_SUCCESS) {
		delete_qmf_levels(p_top_qmf);
		return(eError);
	}

	/*
	**  set up bin size quantizations details for compression.
	**	If YUV, we want Y to have most of the information,
	**	so given (for example) a desired compression ratio of:
	**	50:1, the standard YUV encoding will normally give us
	**	about 25:1 by doing nothing special, as U and V already
	**	have little information.  We want to preserve resolution
	**	for Y, and reduce it for U and V. U is less sensitive than V.
	*/

#ifdef NOTDEF
** [13]
**	/*
**	** [10] Work out bin rescaling factor. May be > 2.0 for large files with many levels
**	*/
**	qmf_rescale_factor = QmfBinRescale(p_top_qmf,compression_factor * 2);	// [10]
#endif

	p_qmf = p_top_qmf;
	// get largest valid sub-level (that is below file level)
	while(p_qmf->p_larger_qmf)
		p_qmf = p_qmf->p_larger_qmf;

	// need to point to largest valid QMF below the file level
	p_qmf = p_qmf->p_smaller_qmf;
	qmf_bin_size = compression_factor * 2;

	while( p_qmf ) {
		// For the smallest level, go to a low compression to improve lowpass image
		if( p_qmf->p_smaller_qmf == NULL )	// at smallest QMF level
				qmf_bin_size = 1;			// force lower compression rate for LL band
		for(band = 0; band < nr_bands; band++ ) {
			if( p_top_qmf->compress_format == COMPRESS_YUV ) {
				switch( band ) {
					case 0:
						if( qmf_bin_size < 2.0 )
							p_qmf->p_band_bin_size[band] = 1;
						else
							p_qmf->p_band_bin_size[band] = (UINT32) (qmf_bin_size / 2.0);	/* Y */
					break;
					case 1:
						if( qmf_bin_size < 0.7 )
							p_qmf->p_band_bin_size[band] = 1;
						else
							p_qmf->p_band_bin_size[band] = (UINT32) (qmf_bin_size * 1.5);	/* U */
					break;
					case 2:
						if( qmf_bin_size < 0.7 )
							p_qmf->p_band_bin_size[band] = 1;
						else
							p_qmf->p_band_bin_size[band] = (UINT32) (qmf_bin_size * 1.5);	/* V */
					break;
					default:
						if( qmf_bin_size < 1.0 )
							p_qmf->p_band_bin_size[band] = 1;
						else
							p_qmf->p_band_bin_size[band] = (UINT32) qmf_bin_size;			/* invalid */
				}
			}
			else {				// not YUV, so do normal bin selection
				if( qmf_bin_size < 1.0 )
					p_qmf->p_band_bin_size[band] = 1;
				else
					p_qmf->p_band_bin_size[band] = (UINT32) qmf_bin_size;
			}
		}

//		if( p_qmf->p_larger_qmf->p_larger_qmf == NULL )	// at largest QMF level
//				qmf_bin_size *= 2;			// For other levels, go back to a higher compression rate
		
		qmf_bin_size /= 2.0;  //[13] make it this again [10] was qmf_bin_size /= 2.0;
		//[13] dont use this qmf_bin_size /= qmf_rescale_factor;// [10] use a variable scale factor based on # levels
		p_qmf = p_qmf->p_smaller_qmf;
	}

	*pp_qmf = p_top_qmf;

	return(NCS_SUCCESS);
}

#endif //ECW_COMPRESS

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Allocate a new qmf level
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

#if (defined(ECW_COMPRESS) && !defined(ECW_ALLOW_MULTI_LINK_STATIC_LIB)) || !defined(ECW_COMPRESS)

QmfLevelStruct *new_qmf_level(UINT32 nBlockSizeX, UINT32 nBlockSizeY,
							  UINT16 level, UINT32 x_size, UINT32 y_size, UINT32 nr_bands,
					QmfLevelStruct *p_smaller_qmf, QmfLevelStruct *p_larger_qmf,
					int compress)
{

	QmfLevelStruct *p_qmf;
	UINT16		band;
	compress;//Keep compiler happy

	p_qmf = (QmfLevelStruct *) NCSMalloc(sizeof(QmfLevelStruct), TRUE);
	if( p_qmf ) {

		p_qmf->version = ERSWAVE_VERSION;
		p_qmf->nr_sidebands = MAX_SIDEBAND;
		p_qmf->blocking_format = BLOCKING_LEVEL;
		p_qmf->compress_format = COMPRESS_UINT8;

		p_qmf->level = level;
		p_qmf->x_size = x_size;
		p_qmf->y_size = y_size;
		p_qmf->nr_bands = (UINT16)nr_bands;
		p_qmf->x_block_size = (UINT16)nBlockSizeX;
		p_qmf->y_block_size = (UINT16)nBlockSizeY;
		p_qmf->nr_x_blocks = QMF_LEVEL_NR_X_BLOCKS(p_qmf);
		p_qmf->nr_y_blocks = QMF_LEVEL_NR_Y_BLOCKS(p_qmf);
        p_qmf->tmp_file = NCS_NULL_FILE_HANDLE;
		{
			ECWFILE null_file = NULL_ECWFILE;
			p_qmf->hEcwFile = null_file;			// [05] Input ECW file pointer (note - NULL might be a valid Windows handle (!))
		}
		p_qmf->p_band_bin_size = (UINT32 *) NCSMalloc( nr_bands * sizeof(UINT32), FALSE);
		if( !p_qmf->p_band_bin_size ) {
			NCS_SAFE_FREE((char *) p_qmf);
			return(NULL);
		}
		for(band=0; band < nr_bands; band++) {
			p_qmf->p_band_bin_size[band] = 1;		// no bin size defined yet
		}

		if( p_smaller_qmf ) {
			p_smaller_qmf->p_larger_qmf = p_qmf;
			p_qmf->p_smaller_qmf = p_smaller_qmf;
		}

		if( p_larger_qmf ) {
			p_larger_qmf->p_smaller_qmf = p_qmf;
			p_qmf->p_larger_qmf = p_larger_qmf;
		}
	}
	return(p_qmf);
}

#endif

#if (!defined(ECW_COMPRESS) && defined(ECW_ALLOW_MULTI_LINK_STATIC_LIB)) || !defined(ECW_ALLOW_MULTI_LINK_STATIC_LIB)
/**************************************************************************/

// Allocate the qmf buffers for all levels. This must be done
// after the qmf level structures have been allocated for all
// levels. Only enough buffers are allocated at each level
// to hold the working lines needed for that level.
// TOP QMF is always the smallest QMF
//
// Output generation:
//	To generate a single set of sideband (LL,LH,HH and LL) lines,
//	enough LOWPASS and HIGHPASS lines are needed to cover the size
//	of the filter bank.  A set of (1 of each) LOWPASS and HIGHPASS lines
//	is generated by sampling a single INPUT line multiplied by a horizontal
//	filter tap.  The INPUT is sampled every second value for LOWPASS and HIGHPASS,
//	with the HIGHPASS offset by 1 across in the sampling.  As this is a horizontal sampling,
//	so we only need one INPUT line to generate a pair (1 of each) of
//	LOWPASS and HIGHPASS lines.  So LOWPASS uses INPUT [0,2,4,...]
//	and HIGHPASS uses INPUT [1,3,5,...]
//
//	A set (1 of each) LL, LH, HL and HH lines is generated by sampling FILTER_TAP+1
//	LOWPASS and HIGHPASS lines.  We need FILTER_TAP+1 because (a) this is a vertical
//	sampling and (b) the HL and HH lines are offset by one down. So we need to keep
//	FILTER_SIZE+1 LOWPASS and HIGHPASS lines around for this level to generate
//	the LL, LH, HL and HH lines.
// 
//	LL and LH use input lines [0,2,5, .. ,FILTER_SIZE-1] and HL and HH use
//	input lines [1,3,5, ... ,FILTER_SIZE].
//	As the LOWPASS/HIGHPASS lines are used as input to vertical filters,
//	there is no left/right reflection on these, so they can be the size of
//	this level (which, note well, might be 1 more than (p_qmf->p_larger_qmf->x_size*2),
//	as we round up when defining the level sizes, in order to not lose data.
//
//	We also allocate enough quantized output lines for each sideband to be compressed,
//	for the number of y lines to be buffered before being written to output.
//
// Input generation:
//	A given QMF level will call the next larger QMF level, asking for an INPUT line
//	(which is actually the LL output from the called larger level).  As we are doing
//	horizontal filter taps on the INPUT line, we need to have left/right reflection.
//	Also, this level might be ((larger level size * 2)+1) in size.  So we can allocate
//	the INPUT line size to be:	((p_qmf->x_size) + FILTER_SIZE-1).
//	As the INPUT line buffer is allocated by the current level for requesting data from
//	a larger level, the largest level (the file), does not need one of these, as it
//	is never requesting input from a still larger level.
//
// Errors:
//	If we had an error (most likely out of memory), the calling routine should delete
//	the entire QMF tree, as it is in an undefined state (but safe to delete)
//

NCSError allocate_qmf_buffers(QmfLevelStruct *p_top_qmf, int compress)
{
	QmfLevelStruct	*p_qmf = p_top_qmf;
	UINT32	nr_blocks;
//	char	*error_msg = "allocate_qmf_buffers: Unable to allocate memory for compressed file QMF tree structures";

	// Allocate top level items
	// allocate space for offsets for all blocks for all levels, at the top level only
	nr_blocks = get_qmf_tree_nr_blocks(p_top_qmf);

//	[03] now possible to have a decompression structure without offset blocks in memory
//	if( !(p_qmf->p_block_offsets) )
//		return(1);

	// [02] set up block number offsets
	{
		UINT32	nFirstBlockNumber = 0;
		while( p_qmf->p_larger_qmf ) {
			p_qmf->nFirstBlockNumber = nFirstBlockNumber;
			nFirstBlockNumber += (p_qmf->nr_x_blocks * p_qmf->nr_y_blocks);
			p_qmf = p_qmf->p_larger_qmf;
		}
		p_qmf = p_top_qmf;
	}

	// Allocate decompression specific items
	if( !compress ) {
		if( p_qmf->p_block_offsets ) {		// [03] block offsets table is now optional
			while(p_qmf) {
				if( p_qmf->p_larger_qmf ) {
					if( p_qmf != p_top_qmf ) {
						p_qmf->p_a_block = p_top_qmf->p_a_block;
						p_qmf->p_block_offsets = p_qmf->p_smaller_qmf->p_block_offsets + 
							(p_qmf->p_smaller_qmf->nr_x_blocks * p_qmf->p_smaller_qmf->nr_y_blocks);
					}
				}
				p_qmf = p_qmf->p_larger_qmf;
			}
		}
	}
	// Allocate compression specific items
	if( compress ) {
		UINT8	tap;
		/* allocate the compression block */
		p_top_qmf->p_a_block = (UINT8 *) NCSMalloc( sizeof(INT16) * p_top_qmf->x_block_size * p_top_qmf->y_block_size, FALSE);

		if( !(p_top_qmf->p_a_block) )
			return(NCS_COULDNT_ALLOC_MEMORY);
		while(p_qmf) {
			UINT16 band;
			NCSError eError;

			if( p_qmf->p_larger_qmf ) {
				// index into or point to data allocated at the top level only
				if( p_qmf != p_top_qmf ) {
					p_qmf->p_a_block = p_top_qmf->p_a_block;
					p_qmf->first_block_offset = p_qmf->p_smaller_qmf->first_block_offset + 
						(p_qmf->p_smaller_qmf->nr_x_blocks * p_qmf->p_smaller_qmf->nr_y_blocks);
				}

				// allocate the input ll line structures, then later cycle through and allocate
				// each line that is pointed to.
				p_qmf->p_p_input_ll_line = (IEEE4 **) NCSMalloc(sizeof(IEEE4 *) * p_qmf->nr_bands, TRUE);
				if( !p_qmf->p_p_input_ll_line ) {
					return(NCS_COULDNT_ALLOC_MEMORY);
				}
	
				// Allocate the band structures. None present for the largest level
				p_qmf->p_bands = (QmfLevelBandStruct *) 
								NCSMalloc( sizeof(QmfLevelBandStruct) * p_qmf->nr_bands, TRUE);
				if( !p_qmf->p_bands ) {
					return(NCS_COULDNT_ALLOC_MEMORY);
				}

				// Allocate the band level structures
				for(band = 0; band < p_qmf->nr_bands; band++ ) {
					QmfLevelBandStruct *p_band = p_qmf->p_bands + band;

					// the low/high lines:
					// allocate the block as a single group. Less malloc calls this
					// way, and greater chance of cache coherency.

					p_band->p_low_high_block = (IEEE4 *) NCSMalloc(
												  sizeof(IEEE4) *	// size of each element
															2	*	// we have a lowpass and a highpass block
												(FILTER_SIZE+1)	*	// we need filtertap vertical high+1 for HL, HH offset
												p_qmf->x_size,		// and each one is the length of this level's line
												FALSE);
					if(!p_band->p_low_high_block)
						return(NCS_COULDNT_ALLOC_MEMORY);
					// now point to the filter block. Group the low lines together, and the high lines together
					for(tap = 0; tap <= FILTER_SIZE; tap++ ) 	// FILTER_TAP + 1 to set up
						p_band->p_p_lo_lines[tap] =
							&p_band->p_low_high_block[tap * p_qmf->x_size];
					for(tap = 0; tap <= FILTER_SIZE; tap++ ) 	// FILTER_TAP + 1 to set up
						p_band->p_p_hi_lines[tap] =
							&p_band->p_low_high_block[(tap * p_qmf->x_size) + ((FILTER_SIZE+1) * p_qmf->x_size)];
					// Now allocate the input LL line. Must be big enough for odd size lines at this
					// level (e.g. larger level x_size might be 63, so this level x_size would be 32 not 31)
					// Again, not needed for the largest file level level. Adds FILTER_SIZE-1 for X reflection
					p_band->p_input_ll_line = NCSMalloc(sizeof(IEEE4) * (p_qmf->x_size*2 + (FILTER_SIZE-1)), FALSE);
					if( p_band->p_input_ll_line == NULL ) {
						return(NCS_COULDNT_ALLOC_MEMORY);
					}
					// must point into input, offset by filter reflection, so recursion is OK
					p_qmf->p_p_input_ll_line[band] = p_band->p_input_ll_line + (FILTER_SIZE / 2);

					// allocate the output LH,HL and HH lines generated each call to the qmf_le	pTmp	CXX0017: Error: symbol "pTmp" not found
					// Again, not needed at the largest (e.g. file) level of the QMF tree
					// No reflection is needed on these

					// allocate the output quantized buffer for Y_BLOCK_SIZE lines. We use a single
					// buffer for each sideband, as it is only accessed once per line during compression writing,
					// so no need to precompute indexes.  The LL block is only allocated for the smallest
					// level, as the LL line is transient data for all other levels.
					if( !p_qmf->level ) {
						if(!p_qmf->p_file_qmf->bLowMemCompress) {//[15]
							p_band->p_quantized_output_ll_block =
								NCSMalloc(sizeof(*(p_band->p_quantized_output_ll_block)) * p_qmf->y_block_size * p_qmf->x_size, FALSE);
							if( !(p_band->p_quantized_output_ll_block) ){
								return(NCS_COULDNT_ALLOC_MEMORY);
							}
						}
					}
					if(p_qmf->p_file_qmf->bLowMemCompress) {//[15]
						int y;
				
						p_band->p_quantized_output_lh_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_lh_block)) * p_qmf->x_size, FALSE);
						p_band->p_quantized_output_hl_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_hl_block)) * p_qmf->x_size, FALSE);
						p_band->p_quantized_output_hh_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_hh_block)) * p_qmf->x_size, FALSE);
						p_band->p_p_ll_lengths = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
						p_band->p_p_p_ll_segs = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
						p_band->p_p_lh_lengths = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
						p_band->p_p_p_lh_segs = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);		
						p_band->p_p_hl_lengths = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
						p_band->p_p_p_hl_segs = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);			
						p_band->p_p_hh_lengths = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
						p_band->p_p_p_hh_segs = NCSMalloc(sizeof(UINT32*) * p_qmf->y_block_size, FALSE);
					
						for(y = 0; y < p_qmf->y_block_size; y++) {
							p_band->p_p_ll_lengths[y] = NCSMalloc(sizeof(UINT32) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_p_ll_segs[y] = NCSMalloc(sizeof(UINT8*) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_lh_lengths[y] = NCSMalloc(sizeof(UINT32) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_p_lh_segs[y] = NCSMalloc(sizeof(UINT8*) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_hl_lengths[y] = NCSMalloc(sizeof(UINT32) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_p_hl_segs[y] = NCSMalloc(sizeof(UINT8*) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_hh_lengths[y] = NCSMalloc(sizeof(UINT32) * (1 + p_qmf->x_size / 1024), FALSE);
							p_band->p_p_p_hh_segs[y] = NCSMalloc(sizeof(UINT8*) * (1 + p_qmf->x_size / 1024), FALSE);
						}
					} else {
						p_band->p_quantized_output_lh_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_lh_block)) * p_qmf->y_block_size * p_qmf->x_size, FALSE);
						p_band->p_quantized_output_hl_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_hl_block)) * p_qmf->y_block_size * p_qmf->x_size, FALSE);
						p_band->p_quantized_output_hh_block =
							NCSMalloc(sizeof(*(p_band->p_quantized_output_hh_block)) * p_qmf->y_block_size * p_qmf->x_size, FALSE);
					}
					if( !(p_band->p_quantized_output_lh_block)
						 || !(p_band->p_quantized_output_hl_block)
						 || !(p_band->p_quantized_output_hh_block) ) {
						return(NCS_COULDNT_ALLOC_MEMORY);
					}				

				}	/* end band loop */
				// set up the temporary files for the compressed sidebands
				// we only need ones for thd LH, HL and HH bands for levels > 0 (> top level)

				p_qmf->tmp_fname = NCSGetTempFileName(p_qmf->pCompressionTask->szTmpDir, "ecw", "");
						
				eError = NCSFileOpen(OS_STRING(p_qmf->tmp_fname), NCS_FILE_READ_WRITE|NCS_FILE_CREATE, &p_qmf->tmp_file); //|NCS_FILE_TEMPORARY
				if( eError != NCS_SUCCESS ) {
					return(eError);
				}
			}
			p_qmf = p_qmf->p_larger_qmf;
		}
	}
	// of compression specific logic
	return(0);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	delete all qmf levels
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
void delete_qmf_levels(QmfLevelStruct *p_qmf )
{
	if( !p_qmf )
		return;

	if( p_qmf->p_top_qmf ) { 	// [03]
		// free the file memory image if present
		NCS_SAFE_FREE( p_qmf->p_top_qmf->pHeaderMemImage );
		// [05] close file handle if it is open, and in decompress mode

//#if !defined(ECW_COMPRESS) || defined(ECW_ALLOW_MULTI_LINK_STATIC_LIB)
		if( p_qmf->p_top_qmf->bEcwFileOpen ) {
			(void) EcwFileClose( p_qmf->p_top_qmf->hEcwFile );

			{
				ECWFILE null_file = NULL_ECWFILE;
				p_qmf->p_top_qmf->hEcwFile = null_file;			// [05] Input ECW file pointer (note - NULL might be a valid Windows handle (!))
			}

			p_qmf->p_top_qmf->bEcwFileOpen = FALSE;
		}
//#endif //!defined(ECW_COMPRESS) || defined(ECW_ALLOW_MULTI_LINK_STATIC_LIB)
		
		// Free the FileInfo structures
		if( p_qmf->p_top_qmf->pFileInfo ) {
			NCSFreeFileInfoEx(p_qmf->p_top_qmf->pFileInfo);
			NCSFree( p_qmf->p_top_qmf->pFileInfo );
			p_qmf->p_top_qmf->pFileInfo = NULL;
		}
	}

	// start at one end
	while(p_qmf->p_larger_qmf)
		p_qmf = p_qmf->p_larger_qmf;

	/*
	**	At file level. Deallocate and close files
	*/

	// and delete all layers
	while(p_qmf) {
		QmfLevelStruct *p_next_qmf = p_qmf->p_smaller_qmf;

		// Free data allocated at the top level but pointed to by larger levels
		if( !p_qmf->p_smaller_qmf ) {
			if( p_qmf->p_top_qmf->bRawBlockTable == TRUE ) {		//[14]
				if( p_qmf->p_block_offsets ) {
					NCS_SAFE_FREE(((char *) p_qmf->p_block_offsets)-sizeof(UINT64));		//[14]
				}
			}														//[14]
			else NCS_SAFE_FREE((char *) p_qmf->p_block_offsets);		//[14]
			NCS_SAFE_FREE((char *) p_qmf->p_a_block);
		}

		NCS_SAFE_FREE((char *) p_qmf->p_band_bin_size);

		// free data held for each band for each level (this is compression only)
		if( p_qmf->p_bands ) {
			UINT16	band;
			for( band = 0; band < p_qmf->nr_bands; band++ ) {
				QmfLevelBandStruct *p_band = p_qmf->p_bands + band;
				int y;

				// don't deallocate the p_p_[lo|hi]__lines, just the block pointed to by them
				NCS_SAFE_FREE((char *) p_band->p_low_high_block );
				NCS_SAFE_FREE((char *) p_band->p_input_ll_line);

	//			if(p_band->quantized_output_ll_file != NCS_NULL_FILE_HANDLE) {
	//				NCSFileClose(p_band->quantized_output_ll_file);//[15]
	//				NCSDeleteFile(p_band->p_quantized_output_ll_fname);//[15]
	//			}
	//			if(p_band->quantized_output_lh_file != NCS_NULL_FILE_HANDLE) {
	//				NCSFileClose(p_band->quantized_output_lh_file);//[15]
	//				NCSDeleteFile(p_band->p_quantized_output_lh_fname);//[15]
	//			}
	//			if(p_band->quantized_output_hl_file != NCS_NULL_FILE_HANDLE) {
	//				NCSFileClose(p_band->quantized_output_hl_file);//[15]
	//				NCSDeleteFile(p_band->p_quantized_output_hl_fname);//[15]
	//			}
	//			if(p_band->quantized_output_hh_file != NCS_NULL_FILE_HANDLE) {
	//				NCSFileClose(p_band->quantized_output_hh_file);//[15]
	//				NCSDeleteFile(p_band->p_quantized_output_hh_fname);//[15]
	//			}
	//			NCS_SAFE_FREE((char *) p_band->p_quantized_output_ll_fname);//[15]
	//			NCS_SAFE_FREE((char *) p_band->p_quantized_output_lh_fname);//[15]
	//			NCS_SAFE_FREE((char *) p_band->p_quantized_output_hl_fname);//[15]
	//			NCS_SAFE_FREE((char *) p_band->p_quantized_output_hh_fname);//[15]

				NCS_SAFE_FREE((char *) p_band->p_quantized_output_ll_block);
				NCS_SAFE_FREE((char *) p_band->p_quantized_output_lh_block);
				NCS_SAFE_FREE((char *) p_band->p_quantized_output_hl_block);
				NCS_SAFE_FREE((char *) p_band->p_quantized_output_hh_block);

				
				for(y = 0; y < p_qmf->y_block_size; y++) {
					if(p_band->p_p_ll_lengths) NCSFree(p_band->p_p_ll_lengths[y]);
					if(p_band->p_p_p_ll_segs) NCSFree(p_band->p_p_p_ll_segs[y]);
					if(p_band->p_p_lh_lengths) NCSFree(p_band->p_p_lh_lengths[y]);
					if(p_band->p_p_p_lh_segs) NCSFree(p_band->p_p_p_lh_segs[y]);
					if(p_band->p_p_hl_lengths) NCSFree(p_band->p_p_hl_lengths[y]);
					if(p_band->p_p_p_hl_segs) NCSFree(p_band->p_p_p_hl_segs[y]);
					if(p_band->p_p_hh_lengths) NCSFree(p_band->p_p_hh_lengths[y]);
					if(p_band->p_p_p_hh_segs) NCSFree(p_band->p_p_p_hh_segs[y]);
				}
				NCSFree(p_band->p_p_ll_lengths);
				NCSFree(p_band->p_p_p_ll_segs);
				NCSFree(p_band->p_p_lh_lengths);
				NCSFree(p_band->p_p_p_lh_segs);		
				NCSFree(p_band->p_p_hl_lengths);
				NCSFree(p_band->p_p_p_hl_segs);			
				NCSFree(p_band->p_p_hh_lengths);
				NCSFree(p_band->p_p_p_hh_segs);
			
			}	/* end band loop */
			NCSFree((char *) p_qmf->p_bands);			/**[07]**/
		}

		// Free the index pointer that was pointing to the p_input_ll_line for each band
		NCS_SAFE_FREE((char *) p_qmf->p_p_input_ll_line);

		// clean up temporary file
		if( p_qmf->tmp_file != NCS_NULL_FILE_HANDLE) {
			(void) NCSFileClose( p_qmf->tmp_file);
			NCSDeleteFile(p_qmf->tmp_fname);
		}
		NCS_SAFE_FREE((char *) p_qmf->tmp_fname);
		NCSFree(p_qmf);
		p_qmf = p_next_qmf;
	}
}

#endif

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Returns the number of blocks+1 in all levels of the QMF tree
**	(excluding file level). Assumes qmf_tree is correct - no error
**	checking is carried out.
**
**	NOTE: 1 more than the actual number of blocks is returned. This
**	is because we have to write one more at the end, indicating the
**	seek location to the end of the file, so it can be used to
**	compute offsets for the final block.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

UINT32 get_qmf_tree_nr_blocks( QmfLevelStruct *p_top_qmf )
{
	UINT32 nr_blocks;
	nr_blocks = 0;
	while(p_top_qmf->p_larger_qmf) {
		nr_blocks += (p_top_qmf->nr_x_blocks * p_top_qmf->nr_y_blocks);
		p_top_qmf = p_top_qmf->p_larger_qmf;
	}
	return(nr_blocks + 1);	// return one more than true number of blocks
}
