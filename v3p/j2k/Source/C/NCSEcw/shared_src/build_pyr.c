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
** FILE:   	build_pyr.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Builds wavelet pyramid from image
** COMMENTS:
**	This set of routines builds QMF trees from the original image.
**	The logic is quite different to the normal wavelet logic, which
**	typically works in memory thusly:
**
**		input:	LL	LL
**				LL	LL
**
**		output:	LL	LH
**				HL	HH
**
**	The above won't work in our case, because our input images
**	are so large they won't fit into memory.  Instead, we
**	use a new single-pass recursive level generation technique
**	invented by Stuart Nixon.
**
**	This technique keeps just enough lowpass/highpass lines in
**	memory to cover the FILTER_SIZE. This means that a 10GB file
**	can be compressed with 5MB of RAM, and a terrabyte
**	file can be compressed with 50MB of RAM with a FILTER_SIZE of
**	15 (much less RAM is required if the filter size is lower).
**
**	In addition, all processing is streamlined to be as sequential
**	as possible through input lines. This optimizes L1 and L2 caches,
**	making compression very fast.
**
**	Disk IO aside, this approach is actually faster than the conventional
**	memory based systems, because it localizes processing, so that
**	CPU and L1/L2 cache systems have a chance to cache input.
**
**	We do expect to be able to allocate enough memory for
**	(FILTER_SIZE * 2) Level N input lines. This is a reasonably low amount of RAM.
**
**	It is however a bit harder to follow the code logic.
**
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns 06Apr99 Added support for 11 tap filters
** [03] sns 21May99 Performance tuning
** [04] sns 06Jun99 Updates to make stand alone Compression SDK library
** [05] mjb 26Jul99 Changed error value to report more user friendly error
** [06] sjc 06Sep99 Fixed left edge reflection
** [07] sns 11May00	Performance tuning
** [08] rar 24-Jan-01 Mac port changes
 *******************************************************/

#include "ECW.h"
#include "NCSECWCompress.h"
//#include "NCSDefs.h"
#undef NCS_VECTOR_CC
#undef NCS_RESTRICT
#undef NCS_OPENMP

#if	__ICL >= 700
#ifdef NCS_VECTOR_CC
#define NCS_RESTRICT restrict
#else
#define NCS_RESTRICT
#endif
#ifdef _OPENMP
#define NCS_OPENMP
#endif // _OPENMP
///QaxMiKW /Qvec_report3 /Qrestrict 

#else
#define NCS_RESTRICT
#endif

#ifdef NCS_OPENMP
#define NCS_VECTOR_CC
#endif // NCS_OPENMP

/*
**	Static functions
*/

static NCSError build_qmf_level_dwt_line(QmfLevelStruct *p_qmf, IEEE4 **p_p_parent_ll_lines);
static void build_qmf_level_roll_line_array(QmfLevelStruct *p_qmf);
static void build_qmf_level_lohi_line_pair(QmfLevelStruct *p_qmf);
static NCSError build_qmf_level_output_line(QmfLevelStruct *p_qmf, IEEE4 **p_p_ll_lines );

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Given a set up QMF tree, and the output file handle, this will
**	write the compressed file.
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/


