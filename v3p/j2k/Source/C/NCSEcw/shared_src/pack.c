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
** FILE:   	pack.c	- packs (encodes) quantized data stream
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:
**
**	Packs (encode) a stream of data.
**	There is one visible routines for packing entire blocks:
**
**		int	pack_data(UINT8 **p_packed,	UINT32 *packed_length,
**					  UINT8  *p_raw,	UINT32 raw_length, UINT8 symbol_size_hint);
**
**	NOTE: This code is set up to encode blocks, but can easily be modified to handle
**	streaming input (not blocked).  This is actually faster than blocking input (which
**	is handle for random access but not much else).
**	If you do stream input, then you should just use the range encoders, and restart
**	them every 2^23 bytes or so.
**
**	Back routine returns non-zero if there was an error, in which case
**	no memory will have been allocated (so don't free it).
**	Raw_length MUST be a multiple of symbol_size_hint
**
**	pack_data() packs the raw data, which is raw_length (in bytes)
**	in length.
**	[06] if *p_packed == NULL, it returns a pointer to a newly allocated array of data p_packed,
**	which is packed_length in bytes, which must be freed by the caller once usage
**	is complete. If *p_packed is NOT null, it is assumed that the caller has pre-allocated
**	the memory required.
**  The caller can supply symbol_size_hint as a hint to the encoder
**	as to the natural length of each symbol in the raw data (for example sizeof(UINT16)),
**	and must be between 1 and 16 inclusive.
**	The returned data will be packed by one of a variety of techniques (the packer
**	selects the best method).  In the worst case, the returned data will never be
**	longer than raw_length+1 in size (which is no packing could be done - one byte is
**	added to indicate the packing technique used). You must pass at least one byte to
**	be packed.
**	It is recommended that compression size is kept under 64K for efficient packing
**
**	NOTES:
**	(1)	You can only compress 2^32 bytes of data in a single hit. If you need to
**		compress more than this, you will need to call the pack_data() routine
**		multiple times.
**
**	(2)	The pack_data() routine tries multiple encoding/compression techniques
**		to decide which is the best for the data. Thus, it is fairly slow to
**		compress, but decompress is fast.
**		
**	Note that in all cases:
**	(1)	Lengths are handed to these routines in bytes, not symbol lengths
**
**	ENCODING NOTES:
**	1) This uses the range encoding technique, with permission, from Michael Schindler
**
**
** EDITS:
** [01] sns	05oct98	Split out from quantize.c
** [02] sns	15oct98 Moved range & qsmodel code into this source file for optimization speed
** [03] sns	07mar99	No longer doing dec_normalize in done_decoding, as not need in read from memory instead of file
** [04] sns 07mar99 Converted dec_normalize to a macro as in-line optimization is not picking it up
** [05] sns 16Mar99 added "COMPRESS" compile definition to compile either pack or unpack logic
** [06] sns 19May99 unpack_data() can now be called with a pre-allocated memory array
** [07] sns 21May99 Adding additional signed range encoding techniques
** [08] sns 26May99 Not zero encoding if +/-1 -- now only zero encoding for true zeros
** [09] sns 02Apr99 Split unpack logic out to unpack.c
** [10] sns 03Apr99 Adding RangeZero encoding
** [11] sns 09Apr99 Adding Huffman encoding
** [12] dmm 14Jun99 fix overrun in worst case huffman encoding
** [13] sns 11May00	More performance tuning - don't bother with range 8 encoder etc
 *******************************************************/


#include "NCSEcw.h"
#include "qsmodel.h"
#include "rangecod.h"
#include "NCSHuffmanCoder.h"

#include "rangecode.c"		/* IMPORTANT!! Include this in-line for performance */

//#define ENCODE_RANGE_ONLY // ONLY use ENCODE_RANGE
#define ECW_ENCODE_RANGE
#define ECW_ENCODE_ZEROS
#define ECW_ENCODE_RUN_ZERO
#define ECW_HUFFMAN_ENCODE
// #define ECW_8BITRANGE_ENCODE	// [13] don't bother with 8bit range encoder, as only decreases file size by 0.1% or so

#define MAX_RANGE	256		/* number of symbols used in range encoding */

#define MAX_INT8	 127	/* used for difference encoding */
#define	MIN_INT8	-127
#define	MAX_INT16	 32767
#define	MIN_INT16	-32767