NCSError build_qmf_compress_file(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile)
{
	IEEE4	**p_p_top_ll_lines;
	IEEE4	*p_top_ll_line_buffer;

	UINT32	line;
	UINT16	band;
	NCSError eError = NCS_SUCCESS;
	QmfLevelStruct	*p_qmf;

	if( !p_top_qmf )
		return(NCS_INVALID_PARAMETER);

	// Allocate a buffer to hold one line of the top level LL.
	// Unlike lower level QMF's, we don't need to reflect this,
	// so it can be exact size. Also, we don't actually use this line
	// outselves; lower level calls will flush it to the compressed file
	// We could perhaps allocate this as a single buffer and index into it;
	// simpler.
	p_p_top_ll_lines = (IEEE4 **) NCSMalloc(sizeof(IEEE4 *) * p_top_qmf->nr_bands, FALSE);
	if(!p_p_top_ll_lines) {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
	p_top_ll_line_buffer = (IEEE4 *) NCSMalloc(sizeof(IEEE4) * p_top_qmf->nr_bands * p_top_qmf->x_size, FALSE);
	if(!p_top_ll_line_buffer) {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
	if( !p_p_top_ll_lines || !p_top_ll_line_buffer ) {
		NCS_SAFE_FREE((char *) p_p_top_ll_lines);
		NCS_SAFE_FREE((char *) p_top_ll_line_buffer);

		return(NCS_COULDNT_ALLOC_MEMORY);
	}
	// now index into the line buffer for each band
	for( band = 0; band < p_top_qmf->nr_bands; band++ )
		p_p_top_ll_lines[band] = p_top_ll_line_buffer + (band * p_top_qmf->x_size);


	// Write ERW header information
	eError = write_compressed_preamble(p_top_qmf, outfile);
	
	if(eError != NCS_SUCCESS)
		return(eError);

	/*
	** read smallest level sideband LL, LH, HL and HH lines.
	** This will trigger recursive reads of larger levels, writing
	** compressed data out as it goes.
	*/
	for(line = 0; line < p_top_qmf->y_size; line++) {
		eError = build_qmf_level_dwt_line(p_top_qmf, p_p_top_ll_lines);
		if( eError != NCS_SUCCESS ) {
			NCS_SAFE_FREE((char *) p_p_top_ll_lines);
			NCS_SAFE_FREE((char *) p_top_ll_line_buffer);
			return(eError);
		}
	}

	// Ditch the line buffers; not needed any more
	NCS_SAFE_FREE((char *) p_p_top_ll_lines);
	NCS_SAFE_FREE((char *) p_top_ll_line_buffer);

	// convert the block table
	eError = convert_block_table(p_top_qmf, outfile);
	if(eError != NCS_SUCCESS)
		return(eError);

	/*
	** we have processed input, now flush the final lines and write the compressed file
	** To do this, we go through the levels, writing the level preamble,
	** then moving the sideband compressed file over to the output file.
	*/

	// Write each layer out
	p_qmf = p_top_qmf;
	while(p_qmf->p_larger_qmf) {	// we don't write the fake file QMF level
		eError = write_compressed_level(p_qmf, outfile);
		
		if(eError != NCS_SUCCESS)
			return(eError);
		p_qmf = p_qmf->p_larger_qmf;
	}

	return(eError);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
// This does a DWT for one line of this level, and returns the LL result
// to its parent, which called this level with p_parent_ll_line pointing to
// where to put the LL line.
//
// The LH,HL and HH lines for this level (except file level - see note 1) are
// also generated, and compressed and written out to the compressed file.
//
// So in effect one set of LL,LH,HL and HH lines are generated, with the LL
// line being returned to the parent, and the LH,HL and HH lines being
// consumed by the compressor during this line and then thrown away.
//
// In order to generate one set of LL,LH,HL and HH lines, two calls to
// the next larger level must be made requesting input. So the call recurses
// down the QMF tree.  Each level holds just enough intermediate lines to
// create one level line; the number of intermediate lines is FILTER_SIZE+1.
//
// So memory usage is quite small.
//
// Note (1) At the file level of the QMF tree, it does not generate LH
// HL or HH lines for that level of the tree; it just sets up the p_parent_ll_line
// for the parent level, which is read directly from disk
//
//
// Before returning the line requested (other than at file level),
// we have to pre-read a number of input lines, to handle vertical reflection.
// This is OK, as at the end, we don't do any calls to larger levels; we just fill
// line requests from our parent from the small FILTER_SIZE+1 buffer
// of lines we carry for this level.
//
// When returning from this call, the p_parent_ll_line pointer
// is filled with the requested line.  Before returning from this call,
// the output_[lh|hl|hh_lines are written out to the compressed file
// (high level recursive calls don't need those lines).  Once again,
// the file level does not create output_[lh|hl|hh_lines; it only reads the
// LL line and returns it to the parent.
//
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

static NCSError build_qmf_level_dwt_line(QmfLevelStruct *p_qmf, IEEE4 **p_p_parent_ll_lines)
{
	NCSError eError;
	UINT16	band;

	if( p_qmf->next_output_line >= p_qmf->y_size ) {
		return(NCS_GENERATED_TOO_MANY_OUTPUT_LINES);
	}

	// if file level, we just return the requested LL line to the parent
	if( !(p_qmf->p_larger_qmf) ) {
		if( p_qmf->pCompressionTask && p_qmf->pCompressionTask->pCancelCallback ) {
			BOOLEAN bCancel;
			bCancel = (p_qmf->pCompressionTask->pCancelCallback)(p_qmf->pCompressionTask->pClient);
			if( bCancel ) {
				/*[05]Replaced error with error value that does not include the text "ERS_ERROR"
				This is to give the user a more user friendly error message*/
				return(NCS_USER_CANCELLED_COMPRESSION);
			}
		}
		if(p_qmf->pCompressionTask && p_qmf->pCompressionTask->pStatusCallback ) {
			(p_qmf->pCompressionTask->pStatusCallback)(p_qmf->pCompressionTask->pClient, p_qmf->next_output_line);
		}
		if(p_qmf->pCompressionTask && p_qmf->pCompressionTask->pClient && p_qmf->pCompressionTask->pReadCallback) {
			NCSError eError;
			
			eError = (p_qmf->pCompressionTask->pReadCallback)(p_qmf->pCompressionTask->pClient, 
														  p_qmf->next_output_line, 
														  p_p_parent_ll_lines);
			
			if(eError != NCS_SUCCESS) {
				return(eError);
			}

			p_qmf->next_output_line += 1;
		}
		return(NCS_SUCCESS);
	}

	// to get to here, we must have a p_qmf->p_larger_qmf, so we can use it without checking

	// Recurse down the tree, reading input lines to generate
	// our single output line requested by our parent.

	// Have to pre-read enough input lines which, after reflection, enables
	// them to be used to generate the first of the output lines.
	// On each read, as always, we generate the lowpass & highpass lines
	//
	// We always generae the last of the lowpass/highpass lines, and roll
	// them up by one.

	if( !p_qmf->next_output_line ) {	// first call, so pre-read (FILTER_SIZE/2)+1 input lines and filter them
		while( p_qmf->next_input_line <= ((FILTER_SIZE / 2) + 1 ) ) {
			// roll the lowpass/highpass line array down. Just moves pointers, so is fast.
			// Technically we don't need to do this for the very first line, but no harm done
			build_qmf_level_roll_line_array(p_qmf);
			// gather an input line.  Must offset into our input line by reflection amount
			if( (eError = build_qmf_level_dwt_line(p_qmf->p_larger_qmf, p_qmf->p_p_input_ll_line)) != NCS_SUCCESS )
				return( eError );
			// generate this lowpass and highpass line pair from the input line
			build_qmf_level_lohi_line_pair(p_qmf);
			p_qmf->next_input_line += 1;
		}
		// now we have enough input lines to generate the first output line, we must
		// reflect the input lines. We memcopy() to do this. Although slow, this means
		// later we can just roll the lowpass/highpass line pointers for all lines,
		// so we only do FILTER_SIZE/2 memcopy()'s per level, making overall performance much faster
		{
			for( band = 0; band < p_qmf->nr_bands; band++ ) {
				QmfLevelBandStruct *p_band = p_qmf->p_bands + band;

				UINT32	reflect_line;
				for( reflect_line = 0; reflect_line < FILTER_SIZE/2; reflect_line++ ) {
					memcpy(p_band->p_p_lo_lines[(FILTER_SIZE/2) - reflect_line - 1],
								p_band->p_p_lo_lines[(FILTER_SIZE/2) + reflect_line + 1],
								p_qmf->x_size * sizeof(IEEE4));
					memcpy(p_band->p_p_hi_lines[(FILTER_SIZE/2) - reflect_line - 1],
								p_band->p_p_hi_lines[(FILTER_SIZE/2) + reflect_line + 1],
								p_qmf->x_size * sizeof(IEEE4));
				}
			}	/* end band loop */
		}
	}

	// Now read two input lines, and generate an output line. If the first
	// output line, we have already read enough input lines to generate output
	// (because of reflection, had to pre-read (FILTER_SIZE/2)+1 lines).
	// If at the end, must not read any more input lines; we just reflect the
	// lines we already have.

	// read lines if not at the very start
	if( p_qmf->next_output_line ) {
		UINT8	read_two;
		// We need to read two input lines - this is how we decimate input in the Y direction
		// We check input to start reflecting at end. This way it works well for odd or even
		// size input number of lines
		for( read_two = 0; read_two < 2; read_two++ ) {
			if( p_qmf->next_input_line < p_qmf->p_larger_qmf->y_size ) {
				// not at end of larger QMF, so OK to recurse down and read a line 
				// roll the lowpass/highpass line array down. Just moves pointers, so is fast.
				build_qmf_level_roll_line_array(p_qmf);
				// gather an input line.  Must offset into our input line by reflection amount
				if( (eError = build_qmf_level_dwt_line(p_qmf->p_larger_qmf, p_qmf->p_p_input_ll_line)) != NCS_SUCCESS )
					return( eError );
				// generate this lowpass and highpass line pair from the input line
				build_qmf_level_lohi_line_pair(p_qmf);
				p_qmf->next_input_line += 1;
			}
			else {
				int	reflect_line;
				// At end of larger QMF, so reflect the lines we already have
				// First roll the lohi lines so our filter line is in the middle.
				build_qmf_level_roll_line_array(p_qmf);
				// as the hilo lines are already built, we just need to reflect one line
				// To do this, work out our current position, and offset by that amount
				// Must be 2x jump each loop, as the roll above moved the center of reflection down
				reflect_line = (p_qmf->y_size * 2) - ((p_qmf->next_output_line*2) + read_two);
				reflect_line = (reflect_line * 2) - 1;
				for( band = 0; band < p_qmf->nr_bands; band++ ) {
					QmfLevelBandStruct *p_band = p_qmf->p_bands + band;

					if( reflect_line >= 0 && reflect_line <= FILTER_SIZE ) {	// sanity check it
						memcpy(p_band->p_p_lo_lines[FILTER_SIZE],
									p_band->p_p_lo_lines[reflect_line],
									p_qmf->x_size * sizeof(IEEE4));
						memcpy(p_band->p_p_hi_lines[FILTER_SIZE],
									p_band->p_p_hi_lines[reflect_line],
									p_qmf->x_size * sizeof(IEEE4));
					}
				} /* end band loop */
			}
		}
	}

	// We have generated the two additional low and highpass lines, which have
	// been added to our FILTER_SIZE+1 set of low and high pass lines. Now,
	// we generate the LL, LH, HL and HH lines from these. The LL line will
	// be generated to the buffer indicated by the parent call to this routine,
	// the LH,HL and HH lines will be created and then consumed by the encoder,
	// which is called by the lower level build routine
	eError = build_qmf_level_output_line(p_qmf, p_p_parent_ll_lines );
	
	if(eError != NCS_SUCCESS)
		return(eError);

	p_qmf->next_output_line += 1;
	return(NCS_SUCCESS);
}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	build_qmf_level_roll_line_array() updates the line array pointers so that
**	the current line is in the middle of the array (shuffles all towards [0]
**	by one.  There are FILTER_SIZE+1 lines, so we move p_p_[lo|hi]_line[0] up
**  to p_p_[lo|hi]_line[FILTER_SIZE]
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
static void build_qmf_level_roll_line_array(QmfLevelStruct *p_qmf)
{
	UINT16	band;
	for( band = 0; band < p_qmf->nr_bands; band++ ) {
		QmfLevelBandStruct *p_band = p_qmf->p_bands + band;

		IEEE4	*p_temp;
		UINT32	line;
		// there are FILTER_SIZE+1 lines to roll.
		p_temp = p_band->p_p_lo_lines[0];
		for(line = 1; line <= FILTER_SIZE; line ++)
			p_band->p_p_lo_lines[line-1] = p_band->p_p_lo_lines[line];
		p_band->p_p_lo_lines[FILTER_SIZE] = p_temp;

		p_temp = p_band->p_p_hi_lines[0];
		for(line = 1; line <= FILTER_SIZE; line ++)
			p_band->p_p_hi_lines[line-1] = p_band->p_p_hi_lines[line];
		p_band->p_p_hi_lines[FILTER_SIZE] = p_temp;
	}	/* end band loop */
}
/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Builds the p_p_lo_line[FILTER_SIZE] and the p_p_hi_line[FILTER_SIZE]
**	lines from the p_input_ll_line.
**
**	Input is subsampled. Logic is that the lowpass steps through very
**	N*2 inputs, then convolves FILTER_SIZE elements around that (step 1,
**	not 2, for the actual convolution). The highpass is similar, except
**	it steps through N*2+1. In otherwords, is offset by 1 element.
**
**	The filters are hardcoded as #defines, as this results in in-line
**	numbers to multiply by, which is much faster than reading the filters
**	from an array (although it requires a recompile to use different filters)
**
**	We have to set up reflection on the input line in the Y direction
**	before generating the output lines. Note that the input line
**	might not be have been twice our output size (for example larger
**	level might be 63 wide, so this level would be 32 wide). Because
**	of this, the right hand reflection might have to reflect one more
**	value.
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
static void build_qmf_level_lohi_line_pair(QmfLevelStruct *p_qmf)
{
	UINT32 band;
	UINT32 larger_x_size = p_qmf->p_larger_qmf->x_size;

	for( band = 0; band < p_qmf->nr_bands; band++ ) {
		QmfLevelBandStruct *p_band = p_qmf->p_bands + band;

		register IEEE4	*p_in_line;
		IEEE4			*p_temp;
		register		int i;

		p_in_line = p_band->p_input_ll_line;

		// reflect left edge. Always FILTER_SIZE/2 to reflect
		for(i = 0; i < FILTER_SIZE/2; i++)
			p_in_line[(FILTER_SIZE/2) -i - 1 /**[06]* - index was just i **/] =		// left edge
				p_in_line[((FILTER_SIZE/2) + 1) + i];
		// reflect right edge. Might have to reflect one more value if larger x_size != x_size*2
		p_temp = &p_in_line[(FILTER_SIZE/2) + (larger_x_size-1)];	// last valid value
		for(i = 0; i < ((FILTER_SIZE/2) + (int) ((p_qmf->x_size * 2) - larger_x_size)); i++ )
			p_temp[i + 1] = p_temp[-i - 1];

		// generate output using convolution for both lowpass and highpass
		// We do both filters at the same time. This ensures that input
		// will be in CPU cache, making operations much faster.
		{
			register IEEE4 *NCS_RESTRICT p_lo_line = p_band->p_p_lo_lines[FILTER_SIZE];
			register IEEE4 *NCS_RESTRICT p_hi_line = p_band->p_p_hi_lines[FILTER_SIZE];
			register IEEE4 *NCS_RESTRICT p_in = p_in_line;	// [07] restructured loops for performance
			int x_size = p_qmf->x_size;
//#ifdef NOTDEF				

//1.9MBs to 2.4MBs by removing write on P3 800
//3.5MBs to 6.3MBs by removing write on P4Xeon 2.4GHz!
//			i = p_qmf->x_size;
//			while( i-- ) {				// faster this way so can test against non-zero
#ifdef NCS_OPENMP
#pragma omp parallel for
#endif // NCS_OPENMP
			for(i = 0; i < x_size; i++) {				// faster this way so can test against non-zero
				// LO filter starts with even input, HI starts with odd input
				*p_lo_line++ = (*p_in + *(p_in + 10)) * LO_FILTER_0 +
						  (*(p_in + 1) + *(p_in + 9)) * LO_FILTER_1 +
						  (*(p_in + 2) + *(p_in + 8)) * LO_FILTER_2 +
						  (*(p_in + 3) + *(p_in + 7)) * LO_FILTER_3 +
						  (*(p_in + 4) + *(p_in + 6)) * LO_FILTER_4 +
										  *(p_in + 5) * LO_FILTER_5;
				
				*p_hi_line++ = (*(p_in + 1) + *(p_in + 11)) * HI_FILTER_0 +
							   (*(p_in + 2) + *(p_in + 10)) * HI_FILTER_1 +
							    (*(p_in + 3) + *(p_in + 9)) * HI_FILTER_2 +
							    (*(p_in + 4) + *(p_in + 8)) * HI_FILTER_3 +
							    (*(p_in + 5) + *(p_in + 7)) * HI_FILTER_4 +
												*(p_in + 6) * HI_FILTER_5;

				p_in += 2;
			}	/* end loop loop */
//#endif
		}
	}	/* end band loop */
}

/*
static const int CACHEBLOCK = 0x1000; // prefetch block size (4K bytes)
int p_fetch; // this "anchor" variable helps to
// fool the compiler’s optimizer
static void __inline BLOCK_PREFETCH_1K(void* addr) {
	int* a = (int*) addr; // cast as INT pointer for speed
	int i; 
	for(i = 192; i > 0; i -= 8) {
		p_fetch += a[i];
	}
}		
static void __inline BLOCK_PREFETCH_4K(void* addr) {
	int* a = (int*) addr; // cast as INT pointer for speed
	p_fetch += a[0] + a[16] + a[32] + a[48] // Grab every
	+ a[64] + a[80] + a[96] + a[112] // 64th address,
	+ a[128] + a[144] + a[160] + a[176] // to hit each
	+ a[192] + a[208] + a[224] + a[240]; // cache line once.
	a += 256; // advance to next 1K block
	p_fetch += a[0] + a[16] + a[32] + a[48]
	+ a[64] + a[80] + a[96] + a[112]
	+ a[128] + a[144] + a[160] + a[176]
	+ a[192] + a[208] + a[224] + a[240];
	a += 256;
	p_fetch += a[0] + a[16] + a[32] + a[48]
	+ a[64] + a[80] + a[96] + a[112]
	+ a[128] + a[144] + a[160] + a[176]
	+ a[192] + a[208] + a[224] + a[240];
	a += 256;
	p_fetch += a[0] + a[16] + a[32] + a[48]
	+ a[64] + a[80] + a[96] + a[112]
	+ a[128] + a[144] + a[160] + a[176]
	+ a[192] + a[208] + a[224] + a[240];
}	
*/

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	build_qmf_level_output_line().  Using the input low & highpass list of
**	lines, and the low and highpass filters, generate the LL, LH, HL and HH
**	output lines.
**
**	About the only interesting thing to note here is that we optimize filters,
**	and use interleaved access to lines to improve cache speed.
**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
static NCSError build_qmf_level_output_line(QmfLevelStruct *p_qmf, IEEE4 **p_p_ll_lines )
{
	UINT32	band;			// [03] VTune says to make this an int not short
#ifndef NCS_VECTOR_CC
	FLT_TO_INT_INIT();		
#endif //!NCS_VECTOR_CC
	for( band = 0; band < p_qmf->nr_bands; band++ ) {
		QmfLevelBandStruct *p_band = p_qmf->p_bands + band;
	
		// p_hh_line is handed to us, from parent level
		register IEEE4 *NCS_RESTRICT p_lo_line;
//		register IEEE4 *p_hi_line;
		register INT16 *NCS_RESTRICT p_lo_line16;
		register INT16 *NCS_RESTRICT p_hi_line16;
		register INT32 i;
		register	IEEE4	**p_p_in;
		INT32 x_size = p_qmf->x_size;
		/*
		**	Note that we create LL and LH centered on the middle of input lowpass & highpass
		**	array, and we create HL and HH centered on the middle+1 of the input. As the higher
		**	routine reads two inputs for each call to this routine, this decimates
		**	input in the Y direction.
		*/
		register IEEE4 fBinSize = 1.0f / p_qmf->p_band_bin_size[band];	// [03]
		// generate output LL and LH from lowpass input using convolution. Do this, then highpass
		// input as a separate run, to improve cache hits

		register IEEE4 *NCS_RESTRICT p_i_0, *NCS_RESTRICT p_i_1, *NCS_RESTRICT p_i_2, *NCS_RESTRICT p_i_3, 
					   *NCS_RESTRICT p_i_4, *NCS_RESTRICT p_i_5, *NCS_RESTRICT p_i_6, *NCS_RESTRICT p_i_7, 
					   *NCS_RESTRICT p_i_8, *NCS_RESTRICT p_i_9, *NCS_RESTRICT p_i_10,*NCS_RESTRICT p_i_11;
		register IEEE4 f1;
		register INT32 n1;

		p_p_in = p_band->p_p_lo_lines;
		p_i_0 = p_p_in[0];	p_i_1 = p_p_in[1];	p_i_2 = p_p_in[2];	p_i_3 = p_p_in[3];
		p_i_4 = p_p_in[4];	p_i_5 = p_p_in[5];	p_i_6 = p_p_in[6];	p_i_7 = p_p_in[7];
		p_i_8 = p_p_in[8];	p_i_9 = p_p_in[9];	p_i_10 = p_p_in[10];p_i_11 = p_p_in[11];

		p_lo_line = p_p_ll_lines[band];
//		p_hi_line = p_band->p_output_lh_line;

		if(p_qmf->p_file_qmf->bLowMemCompress) {
			p_lo_line16 = (INT16*)p_p_ll_lines[band];
		} else {
			p_lo_line16 = p_band->p_quantized_output_ll_block;
		}
		p_hi_line16 = p_band->p_quantized_output_lh_block;
		if(!p_qmf->p_file_qmf->bLowMemCompress) {
			p_lo_line16 += p_qmf->x_size * p_qmf->next_output_block_y_line;		
			p_hi_line16 += p_qmf->x_size * p_qmf->next_output_block_y_line;		
		}
		//p_qmf->x_size;
		if(p_qmf->level != 0) {
#ifdef NCS_OPENMP
#pragma omp parallel for private (f1,n1)
#endif // NCS_OPENMP
/*			while(x_size > 256) {
				BLOCK_PREFETCH_1K(p_i_0);
				BLOCK_PREFETCH_1K(p_i_1);
				BLOCK_PREFETCH_1K(p_i_2);
				BLOCK_PREFETCH_1K(p_i_3);
				BLOCK_PREFETCH_1K(p_i_4);
				BLOCK_PREFETCH_1K(p_i_5);
				BLOCK_PREFETCH_1K(p_i_6);
				BLOCK_PREFETCH_1K(p_i_7);
				BLOCK_PREFETCH_1K(p_i_8);
				BLOCK_PREFETCH_1K(p_i_9);
				BLOCK_PREFETCH_1K(p_i_10);
				BLOCK_PREFETCH_1K(p_i_11);
				for(i = 0; i < 256; i++) {	// LOWPASS:	Generate LL and LH lines
						// LO filter starts with even input, HI starts with odd input
					*p_lo_line++ = (*p_i_0 + *p_i_10) * LO_FILTER_0 + 
								   (*p_i_1 + *p_i_9) * LO_FILTER_1 + 
								   (*p_i_2 + *p_i_8) * LO_FILTER_2 + 
								   (*p_i_3 + *p_i_7) * LO_FILTER_3 + 
								   (*p_i_4 + *p_i_6) * LO_FILTER_4 + 
								   *p_i_5 * LO_FILTER_5;
					f1 = ((*p_i_1 + *p_i_11) * HI_FILTER_0 + 
						  (*p_i_2 + *p_i_10) * HI_FILTER_1 + 
						  (*p_i_3 + *p_i_9) * HI_FILTER_2 + 
						  (*p_i_4 + *p_i_8) * HI_FILTER_3 +
						  (*p_i_5 + *p_i_7) * HI_FILTER_4 + 
						   *p_i_6 * HI_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
					*p_hi_line16++ = (INT16)(f1 + 0.5);
#else
					FLT_TO_INT32(n1,f1);
					*p_hi_line16++ = (INT16)n1;
#endif
					++p_i_0; ++p_i_1; ++p_i_2; ++p_i_3; ++p_i_4; ++p_i_5; ++p_i_6; ++p_i_7; ++p_i_8; ++p_i_9; ++p_i_10; ++p_i_11;
				}
				x_size -= 256;
			}*/
			for(i = 0; i < x_size; i++) {	// LOWPASS:	Generate LL and LH lines
					// LO filter starts with even input, HI starts with odd input
				*p_lo_line++ = (*p_i_0 + *p_i_10) * LO_FILTER_0 + 
							   (*p_i_1 + *p_i_9) * LO_FILTER_1 + 
							   (*p_i_2 + *p_i_8) * LO_FILTER_2 + 
							   (*p_i_3 + *p_i_7) * LO_FILTER_3 + 
							   (*p_i_4 + *p_i_6) * LO_FILTER_4 + 
							   *p_i_5 * LO_FILTER_5;
				f1 = ((*p_i_1 + *p_i_11) * HI_FILTER_0 + 
					  (*p_i_2 + *p_i_10) * HI_FILTER_1 + 
					  (*p_i_3 + *p_i_9) * HI_FILTER_2 + 
					  (*p_i_4 + *p_i_8) * HI_FILTER_3 +
					  (*p_i_5 + *p_i_7) * HI_FILTER_4 + 
					   *p_i_6 * HI_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
				*p_hi_line16++ = (INT16)(f1 + 0.5);
#else
				FLT_TO_INT32(n1,f1);
				*p_hi_line16++ = (INT16)n1;
#endif
				++p_i_0; ++p_i_1; ++p_i_2; ++p_i_3; ++p_i_4; ++p_i_5; ++p_i_6; ++p_i_7; ++p_i_8; ++p_i_9; ++p_i_10; ++p_i_11;
			}
			x_size = p_qmf->x_size;
		} else {
#ifdef NCS_OPENMP
#pragma omp parallel for private (f1,n1)
#endif // NCS_OPENMP
			for(i = 0; i < x_size; i++) {	// HIGPASS:	Generate HL and HH lines
				// LO filter starts with even input, HI starts with odd input
				f1 = ((*p_i_0 + *p_i_10) * LO_FILTER_0 + 
					  (*p_i_1 + *p_i_9) * LO_FILTER_1 + 
					  (*p_i_2 + *p_i_8) * LO_FILTER_2 + 
					  (*p_i_3 + *p_i_7) * LO_FILTER_3 +
					  (*p_i_4 + *p_i_6) * LO_FILTER_4 + 
					   *p_i_5 * LO_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
				*p_lo_line16++ = (INT16)(f1 + 0.5);
#else
				FLT_TO_INT32(n1,f1);
				*p_lo_line16++ = (INT16)n1;
#endif
				f1 = ((*p_i_1 + *p_i_11) * HI_FILTER_0 + 
					  (*p_i_2 + *p_i_10) * HI_FILTER_1 + 
					  (*p_i_3 + *p_i_9) * HI_FILTER_2 + 
					  (*p_i_4 + *p_i_8) * HI_FILTER_3 +
					  (*p_i_5 + *p_i_7) * HI_FILTER_4 + 
					   *p_i_6 * HI_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
				*p_hi_line16++ = (INT16)(f1 + 0.5);
#else
				FLT_TO_INT32(n1,f1);
				*p_hi_line16++ = (INT16)n1;
#endif
				++p_i_0; ++p_i_1; ++p_i_2; ++p_i_3; ++p_i_4; ++p_i_5; ++p_i_6; ++p_i_7; ++p_i_8; ++p_i_9; ++p_i_10; ++p_i_11;
			}
		}
		// generate output HL and HH from highpass input using convolution.
		p_p_in = p_band->p_p_hi_lines;
		p_i_0 = p_p_in[0];	p_i_1 = p_p_in[1];	p_i_2 = p_p_in[2];	p_i_3 = p_p_in[3];
		p_i_4 = p_p_in[4];	p_i_5 = p_p_in[5];	p_i_6 = p_p_in[6];	p_i_7 = p_p_in[7];
		p_i_8 = p_p_in[8];	p_i_9 = p_p_in[9];	p_i_10 = p_p_in[10];p_i_11 = p_p_in[11];

		p_lo_line16 = p_band->p_quantized_output_hl_block;
		p_hi_line16 = p_band->p_quantized_output_hh_block;
		if(!p_qmf->p_file_qmf->bLowMemCompress) {//[05]
			p_lo_line16 += p_qmf->x_size * p_qmf->next_output_block_y_line;		
			p_hi_line16 += p_qmf->x_size * p_qmf->next_output_block_y_line;		
		}
#ifdef NCS_OPENMP
#pragma omp parallel for private (f1,n1)
#endif // NCS_OPENMP
/*		while(x_size > 256) {
			BLOCK_PREFETCH_1K(p_i_0);
			BLOCK_PREFETCH_1K(p_i_1);
			BLOCK_PREFETCH_1K(p_i_2);
			BLOCK_PREFETCH_1K(p_i_3);
			BLOCK_PREFETCH_1K(p_i_4);
			BLOCK_PREFETCH_1K(p_i_5);
			BLOCK_PREFETCH_1K(p_i_6);
			BLOCK_PREFETCH_1K(p_i_7);
			BLOCK_PREFETCH_1K(p_i_8);
			BLOCK_PREFETCH_1K(p_i_9);
			BLOCK_PREFETCH_1K(p_i_10);
			BLOCK_PREFETCH_1K(p_i_11);

			for(i = 0; i < 256; i++) {
				// LO filter starts with even input, HI starts with odd input
				f1 = ((*p_i_0 + *p_i_10) * LO_FILTER_0 + 
					  (*p_i_1 + *p_i_9) * LO_FILTER_1 + 
					  (*p_i_2 + *p_i_8) * LO_FILTER_2 + 
					  (*p_i_3 + *p_i_7) * LO_FILTER_3 +
					  (*p_i_4 + *p_i_6) * LO_FILTER_4 + 
					   *p_i_5 * LO_FILTER_5) * fBinSize;
	#ifdef NCS_VECTOR_CC
				*p_lo_line16++ = (INT16)(f1 + 0.5);
	#else
				FLT_TO_INT32(n1,f1);
				*p_lo_line16++ = (INT16)n1;
	#endif
				f1 = ((*p_i_1 + *p_i_11) * HI_FILTER_0 + 
					  (*p_i_2 + *p_i_10) * HI_FILTER_1 + 
					  (*p_i_3 + *p_i_9) * HI_FILTER_2 + 
					  (*p_i_4 + *p_i_8) * HI_FILTER_3 +
					  (*p_i_5 + *p_i_7) * HI_FILTER_4 + 
					   *p_i_6 * HI_FILTER_5) * fBinSize;
	#ifdef NCS_VECTOR_CC
				*p_hi_line16++ = (INT16)(f1 + 0.5);
	#else
				FLT_TO_INT32(n1,f1);
				*p_hi_line16++ = (INT16)n1;
	#endif
				++p_i_0; ++p_i_1; ++p_i_2; ++p_i_3; ++p_i_4; ++p_i_5; ++p_i_6; ++p_i_7; ++p_i_8; ++p_i_9; ++p_i_10; ++p_i_11;
			}
			x_size -= 256;
		}*/
		for(i = 0; i < x_size; i++) {	// HIGPASS:	Generate HL and HH lines
			// LO filter starts with even input, HI starts with odd input
			f1 = ((*p_i_0 + *p_i_10) * LO_FILTER_0 + 
				  (*p_i_1 + *p_i_9) * LO_FILTER_1 + 
				  (*p_i_2 + *p_i_8) * LO_FILTER_2 + 
				  (*p_i_3 + *p_i_7) * LO_FILTER_3 +
				  (*p_i_4 + *p_i_6) * LO_FILTER_4 + 
				   *p_i_5 * LO_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
			*p_lo_line16++ = (INT16)(f1 + 0.5);
#else
			FLT_TO_INT32(n1,f1);
			*p_lo_line16++ = (INT16)n1;
#endif
			f1 = ((*p_i_1 + *p_i_11) * HI_FILTER_0 + 
				  (*p_i_2 + *p_i_10) * HI_FILTER_1 + 
				  (*p_i_3 + *p_i_9) * HI_FILTER_2 + 
				  (*p_i_4 + *p_i_8) * HI_FILTER_3 +
				  (*p_i_5 + *p_i_7) * HI_FILTER_4 + 
				   *p_i_6 * HI_FILTER_5) * fBinSize;
#ifdef NCS_VECTOR_CC
			*p_hi_line16++ = (INT16)(f1 + 0.5);
#else
			FLT_TO_INT32(n1,f1);
			*p_hi_line16++ = (INT16)n1;
#endif

/*
			*p_lo_line++ = *p_i_0 * LO_FILTER_0 + *p_i_1 * LO_FILTER_1 + *p_i_2 * LO_FILTER_2 + *p_i_3 * LO_FILTER_3
						 + *p_i_4 * LO_FILTER_4 + *p_i_5 * LO_FILTER_5 + *p_i_6 * LO_FILTER_6 + *p_i_7 * LO_FILTER_7
						 + *p_i_8 * LO_FILTER_8 + *p_i_9 * LO_FILTER_9 + *p_i_10 * LO_FILTER_10;

			*p_hi_line++ = *p_i_1 * HI_FILTER_0 + *p_i_2 * HI_FILTER_1 + *p_i_3 * HI_FILTER_2 + *p_i_4 * HI_FILTER_3
						 + *p_i_5 * HI_FILTER_4 + *p_i_6 * HI_FILTER_5 + *p_i_7 * HI_FILTER_6 + *p_i_8 * HI_FILTER_7
						 + *p_i_9 * HI_FILTER_8 + *p_i_10 * HI_FILTER_9 + *p_i_11 * HI_FILTER_10;
*/
			++p_i_0; ++p_i_1; ++p_i_2; ++p_i_3; ++p_i_4; ++p_i_5; ++p_i_6; ++p_i_7; ++p_i_8; ++p_i_9; ++p_i_10; ++p_i_11;
		}
			x_size = p_qmf->x_size;

#ifdef NEVER_OLD_CODE_PRE_07
		p_lo_line = p_p_ll_lines[band];
		p_hi_line = p_band->p_output_lh_line;
		for( i=0; i < p_qmf->x_size; i++ ) {
			// Generate LL and LH lines
			register	IEEE4	**p_p_in = p_band->p_p_lo_lines;

			// LO filter starts with even input, HI starts with odd input
			lo_result  = p_p_in[0][i]  * LO_FILTER_0;
			hi_result  = p_p_in[1][i]  * HI_FILTER_0;
			lo_result += p_p_in[1][i]  * LO_FILTER_1;
			hi_result += p_p_in[2][i]  * HI_FILTER_1;
			lo_result += p_p_in[2][i]  * LO_FILTER_2;
			hi_result += p_p_in[3][i]  * HI_FILTER_2;
			lo_result += p_p_in[3][i]  * LO_FILTER_3;
			hi_result += p_p_in[4][i]  * HI_FILTER_3;
			lo_result += p_p_in[4][i]  * LO_FILTER_4;
			hi_result += p_p_in[5][i]  * HI_FILTER_4;
			lo_result += p_p_in[5][i]  * LO_FILTER_5;
			hi_result += p_p_in[6][i]  * HI_FILTER_5;
			lo_result += p_p_in[6][i]  * LO_FILTER_6;
			hi_result += p_p_in[7][i]  * HI_FILTER_6;
			lo_result += p_p_in[7][i]  * LO_FILTER_7;
			hi_result += p_p_in[8][i]  * HI_FILTER_7;
			lo_result += p_p_in[8][i]  * LO_FILTER_8;
			hi_result += p_p_in[9][i]  * HI_FILTER_8;
			lo_result += p_p_in[9][i]  * LO_FILTER_9;
			hi_result += p_p_in[10][i] * HI_FILTER_9;
			lo_result += p_p_in[10][i] * LO_FILTER_10;
			hi_result += p_p_in[11][i] * HI_FILTER_10;

			*p_lo_line++ = lo_result;
			*p_hi_line++ = hi_result;
		}

		// generate output HL and HH from highpass input using convolution.
		p_lo_line = p_band->p_output_hl_line;
		p_hi_line = p_band->p_output_hh_line;

		for( i=0; i < p_qmf->x_size; i++ ) {
			// Generate LL and LH lines
			register	IEEE4	**p_p_in = p_band->p_p_hi_lines;

			// LO filter starts with even input, HI starts with odd input
			lo_result  = p_p_in[0][i]  * LO_FILTER_0;
			hi_result  = p_p_in[1][i]  * HI_FILTER_0;
			lo_result += p_p_in[1][i]  * LO_FILTER_1;
			hi_result += p_p_in[2][i]  * HI_FILTER_1;
			lo_result += p_p_in[2][i]  * LO_FILTER_2;
			hi_result += p_p_in[3][i]  * HI_FILTER_2;
			lo_result += p_p_in[3][i]  * LO_FILTER_3;
			hi_result += p_p_in[4][i]  * HI_FILTER_3;
			lo_result += p_p_in[4][i]  * LO_FILTER_4;
			hi_result += p_p_in[5][i]  * HI_FILTER_4;
			lo_result += p_p_in[5][i]  * LO_FILTER_5;
			hi_result += p_p_in[6][i]  * HI_FILTER_5;
			lo_result += p_p_in[6][i]  * LO_FILTER_6;
			hi_result += p_p_in[7][i]  * HI_FILTER_6;
			lo_result += p_p_in[7][i]  * LO_FILTER_7;
			hi_result += p_p_in[8][i]  * HI_FILTER_7;
			lo_result += p_p_in[8][i]  * LO_FILTER_8;
			hi_result += p_p_in[9][i]  * HI_FILTER_8;
			lo_result += p_p_in[9][i]  * LO_FILTER_9;
			hi_result += p_p_in[10][i] * HI_FILTER_9;
			lo_result += p_p_in[10][i] * LO_FILTER_10;
			hi_result += p_p_in[11][i] * HI_FILTER_10;

			*p_lo_line++ = lo_result;
			*p_hi_line++ = hi_result;
		}
#endif
	
	}	/* end band loop */
#ifndef NCS_VECTOR_CC
	FLT_TO_INT_FINI();
#endif //!NCS_VECTOR_CC
	// write output lines
	return(build_qmf_level_qencode_line(p_qmf, p_qmf->next_output_line, p_p_ll_lines));
}