/*
**	pack_data() packs the raw data, which is raw_length (in bytes)
**
**	See file header for full details
**
*/


NCSError	pack_data(QmfLevelStruct *p_top_qmf,
			  UINT8 **p_packed,	UINT32 *packed_length,
			  UINT8 *p_raw,		UINT32 raw_length, UINT8 symbol_size_hint, BOOLEAN bTryHuffman)
{
	register	UINT8 *p_try_packed;
	UINT8		*pRunPacked = NULL;		// pointer to Run Zero or Huffman packed data, if any
	EncodeFormat	encode_format;
	p_top_qmf;// Keep Compiler Happy
	// must have enough space for partial overrun during compression attempt
	// With range encoding, we need 5 + 1 + 3, so 20 is safe. We don't ever
	// write this extra to disk, of course.
	p_try_packed = (UINT8 *) NCSMalloc(raw_length * 2 + 20, FALSE);
	if( !p_try_packed )
		return(NCS_COULDNT_ALLOC_MEMORY);

#ifdef NEVER_PERFORMANCE_TESTING
	*packed_length = 1;
	*p_packed = p_try_packed;
	return(NCS_SUCCESS);
#endif

	*packed_length  = raw_length;

	encode_format	= ENCODE_RANGE;
//	encode_format	= ENCODE_RAW;	// DEBUG TEST - set this to force no packing

	//
	// Try range encoding
	//	For 16 bit symbols only:
	//	First try difference encoding with 8 bit values. If we can fit them
	//	into 8 bits, we use this instead, as it is faster and always more
	//	efficient than straight range encoding. There is no point doing 16 bit
	//	range - it is less efficient that normal 16 bit range encoding.
	//	We encode a start 16 bit value, then ALL the values (including for the first
	//	symbol - this value will therefor always be zero). We do this as it makes
	//	line by line decoding much simpler.
	//	At the end of encoding, if we note that all values were zero,
	//	we just encode the block as ZERO's. This will result in much faster decode.
	//
	//	[10] If ZERO encoding failed, and after trying RANGE8 encoding, try RUN_ZERO or HUFFMAN,
	//	and pick the best (=shortest).  Then try RANGE8 encoding only if RANGE8 failed (so still
	//	try it if RANGE8 failed but RUN_ZERO or HUFFMAN generated reasonable results.
	//
	//	Failing any of the above (or not 16 bit symbols), straight range encoding is tried.
	//
	if( encode_format == ENCODE_RANGE ) {	// try range encoding
		register	UINT8 *p_try_raw;
		register	UINT32 count;

		// range encoder related
		register	int syfreq;
		int ltfreq;
		rangecoder rc;
		qsmodel qsm;

		// start encoding
#ifndef ENCODE_RANGE_ONLY
		// try difference encoding (order 1) encoding
		// We always encode the first value as 16 bit, so we need at least 2 symbols to work with
		if( symbol_size_hint == 2 && raw_length >= 4) {
			register	INT8	*p_diff, *p_try_diff;
			register	INT32	prev;
			register	UINT8	ok_zeros;
			register	UINT8	ok_8bit;
			register	INT16	*p_try_raw16;

			p_diff = p_try_diff = (INT8 *) NCSMalloc(raw_length*2 + 20, FALSE);
			if( !p_diff ) {
				NCS_SAFE_FREE(p_try_packed);
				return(NCS_COULDNT_ALLOC_MEMORY);
			}
			p_try_raw16 = (INT16 *) p_raw;
			count = raw_length / 2;		// we want nr. symbols, not data length in bytes
#ifdef ECW_ENCODE_ZEROS
			ok_zeros = TRUE;
#else
			ok_zeros = FALSE;
#endif
			ok_8bit = TRUE;
			prev = *p_try_raw16;
			if( prev < -1 || prev > 1 )
				ok_zeros = FALSE;		// not a zero block

			// try and 8 bit difference encode
			while(count--) {
				register	INT32	symbol;
				symbol = *p_try_raw16++;	// note that first value is always zero - this is OK
				if( (symbol - prev) > MAX_INT8
				|| (symbol - prev) < MIN_INT8 ) {
						ok_zeros = FALSE;
						ok_8bit = FALSE;
						break;			// can't do 8 bit range packing, so abort
				}
				if( symbol )			// [08] zero encode for true zeros only
					ok_zeros = FALSE;	// can't do zero's, but try 8 bit differencing

				*p_try_diff++ = (INT8) (symbol - prev);
				prev = symbol;
			}

			if( ok_zeros ) {
				// encode the block by marketing that it was all zeros. So we don't write
				// anything (other than the encode_format value)
				encode_format = ENCODE_ZEROS;
				*packed_length = 0;		// well, that was simple.
			}
#ifdef ECW_8BITRANGE_ENCODE		// [13]
			else if( ok_8bit ) {
				// it was OK, so go ahead and encode using this. We have to re-fetch the first symbol
				// from raw, and write it as a 16 bit symbol, then a 0 value for the first difference
				register	int ch;
				encode_format = ENCODE_RANGE8;
				p_try_raw16 = (INT16 *) p_raw;	// need the first value again

				count = raw_length / 2;		// we want nr. symbols, not data length in bytes
				p_try_diff = p_diff;
				prev = *p_try_raw16;		// write 1st symbol as 16 bit

				rc.p_packed = p_try_packed + 1;		// must leave room for encoding type symbol
				initqsmodel(&qsm,257,12,2000,NULL,1);
				start_encoding(&rc,0);

#if defined(NCSBO_MSBFIRST) && 0 // MDT -- removed from Mac build due to lack of time- Huffman/zero/raw supported
				//	This test is due to only 2 entry paths into the pack_data routine.  
				//	The preamble handles the swapping, the qencode_qmf_level_output_block doesn't.
				//	in the future handle other cases.
				//	
				if(2 == symbol_size_hint)
					NCSByteSwap16( prev );
#endif
				// top 8 bits 1st symbol
				ch = (int)  ((prev >> 8) & 0xff);
				syfreq = qsgetfreq(&qsm,ch,&ltfreq);
				encode_shift(&rc,syfreq,ltfreq,12);
				qsupdate(&qsm,ch);

				// bottom 8 bits 1st symbol
				ch = (int)  (prev & 0xff);
				syfreq = qsgetfreq(&qsm,ch,&ltfreq);
				encode_shift(&rc,syfreq,ltfreq,12);
				qsupdate(&qsm,ch);

#if defined(NCSBO_MSBFIRST) && 0 // MDT -- removed from Mac build due to lack of time- Huffman/zero/raw supported
				//	This test is due to only 2 entry paths into the pack_data routine.  
				//	The preamble handles the swapping, the qencode_qmf_level_output_block doesn't.
				//	in the future handle other cases.
				//	
				if(2 == symbol_size_hint)
					NCSByteSwap16( prev );	// put back
#endif	
				// now write the 8 bit difference encoded symbols. Note that the first value
				// WILL get written, even though it is always zero by definition. This is so
				// that decoding becomes a lot simpler
				while(count--) {
					if( rc.bytecount >= raw_length) {	// ran out of room. Try normal range encoding
						encode_format = ENCODE_RANGE;
						done_encoding(&rc);
						deleteqsmodel(&qsm);
						break;		// abort the compression
					}

					ch = (int)  ((*p_try_diff++) & 0xff);

					syfreq = qsgetfreq(&qsm,ch,&ltfreq);
					encode_shift(&rc,syfreq,ltfreq,12);
					qsupdate(&qsm,ch);
				}

				// write 256 as end of stream marker
				syfreq = qsgetfreq(&qsm,256,&ltfreq);
				encode_shift(&rc,syfreq,ltfreq,12);
				done_encoding(&rc);
				*packed_length = rc.bytecount;
				deleteqsmodel(&qsm);
				*packed_length = rc.bytecount;
			}
#endif				// [13]

			/*
			** [10] At this point, unless runlength-zero-encoding worked, we try zero range encoding,
			**		by itself, and we try runlength-zero-encoding *and* huffman. If the encoding
			**		format was ENCODE_RANGE8, and we did better than it did, we swap over to
			**		ENCODE_HUFFMAN or ENCODE_RUNZERO as our prefered approach. We then also
			**		try range encoding (except if our preferred approach is still ENCODE_RANGE8)
			**		and if that beats the ENCODE_HUFFMAN or ENCODE_RUNZERO, then we use that
			**		as the encoding technique.  Somewhat slow, as we are trying multiple
			**		techniques, but it is worth it as we get better compression, and much
			**		faster decompression.
			**		Conceptually, we could try doing a RUNZERO on the RANGE8 p_diff buffer,
			**		but I don't think it would add much (if anything) compared to a standard
			**		huffman, so we just work off the original data.
			**		NOTE: The Range Encoder can only handle 15 bits of data. The Huffman encoder
			**		can only handle 14 bits of data.  So we clip data to this range (it should never
			**		exceed this range anyway, if all went well at higher level quantizer routines).
			*/
			if( encode_format != ENCODE_ZEROS ) {
				// True RUNZERO *and* HUFFMAN. Use the best of these if it was better than RANGE8
				register UINT16	*pTryPacked = (UINT16 *) p_diff;		// can use the difference buffer, as RANGE8 is done with that buffer
				register INT16	*pTryRaw = (INT16 *) p_raw;				// cast input to 16 bit values
				register UINT32 nRawLength;
				register UINT16 nZeroCount;

				nRawLength = raw_length / 2;
				nZeroCount = 0;
				while( nRawLength-- ) {
					register INT16	nValue = *pTryRaw++;
					if( nValue ) {
						// flush any outstanding zero's, and pack and output this value
						if( nZeroCount ) {
							// flush and reset zero count to zero
							*pTryPacked++ = nZeroCount | RUN_MASK;
							nZeroCount = 0;
						}
						// output value with adjusted sign bit
						if( nValue < 0 ) {
							if( nValue <= MIN_BIN_VALUE )
								*pTryPacked++ = ((UINT16)-(MIN_BIN_VALUE)) | SIGN_MASK;
							else
								*pTryPacked++ = ((UINT16) (-nValue)) | SIGN_MASK;
						}
						else {
							if( nValue >= MAX_BIN_VALUE )
								*pTryPacked++ = MAX_BIN_VALUE;
							else
								*pTryPacked++ = (UINT16) nValue;
						}
					}
					else {
						// Increment zero count, and flush if equal to maximum zero length
						nZeroCount += 1;
						if( nZeroCount == MAX_RUN_LENGTH ) {
							// flush and reset zero count to zero
							*pTryPacked++ = nZeroCount | RUN_MASK;
							nZeroCount = 0;
						}
					}
				}
				// Flush trailing zeros
				if( nZeroCount ) {
					// flush and reset zero count to zero
					*pTryPacked++ = nZeroCount | RUN_MASK;
					nZeroCount = 0;
				}
#if defined(NCSBO_MSBFIRST) && 0 // need to verify that this code does work 
				//
				//	Swap UINT16 Stream here
				//
				if(2 == symbol_size_hint)
					{
					UINT32	cbPacked = (((UINT8 *)pTryPacked) - ((UINT8 *) p_diff));
					UINT32	c;
					UINT16	*pSwapped2;
					
					pSwapped2 = (UINT16*)(p_diff);
					for(c=0;c<cbPacked;c+=2)
						{
						*pSwapped2++ = NCSByteSwap16( *pSwapped2++ ); 
						}
					}
#endif
				// Now compute length of zero run length array. We do this last,
				// to save having to keep a length counter during zero packing
				// If less than RANGE or RANGE8, set up the packed array.
				// [11] Also try huffman encoding
				{
					UINT32	nPackedLength;
					UINT8	*pEndPacked = (UINT8 *) pTryPacked;		// convert to UINT8 ptr for offset calculcation

					nPackedLength = (UINT32)(pEndPacked - ((UINT8 *) p_diff));
#ifdef ECW_ENCODE_RUN_ZERO
					if( *packed_length > nPackedLength ) {
						// Well, Run Zero was shorter...
						encode_format = ENCODE_RUN_ZERO;
						*packed_length = nPackedLength;
						pRunPacked = (UINT8*)p_diff;						// Used to flag to keep this data for a little longer
					}
#endif //ECW_ENCODE_RUN_ZERO
#ifdef ECW_HUFFMAN_ENCODE
					if(bTryHuffman) {
						UINT8	*pHuffman;
						UINT32	nHuffmanLength;
						// Now see if Huffman was any better.

						// [12] huffman_encode docs say "encoded_stream size should be twice as large
						// as symbol stream size to be safe", but we found a case where there was a 3
						// byte overrun, so make a buffer twice the size + 16 for some extra padding.
						pHuffman = (UINT8 *) NCSMalloc(raw_length * 4 + 16, FALSE);	// [12]
						if( !pHuffman )
							return(NCS_COULDNT_ALLOC_MEMORY);				// SERIOUS ERROR

						nHuffmanLength = pack_huffman(pHuffman, (INT16 *) p_diff, nPackedLength);
			
#ifdef _DEBUG
						if (nHuffmanLength > raw_length * 4 + 16)
							return(NCS_INVALID_PARAMETER);				// overran pHuffman
#endif
#if defined(NCSBO_MSBFIRST)
						// Now for MAC -- force Huffman
						if(1) {
#else
						if(*packed_length > nHuffmanLength ) {
#endif 
							encode_format = ENCODE_HUFFMAN;
							pRunPacked = pHuffman;
							NCSFree(p_diff); p_diff = NULL;
							*packed_length = nHuffmanLength;
						}
						else {
							NCSFree(pHuffman);
						}
					}
#endif
				}
			}

			if( !pRunPacked )		// If the RunPacked or Huffman failed, ditch this packing
				NCS_SAFE_FREE((char *) p_diff);
		}
#endif // ENCODE_RANGE_ONLY

#ifdef ECW_ENCODE_RANGE
		// Do normal range encoding if the RANGE8 failed. Later, test against HUFFMAN or RUN_ZERO to see what was best
		if( encode_format == ENCODE_RANGE
) { //		 || encode_format == ENCODE_RUN_ZERO
//			 || encode_format == ENCODE_HUFFMAN ) {
			p_try_raw = p_raw;
			count = raw_length;
			rc.p_packed = p_try_packed + 1;		// must leave room for encoding type symbol
			initqsmodel(&qsm,257,12,2000,NULL,1);
			start_encoding(&rc,0);

			while(count--) {
				register	int	ch;
				if( rc.bytecount >= raw_length)
					break;		// abort the compression
				ch = (int)  ((*p_try_raw++) & 0xff);
				syfreq = qsgetfreq(&qsm,ch,&ltfreq);
				encode_shift(&rc,syfreq,ltfreq,12);
				qsupdate(&qsm,ch);
			}
			// write 256 as end of stream marker
			syfreq = qsgetfreq(&qsm,256,&ltfreq);
			encode_shift(&rc,syfreq,ltfreq,12);
			done_encoding(&rc);
			deleteqsmodel(&qsm);
			if( rc.bytecount < *packed_length) {
				// we were better than the other attempts, so throw the other packed alternative away
				*packed_length = rc.bytecount;
				encode_format = ENCODE_RANGE;
				// Make sure the runpacked memory is freed
				if( pRunPacked ) {
					NCSFree( (char *) pRunPacked );
					pRunPacked = NULL;
				}
			}
		}
#endif // ECW_ENCODE_RANGE
		if( pRunPacked ) {
			// one of the methods (RUN_ZERO or HUFFMAN) worked better, so copy that data
			memcpy(p_try_packed + 1, pRunPacked, *packed_length);
			NCSFree( (char *) pRunPacked );
			pRunPacked = NULL;
		}
	}


	if( *packed_length < raw_length ) {
		// range compression generated output smaller than RAW
		*p_try_packed = (UINT8)encode_format;
		*packed_length += 1;
		*p_packed = p_try_packed;
	}
	else {
		// No good, so fall back to RAW output
		*p_try_packed = ENCODE_RAW;

#ifdef NCSBO_MSBFIRST	
		if(2 == symbol_size_hint) {
			NCSByteSwapRange16((UINT16*)(p_try_packed + 1), (UINT16*)p_raw, raw_length / 2);
		} else {	
			memcpy(p_try_packed + 1, p_raw, raw_length);
		}
#else
		memcpy(p_try_packed + 1, p_raw, raw_length);
#endif
		*packed_length = raw_length + 1;
		*p_packed = p_try_packed;
	}

//printf("Encode: [%d] from %6d to %6d (%4.1lf%%)\n",
//	   *p_try_packed, raw_length, *packed_length,
//		(IEEE8) ((IEEE8) *packed_length / (IEEE8) raw_length) * 100.0);

	return(NCS_SUCCESS);
}
