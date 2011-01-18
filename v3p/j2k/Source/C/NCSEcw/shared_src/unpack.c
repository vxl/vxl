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
** FILE:   	unpack.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:
**
**	Unpacks (decodes) a stream of data.
**	There are various routines for unpacking entire blocks, including:
**
**		int	unpack_data(UINT8 **p_raw,	UINT32 *packed_length,
**						UINT8  *p_packed, UINT32 raw_length);
**
**	This routine is block oriented, but can easily be modified to
**	unpack data in a continual stream (indeed, this is how the original
**	versions were set up prior to adding block structures for random seeks).
**
**	Routines return non-zero if there was an error, in which case
**	no memory will have been allocated (so don't free it).
**	Raw_length MUST be a multiple of symbol_size_hint
**
**	[06] if *p_packed == NULL, it returns a pointer to a newly allocated array of data p_packed,
**	which is packed_length in bytes, which must be freed by the caller once usage
**	is complete. If *p_packed is NOT null, it is assumed that the caller has pre-allocated
**	the memory required.
**
**	unpack_data() unpacks previously packed data, and returns a newly allocated
**	memory array p_raw, which must be freed by the caller once it is used.
**	Note that the caller must report the original raw length to the unpacker,
**	which returns the length of the packed data. This is handy when multiple sets
**	of encoded data were stored as a single stream of data (for example sideband blocks);
**	you can set p_packed = p_packed + packed_length and then retrieve the next
**	packed set of data.  Note that this means you must know the length of the data
**	that was packed; the unpack_data() routine does not know this number so must
**	have it supplied.
**
**	There are three visible routines for unpacking individual lines out of a block.
**	You must unpack lines in sequence, although you can skip lines at the start,
**	and you can terminate unpacking before reading to the end of the block.
**	These routines know about the quantization, and will dequantize
**
**	The routines are:
**
**	unpack_start_lines()
**		Get ready to start unpacking lines out of this block.
**	unpack_finish_lines()
**		We have finished with this block, so the unpacker can free any memory allocated
**	unpack_line()
**		unpack one line.
**
**	Note that in all cases:
**	(1)	Lengths are handed to these routines in bytes, not symbol lengths
**
**	ENCODING/DECODING NOTES:
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
** [10] sns 03Apr99 Adding RangeZero decoding
** [11] sns 10Apr99 Adding Huffman decoding
** [12] sns 13May99 Fixing nasty memory leak in free line logic
** [13] sjc 08Jul99 Fixed UMR
** [14]  ny 24Nov00 WinCE/PALM SDK Changes
 *******************************************************/

#include "NCSEcw.h"
#include "qsmodel.h"
#include "rangecod.h"
#include "NCSHuffmanCoder.h"

#if !defined(_WIN32_WCE)
	#ifdef WIN32
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif
#ifdef USE_SSE
#include <xmmintrin.h>
#endif // USE_SSE

#include "rangecode.c"		/* IMPORTANT!! Include this in-line for performance */

#define MAX_RANGE	256		/* number of symbols used in range encoding */

#define MAX_INT8	 127	/* used for difference encoding */
#define	MIN_INT8	-127
#define	MAX_INT16	 32767
#define	MIN_INT16	-32767


#define MIN_ZERO_MEMSET	16	/* number of IEEE4 values that are faster to clear in a loop rather than memset */


#ifdef NCS_NO_UNALIGNED_ACCESS
#	define		READ_UINT16(p) (((UINT16)p[0]) | ((UINT16)p[1] << 8))
#else
#	ifdef NCSBO_MSBFIRST
#	define		READ_UINT16(p) NCSByteSwap16(*((UINT16*)p))
#	else
#	define		READ_UINT16(p) *((UINT16*)p);
#	endif
#endif

/*
**	For the unpack_line() set of routines, we have some local information we allocate
**	while reading a block, and free at the end. This is so we can hide the unpacking
**	from the decompression routine, which only cares about data in a line by line basis
**	not as blocks, so we don't want to contaminate that code with the fact that the block
**	routines work with multiple Y values in a block.
*/
// there is one UnpackBandStruct for each band within each UnpackLineStruct
typedef struct {
	UINT8	*p_packed[MAX_SIDEBAND];	/* ptr into currect location of packed data being unpacked */
	INT16	prev_val[MAX_SIDEBAND];		/* used for ENCODE_RANGE8; previous difference value */
										/* [10] also used for ENCODE_RUN_ZERO; zero run count outstanding */
	NCSHuffmanState HuffmanState[MAX_SIDEBAND];

	EncodeFormat encode_format[MAX_SIDEBAND];
	// Range encoder specific values
	rangecoder rc[MAX_SIDEBAND];
	qsmodel qsm[MAX_SIDEBAND];
} UnpackBandStruct;

// there is one UnpackLineStruct for each block being unpacked
typedef struct {
	// There are (nr_bands * MAX_SIDEBAND) of the two following values
	UINT8	*p_compressed_x_block;	// pointer to current packed block for this unpack block
	UnpackBandStruct *p_bands;	/* ptr to array, one for each band */
	UINT16	nr_sidebands;		/* number of sidebands in this block */
	UINT32	nr_bands;			/* number of bands in this block */
	UINT16	first_sideband;		/* LL for 0 level, LH for others */
	UINT32	line_length;		/* line length in bytes */
	UINT32	valid_line;			/* number of bytes between pre-skip and post-skip */
	UINT32	pre_skip;			/* amount to pre-read for each line	 */
	UINT32	post_skip;			/* amount to post-read for each line */
} UnpackLineStruct;



/*
**	Some of these are called a lot, so are inline where possible
*/

/*******************************************************************************
**	unpack_ecw_block() unpacks an entire block, and returns a newly allocated
**	unpacked block of data. This routine is optional. It is used only by NCS
**	caching, where that library notices that it is worth keeping an unpacked
**	block in memory, rather than unpacking the block on the fly, for performance
**	reasons.
**	This routine is never used where the fileview is so large that caching is
**	not set for the file view.
**
**	NOTES:
**		Despite what NCS might want, this routine MAY decide not to unpack the
**		block. Most likely because:
**		-	The output block would be huge
**		-	Memory is too low
**		In these cases, NCS should continue to use the packed block.
**
**	Returns zero if no error
**
**	See NCS routines for full details
**
*******************************************************************************/

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
int unpack_ecw_block(QmfLevelStruct *pQmfLevel, UINT32 nBlockX, UINT32 nBlockY,
					 Handle *ppUnpackedECWBlock, UINT32	*pUnpackedLength,
					 UINT8 *pPackedBlock)
#else
int unpack_ecw_block(QmfLevelStruct *pQmfLevel, UINT32 nBlockX, UINT32 nBlockY,
					 UINT8 **ppUnpackedECWBlock, UINT32	*pUnpackedLength,
					 UINT8 *pPackedBlock)
#endif //MACINTOSH
{
	UINT8	*pUnpackedBlock, *pUnpackedBand, *pPackedBand;
	UINT32	nSidebands;
	UINT32	nSidebandLength;
	UINT32	nSideband, nBlockSizeX, nBlockSizeY;
	UINT32	nPackedOffset = 0;
	UINT32	nUnpackedOffset;
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	OSErr osErr;	
#endif //MACINTOSH

	*ppUnpackedECWBlock = NULL;

	if( pQmfLevel->level )
		nSidebands = pQmfLevel->nr_sidebands - 1;
	else
		nSidebands = pQmfLevel->nr_sidebands;

	if( nBlockX != (pQmfLevel->nr_x_blocks - 1) )
		nBlockSizeX = pQmfLevel->x_block_size;
	else
		nBlockSizeX = (pQmfLevel->x_size - (nBlockX * pQmfLevel->x_block_size));
	if( nBlockY != (pQmfLevel->nr_y_blocks - 1) )
		nBlockSizeY = pQmfLevel->y_block_size;
	else
		nBlockSizeY = (pQmfLevel->y_size - (nBlockY * pQmfLevel->y_block_size)); 

	nSidebands = nSidebands * pQmfLevel->nr_bands;
	nSidebandLength = nBlockSizeX * nBlockSizeY * sizeof(INT16);
	nSidebandLength += sizeof(EncodeFormat);		// we want to add one for the leading COMPRESSION TYPE flag
	// we need room for N-1 UINT32's of sidebands, and N bytes of compression type flags,
	// and N unpacked blocks.

	*pUnpackedLength = (sizeof(UINT32) * (nSidebands-1)) + (nSidebands * nSidebandLength);
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	*ppUnpackedECWBlock = TempNewHandle( *pUnpackedLength, &osErr );
	if( *ppUnpackedECWBlock )
		pUnpackedBlock = **ppUnpackedECWBlock;
	else
		pUnpackedBlock = NULL;
#else
	pUnpackedBlock = (UINT8*)NCSMalloc( *pUnpackedLength , FALSE);
#endif
	if( !pUnpackedBlock )
		return(1);				// out of memory, so don't attempt the unpack

	// Now unpack. Final structure is:
	//	0:		UINT32 Offset to sideband 1 relative to offset to sideband 0
	//	1:		UINT32 Offset of sideband 2 relative to offset to sideband 0
	//	....
	//	N-1:	UINT32 Offset of sideband N relative to offset to sideband 0
	//			EncodeFormat	sideband 0 COMPRESSION TYPE
	//	<block> <unpacked INT16 block>
	//			EncodeFormat	sideband 1 COMPRESSION TYPE
	//	<block> <unpacked INT16 block>
	//
	//	and so on for all unpacked blocks
	//
	pUnpackedBand = pUnpackedBlock + (sizeof(UINT32) * (nSidebands-1));
	pPackedBand   = pPackedBlock + (sizeof(UINT32) * (nSidebands-1));
	nSideband = nSidebands;

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
#else
	*ppUnpackedECWBlock = pUnpackedBlock;	// will reuse the unpacked block pointer
#endif

	// now go and pack the block. Start offsets with offset to sideband 1
	nUnpackedOffset = nSidebandLength;

	while(nSideband--) {
		// Length will be the same for all bands, as we unpack to uncompressed format
		if( nSideband ) {
			*pUnpackedBlock++ = (UINT8)((nUnpackedOffset >> 24) & 0xff);	// 0xFF000000
			*pUnpackedBlock++ = (UINT8)((nUnpackedOffset >> 16) & 0xff);	// 0x00FF0000
			*pUnpackedBlock++ = (UINT8)((nUnpackedOffset >>  8) & 0xff);	// 0x0000FF00
			*pUnpackedBlock++ =  (UINT8)(nUnpackedOffset        & 0xff);	// 0x000000FF
		}

		// now go and unpack the block itself
		*((EncodeFormat*)pUnpackedBand) = ENCODE_RAW;
		pUnpackedBand += sizeof(EncodeFormat);

		if( unpack_data(&pUnpackedBand,&pPackedBand[nPackedOffset], nSidebandLength - sizeof(EncodeFormat), sizeof(EncodeFormat)) ) {
			// ERROR during unpack
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
			DisposeHandle( *ppUnpackedECWBlock );
#else
			NCSFree(*ppUnpackedECWBlock);
#endif
			*ppUnpackedECWBlock = NULL;
			return(1);
		}

		// and work out where the next packed block is
		if( nSideband ) {
			nPackedOffset  = ((UINT32) *pPackedBlock++) << 24;
			nPackedOffset |= ((UINT32) *pPackedBlock++) << 16;
			nPackedOffset |= ((UINT32) *pPackedBlock++) << 8;
			nPackedOffset |= ((UINT32) *pPackedBlock++);

			pUnpackedBand += (nSidebandLength-sizeof(EncodeFormat));	// was incremented by ENCODE byte already, so just add block length
			nUnpackedOffset += nSidebandLength;
		}
	}
	return(0);
}

/*******************************************************************************
**	align_ecw_block() convert the raw block data to an aligned format
*******************************************************************************/

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
int align_ecw_block( NCSFile *pNCSFile, NCSBlockId nBlockID, Handle *ppAlignedECWBlock, UINT32	*pAlignedLength,
					 UINT8 *pPackedBlock, UINT32 nPackedLength)
#else
int align_ecw_block( NCSFile *pNCSFile, NCSBlockId nBlockID, UINT8 **ppAlignedECWBlock, UINT32	*pAlignedLength,
					 UINT8 *pPackedBlock, UINT32 nPackedLength)
#endif //MACINTOSH
{
	UINT8	*pAlignedBlock, *pAlignedBand, *pPackedBand;
	UINT32	nSidebands;
//	UINT32	nSidebandLength;
	UINT32	nSideband;
	UINT32	nPackedOffset = 0;
	UINT32 nNextPackedOffset = 0;
//	UINT32	nAlignedOffset;
	UINT32 nAddOffset = 0;
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	OSErr osErr;	
#endif //MACINTOSH
	QmfLevelStruct *pQmfLevel = pNCSFile->pTopQmf;
//	MessageBox(NULL, "Here", "HERE", MB_OK);
	while(pQmfLevel && (pQmfLevel->nFirstBlockNumber + pQmfLevel->nr_x_blocks * pQmfLevel->nr_y_blocks) <= nBlockID) {
		pQmfLevel = pQmfLevel->p_larger_qmf;
	}
	if(!pQmfLevel) {
		return(1);
	}
	*ppAlignedECWBlock = NULL;

	if( pQmfLevel->level )
		nSidebands = pQmfLevel->nr_sidebands - 1;
	else
		nSidebands = pQmfLevel->nr_sidebands;

	nSidebands = nSidebands * pQmfLevel->nr_bands;
	// we need room for N-1 UINT32's of sidebands, and N bytes of compression type flags,
	// and N unpacked blocks.

	*pAlignedLength = nPackedLength + 2*nSidebands;
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	*ppAlignedECWBlock = TempNewHandle( *pAlignedLength, &osErr );
	if( *ppAlignedECWBlock )
		pAlignedBlock = **ppAlignedECWBlock;
	else
		pAlignedBlock = NULL;
#else
	pAlignedBlock = (UINT8*)NCSMalloc( *pAlignedLength , FALSE);
#endif
	if( !pAlignedBlock )
		return(1);				// out of memory, so don't attempt the unpack

	// Now unpack. Final structure is:
	//	0:		UINT32 Offset to sideband 1 relative to offset to sideband 0
	//	1:		UINT32 Offset of sideband 2 relative to offset to sideband 0
	//	....
	//	N-1:	UINT32 Offset of sideband N relative to offset to sideband 0
	//			EncodeFormat	sideband 0 COMPRESSION TYPE
	//	<block> <packed INT16 block>
	//			EncodeFormat	sideband 1 COMPRESSION TYPE
	//	<block> <packed INT16 block>
	//
	//	and so on for all unpacked blocks
	//
	pAlignedBand = pAlignedBlock + (sizeof(UINT32) * (nSidebands-1));
	pPackedBand   = pPackedBlock + (sizeof(UINT32) * (nSidebands-1));
	nSideband = nSidebands;

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
#else
	*ppAlignedECWBlock = pAlignedBlock;	// will reuse the unpacked block pointer
#endif

	// now go and pack the block. Start offsets with offset to sideband 1

	while(nSideband--) {
		BOOLEAN bEven = FALSE;

		if( nSideband ) {
			// Read in the offset to the NEXT block
			nNextPackedOffset  = ((UINT32) *pPackedBlock++) << 24;
			nNextPackedOffset |= ((UINT32) *pPackedBlock++) << 16;
			nNextPackedOffset |= ((UINT32) *pPackedBlock++) << 8;
			nNextPackedOffset |= ((UINT32) *pPackedBlock++);
			
			if(((nNextPackedOffset - nPackedOffset) & 0x1) == 0) {
				bEven = TRUE;
				nAddOffset += 2;
			} else {
				nAddOffset++;
			}
			// Add on the alignment factor (+1 per iteration through loop)
			*pAlignedBlock++ = (UINT8)(((nNextPackedOffset + nAddOffset) >> 24) & 0xff);	// 0xFF000000
			*pAlignedBlock++ = (UINT8)(((nNextPackedOffset + nAddOffset) >> 16) & 0xff);	// 0x00FF0000
			*pAlignedBlock++ = (UINT8)(((nNextPackedOffset + nAddOffset) >>  8) & 0xff);	// 0x0000FF00
			*pAlignedBlock++ =  (UINT8)((nNextPackedOffset + nAddOffset)        & 0xff);	// 0x000000FF
		}
		// Copy the EncodeFormat, changing from a UINT8 to a UINT16 so the encoded data is UINT16 aligned
		*(EncodeFormat*)pAlignedBand = *pPackedBand++;
		// Increment the AlignedBand pointer by sizeof(UINT16)
		pAlignedBand += sizeof(EncodeFormat);
		// Copy the encoded data, the length being the difference betweem the current and the last offset, 
		// minus 1 for the UINT8 encodeformat.
		memcpy(pAlignedBand, pPackedBand, nSideband ? (nNextPackedOffset - nPackedOffset - 1) : (nPackedLength - (nNextPackedOffset + sizeof(UINT32) * (nSidebands-1) + 1)));

		// and work out where the next packed block is
		if( nSideband ) {
			pAlignedBand += nNextPackedOffset - nPackedOffset - (bEven ? 0 : 1);
			pPackedBand += nNextPackedOffset - nPackedOffset - 1;
			nPackedOffset  = nNextPackedOffset;
		}
	}
	return(0);
}

/*******************************************************************************
**	unpack_data() unpacks previously packed data, and returns a newly allocated
**	unpacked array of data.
**
**	See file header for full details
**
*******************************************************************************/

int	unpack_data(UINT8 **p_raw,
				UINT8 *p_packed, UINT32 raw_length,
				UINT8 nSizeOfEncodeFormat)
{
	UINT8	*p_unpacked;

	if( !p_packed || !raw_length ) {
		return(1);
//		ERS_setup_error(ERS_RASTER_ERROR,
//					"\nunpack_data: was given a NULL pointer for packed data, or a zero unpacked length"));

	}
	// Allocate the unpack array if caller did not allocate it
	if( *p_raw )		// [06]
		p_unpacked = *p_raw;
	else {
		p_unpacked = (UINT8 *) NCSMalloc( raw_length , FALSE);
		if( !p_unpacked )
			return(1);
//			return(ERS_setup_error(ERS_RASTER_ERROR,
//					"\nunpack_data: out of memory while allocating unpacked memory array"));
	}

	switch( (nSizeOfEncodeFormat == sizeof(EncodeFormat)) ? (*(EncodeFormat*)p_packed) : *p_packed ) {
		default :
			return(1);	
		break;
		case ENCODE_ZEROS :
			memset(p_unpacked, 0, raw_length);
		break;
		case ENCODE_RAW :
			memcpy(p_unpacked, p_packed + nSizeOfEncodeFormat, raw_length);
		break;

		case ENCODE_RUN_ZERO : {
			register UINT32	nWordCount = raw_length / 2;
			register UINT16	nZeroRun = 0;
			register INT16 *p_output = (INT16 *) p_unpacked;

			p_packed += nSizeOfEncodeFormat;

			while(nWordCount--) {
				register	UINT16	nValue;
				nValue = READ_UINT16(p_packed);p_packed += sizeof(UINT16);//*pZeroPacked++;

				if( nValue & RUN_MASK ) {
					register	UINT16 nZero;
					// If a zero value, have to update word skip count
					nZeroRun = (nValue & MAX_RUN_LENGTH) - 1;

					if( nZeroRun >= nWordCount ) {
						nZero = (UINT16)nWordCount + 1;	// already decrememented by 1
						nZeroRun = (UINT16)(nZeroRun - nWordCount);
						nWordCount = 0;			// Force exit of the while() loop later
					}
					else {
						nZero = nZeroRun + 1;	// already decremented by 1
						nWordCount -= nZeroRun;		// else mark off these zeros as read
						nZeroRun = 0;
					}
					// Set memory to zero. Use memclr if a large amount of memory
					if( nZero < MIN_ZERO_MEMSET ) {
						while(nZero--)
							*p_output++ = 0;
					}
					else {
						memset(p_output, 0, nZero * sizeof(INT16));
						p_output += nZero;
					}
				}
				else {
					if( nValue & SIGN_MASK )
#ifdef NCSBO_MSBFIRST
						*p_output++  = NCSByteSwap16(((INT16) (nValue & VALUE_MASK)) * -1);
#else
						*p_output++  = ((INT16) (nValue & VALUE_MASK)) * -1;
#endif
					else
#ifdef NCSBO_MSBFIRST
						*p_output++  = NCSByteSwap16((INT16) nValue);
#else
						*p_output++  = ((INT16) nValue);
#endif
				}
			}
		}
		break;

		case ENCODE_HUFFMAN : {
				p_packed += nSizeOfEncodeFormat;		// skip past encoding information
				unpack_huffman(p_packed, (INT16 *) p_unpacked, raw_length);
			}
		break;

		case ENCODE_RANGE : {
			// range encoder variables
			register int ch, syfreq;
			int ltfreq;
			rangecoder rc;
			qsmodel qsm;
			register UINT32	count = raw_length;
			register UNALIGNED UINT8	*p_output = p_unpacked;

			memset( &rc, 0, sizeof(rc));
			memset( &qsm, 0, sizeof(qsm));

			/* init the model the same as in the compressor */
			rc.p_packed = p_packed + nSizeOfEncodeFormat;	/* read encoded stream from this memory */
			initqsmodel(&qsm,257,12,2000,NULL,0);
			start_decoding(&rc);
			while(count--) {
				// unpack a byte
				ltfreq = decode_culshift(&rc,12);
				ch = qsgetsym(&qsm, ltfreq);
				*p_output++ = (UINT8) ch;
				syfreq = qsgetfreq(&qsm,ch,&ltfreq);
				decode_update( &rc, syfreq, ltfreq, 1<<12);
				qsupdate(&qsm,ch);
			}
			syfreq = qsgetfreq(&qsm,256,&ltfreq);
			decode_update( &rc, syfreq, ltfreq, 1<<12);
			done_decoding(&rc);
			deleteqsmodel(&qsm);
		}
		break;
		// Similar to range encoding, but first does 8 bit differencing
		case ENCODE_RANGE8 : {
//memset(p_unpacked, 0, raw_length);
//break;
			// range encoder variables
			register int ch, syfreq;
			int ltfreq;
			rangecoder rc;
			qsmodel qsm;
			register UINT32	count = raw_length;
			register UNALIGNED INT16	*p_output = (INT16 *) p_unpacked;
			register INT16	prev;

			/* init the model the same as in the compressor */
			rc.p_packed = p_packed + nSizeOfEncodeFormat;	/* read encoded stream from this memory */
			initqsmodel(&qsm,257,12,2000,NULL,0);
			start_decoding(&rc);

			// unpack first value as 16 bits, top 8 bits first
			// upper 8 bits
			ltfreq = decode_culshift(&rc,12);
			ch = qsgetsym(&qsm, ltfreq);
			prev = (INT16)((ch & 0xff) << 8);
			syfreq = qsgetfreq(&qsm,ch,&ltfreq);
			decode_update( &rc, syfreq, ltfreq, 1<<12);
			qsupdate(&qsm,ch);
			// lower 8 bits
			ltfreq = decode_culshift(&rc,12);
			ch = qsgetsym(&qsm, ltfreq);
			prev |= (ch & 0xff);
			syfreq = qsgetfreq(&qsm,ch,&ltfreq);
			decode_update( &rc, syfreq, ltfreq, 1<<12);
			qsupdate(&qsm,ch);

			count >>= 1;				// [06] must loop through 16 bit values
			while(count--) {
				register	INT8	diff;
				// unpack a byte
				ltfreq = decode_culshift(&rc,12);
				ch = qsgetsym(&qsm, ltfreq);
				// don't check for end of symbol run, as 256 is same as a negative number
				diff = (INT8) ch;		// we have to force it to signed first
				prev = prev + diff;		// then we can add it safely
#ifdef NCSBO_MSBFIRST
				*p_output++ = NCSByteSwap16(prev);
#else
				*p_output++ = prev;
#endif
				syfreq = qsgetfreq(&qsm,ch,&ltfreq);
				decode_update( &rc, syfreq, ltfreq, 1<<12);
				qsupdate(&qsm,ch);
			}
			syfreq = qsgetfreq(&qsm,256,&ltfreq);
			decode_update( &rc, syfreq, ltfreq, 1<<12);
			done_decoding(&rc);
			deleteqsmodel(&qsm);
		}
		break;
	}
	if( !*p_raw )		// [06]
		*p_raw = p_unpacked;
	return(0);
}


/*
**	unpack_init_lines()	  call once per level. Allocates memory structures for unpacking lines
**	unpack_start_lines()  sets up decompression ready to unpack sequential lines out of
**						  a packed block.
**	unpack_line()		  unpacks one line of previously packed QMF level data, to the
**						  indicated previously allocated output line.
**	unpack_finish_lines() completes decompression, enabling the decompressor to free
**						  any tables or memory it might have set up.
**	unpack_free_lines()	  Call once per level. Frees memory structures used to unpack lines.
**
**	NOTES:
**
**	unpack_line() will pre-read and throw away pre-read bytes, then unpack the desired section,
**	then will post read and throw away post-read bytes.
**
**	It keeps various compression states between calls, so these must not be deleted
**	between calls.
**
**	All lengths for these routines are in symbol size, not bytes
**
**
**	See file header for full details
**
*/

/*
**	unpack_init_lines() - call this once per level.
**
**	Allocates enough p_info blocks to hold the blocks to be read line by line for this level.
**	Also sets up the start and end of line skip values for these blocks.
**
*/
int unpack_init_lines( QmfRegionLevelStruct *p_level )
{

	UnpackLineStruct *p_info;
	UINT32	next_x_block, x_block_count;

	// set up a pointer to an array of the compression info structures
	p_info = (UnpackLineStruct *) NCSMalloc(sizeof(UnpackLineStruct) * p_level->x_block_count, FALSE);
	if( !p_info )
		return(1);
//		return(ERS_setup_error(ERS_RASTER_ERROR,
//			"\nunpack_init_lines: unable to allocate packed block structures"));

	/*
	** To reduce malloc's, we allocate all memory needed for all compression info band structures
	** using the first block (so it always points to the start of the arrays), and
	** then only free using the first compression structure. This is harder to follow,
	** but much faster.
	*/
	// [12] because read can be aborted at any time, must set the structure to zero
	// [12] to ensure free's are correct.
	p_info->p_bands = (UnpackBandStruct *) 
			NCSMalloc(sizeof(UnpackBandStruct) * p_level->x_block_count * p_level->used_bands, TRUE);
	if( !p_info->p_bands ) {
		NCS_SAFE_FREE(p_info);
		return(1);
//		return(ERS_setup_error(ERS_RASTER_ERROR,
//				"\nunpack_start_lines: unable to allocate packed block band structures"));
	}

	// Now point to the correct array locations, set up skip values, etc
	for(next_x_block = p_level->start_x_block, x_block_count = 0;
				x_block_count < p_level->x_block_count;
				x_block_count++, next_x_block++ ) {
		UINT32	x_block_size;

		// point to the appropriate band. We point to the first band for the block,
		// the rest are offset from that band
		p_info[x_block_count].p_bands = p_info[0].p_bands + (x_block_count * p_level->used_bands);

		if( next_x_block != (p_level->p_qmf->nr_x_blocks - 1) )
			x_block_size = p_level->p_qmf->x_block_size;
		else
			x_block_size = (p_level->p_qmf->x_size - (next_x_block * p_level->p_qmf->x_block_size));
		p_info[x_block_count].p_compressed_x_block = NULL;
		p_info[x_block_count].first_sideband = (p_level->p_qmf->level ? LH_SIDEBAND : LL_SIDEBAND );
		p_info[x_block_count].nr_sidebands = p_level->p_qmf->p_top_qmf->nr_sidebands;
		p_info[x_block_count].nr_bands = p_level->used_bands;
		p_info[x_block_count].line_length = x_block_size;
		p_info[x_block_count].pre_skip = (next_x_block == p_level->start_x_block ? p_level->first_block_skip : 0);
		p_info[x_block_count].post_skip = (x_block_count == p_level->x_block_count - 1 ? p_level->last_block_skip  : 0);
		p_info[x_block_count].valid_line = x_block_size - (p_info[x_block_count].pre_skip + p_info[x_block_count].post_skip);
	}
	p_level->p_x_blocks = (void *) p_info;
	return(0);
}

void unpack_free_lines(  QmfRegionLevelStruct *p_level  )
{
	UnpackLineStruct *p_info;

	p_info = (UnpackLineStruct *) p_level->p_x_blocks;
	if( !p_info )
		return;
	// wrap up any unfinished decompression
	unpack_finish_lines( p_level );
	// free the block allocations. They were all allocated and pointed as a 
	// single array for all bands, all blocks.
	
	#ifndef MACINTOSH  //causing memory error, removed line instead of fixing problem.
	NCS_SAFE_FREE(p_info->p_bands);
	#endif
	NCS_SAFE_FREE(p_level->p_x_blocks);
	p_level->p_x_blocks = NULL;
}

/*
**	This stops any compression in progress for these blocks, and frees the compressed blocks,
**	but leaves the info structures still allocated for the next set of X block.
*/

void unpack_finish_lines( QmfRegionLevelStruct *p_level  )
{
	UnpackLineStruct *p_info;
	UINT32	x_block_count;

	p_info = (UnpackLineStruct *) p_level->p_x_blocks;
	for(x_block_count = 0; x_block_count < p_level->x_block_count; x_block_count++ ) {
		// Free any open part-way compressions or opened compressed blocks
		if( p_info->p_compressed_x_block) {
			register UINT32	band;
			register UnpackBandStruct *p_bands;
			p_bands = p_info->p_bands;
			for(band = 0; band < p_info->nr_bands; band++ ) {
#ifdef PALM
				register int sideband;
#else
				register Sideband sideband;
#endif
				// We must free information depending on what type of compression was used
				// for each sideband
				for(sideband = p_info->first_sideband; sideband < p_info->nr_sidebands; sideband++ ) {
					switch( p_bands->encode_format[sideband] ) {
						default :
							// this is an error
							_ASSERT(FALSE);
							;
						break;
						case ENCODE_ZEROS :
						case ENCODE_RAW :
						case ENCODE_RUN_ZERO :
							// do nothing
							;
						case ENCODE_HUFFMAN :
							unpack_huffman_fini_state(&(p_bands->HuffmanState[sideband]));
						break;
						case ENCODE_RANGE :
						case ENCODE_RANGE8 :
							// free the range encoder memory
							done_decoding(&(p_bands->rc[sideband]));
							deleteqsmodel(&(p_bands->qsm[sideband]));
						break;
					}
					p_bands->encode_format[sideband] = ENCODE_INVALID;	/* just in case */
				}
				p_bands++;
			}
			NCScbmFreeViewBlock(p_level, p_info->p_compressed_x_block );
			p_info->p_compressed_x_block = NULL;
		}
		p_info++;
	}
}

/*
**	This sets up unpacking for the specified block. It pre-reads the
**	indicated number of lines_to_skip.  It does NOT free p_packed_block
**	if there was an error. However, if all went well, the block is pointed
**	to by the info structures, and will later be freed by the unpack_finish_lines() call.
*/

int unpack_start_line_block( QmfRegionLevelStruct *p_level, UINT32 x_block_count,
							UINT8 *p_packed_block, UINT32	lines_to_skip)
{

	UnpackLineStruct *p_info;
#ifdef PALM
	int 		sideband;
#else
	Sideband	sideband;
#endif
	UINT32		*p_offset;
	UINT32		offset;
	UINT32		band;
	UnpackBandStruct	*p_band;
	UINT8		*p_base_of_block = p_packed_block;

	p_info = (UnpackLineStruct *) p_level->p_x_blocks;
	p_info += x_block_count;		// point to the currect block to work on

	p_offset = (UINT32 *) p_base_of_block;
	offset = 0;

	// there nr_bands * (nr_sidebands-1) offsets, as we know
	// the first one is straight after the offset array.
	// Note the offset is QMF bands, not level bands.
	p_packed_block = p_base_of_block + (sizeof(UINT32) * 
				((p_level->p_qmf->nr_bands * 
					(p_info->nr_sidebands - p_info->first_sideband)) - 1));
	// extract the offsets for each sideband and point to them
	// and skip the leading bytes
	for(band=0; band < p_info->nr_bands; band++ ) {
		p_band = p_info->p_bands + band;
		for(sideband = p_info->first_sideband; sideband < p_info->nr_sidebands; sideband++ ) {
			UINT8	*p_packed_sideband;
			p_packed_sideband = p_packed_block + offset;
			p_band->encode_format[sideband] = *((EncodeFormat*)p_packed_sideband);
			p_band->p_packed[sideband] = p_packed_sideband + sizeof(EncodeFormat);	// skip over encoding format
			switch( p_band->encode_format[sideband] ) {
				default :
#ifdef _DEBUG_NEVER
				{
				UINT16	aband;
				UINT32	asideband;
					printf("Unpack_start_lines: block decoding error (encode format was %d)\n",
						p_band->encode_format[sideband]);
					printf("Level: %d, Block [%d, %d], Band %d, Subband %d\n",
						p_level->p_qmf->level,
						p_level->current_line / p_level->p_qmf->x_block_size,
						p_level->start_x_block + x_block_count,
						band, sideband);
					printf("Level size is [%d,%d]. This block should have %d bands, %d subbands\n",
						p_level->p_qmf->x_size, p_level->p_qmf->y_size,
						p_level->p_qmf->nr_bands,
						(p_info->nr_sidebands - p_info->first_sideband));
					// get back to the original start of block
					p_offset = (UINT32 *) p_base_of_block;
					offset = 0;
					// There are one less offsets than sidebands in the block
					p_packed_block = p_base_of_block + (sizeof(UINT32) * 
								((p_level->p_qmf->nr_bands * 
									(p_info->nr_sidebands - p_info->first_sideband)) - 1));

					for(aband=0; aband < p_info->nr_bands; aband++ ) {
						for(asideband = p_info->first_sideband; asideband < p_info->nr_sidebands; asideband++ ) {
							p_packed_sideband = p_packed_block + offset;
							printf(" Band %d, sideband %d: offset is %6d, encode_format is %d (-1=%d,+1=%d)\n",
								aband, asideband, offset, p_packed_sideband[0], p_packed_sideband[-1], p_packed_sideband[1]);
							offset = sread_int32((UINT8 *) p_offset);
							p_offset++;
						}
					}
				}
#endif
					return(1);
//					return(ERS_setup_error(ERS_RASTER_ERROR,
//						"\nunpack_start_lines: packed data has an invalid encoding format"));
				break;
				case ENCODE_ZEROS :
					;	// nothing to do
				break;

				case ENCODE_RAW :
					// skip over lines_to_skip
					p_band->p_packed[sideband] += 
							(p_info->line_length * lines_to_skip * sizeof(UINT16));
				break;

				case ENCODE_RUN_ZERO : {
					// [10] The problem here is the zero-run count.
					// We have to keep track of outstanding zero's
					register	UINT32	nWordSkip;
					register	UINT16	nZeroRun = 0;	// outstanding zero count
					register	UINT16	*pZeroPacked = (UINT16 *) p_band->p_packed[sideband];
					nWordSkip = (p_info->line_length * lines_to_skip);
					while(nWordSkip--) {
						register	UINT16	nValue;
#ifdef NCSBO_MSBFIRST
						nValue = NCSByteSwap16(*pZeroPacked++);
#else
						nValue = *pZeroPacked++;
#endif
						if( nValue & RUN_MASK ) {
							// If a zero value, have to update word skip count
							nZeroRun = (nValue & MAX_RUN_LENGTH) - 1;	// already read 1 word
							if( nZeroRun >= nWordSkip ) {
								nZeroRun = (UINT16)(nZeroRun - nWordSkip);
								break;					// found more zero's than we wanted, so break
							}
							nWordSkip -= nZeroRun;		// else mark off these zeros as read
							nZeroRun = 0;
						}
					}
					p_band->prev_val[sideband] = nZeroRun;				/* keep track of outstanding zero's */
					p_band->p_packed[sideband] = (UINT8 *) pZeroPacked;	/* update packed pointer */
				}
				break;

				case ENCODE_HUFFMAN : {
					// [10] The problem here is the zero-run count.
					// We have to keep track of outstanding zero's
					register	UINT32	nWordSkip;
					register	UINT16	nZeroRun = 0;	// outstanding zero count

					unpack_huffman_init_state(&(p_band->HuffmanState[sideband]), &(p_band->p_packed[sideband]));

					nWordSkip = (p_info->line_length * lines_to_skip);
					while(nWordSkip--) {
						register	NCSHuffmanSymbol *pSymbol;

						pSymbol = unpack_huffman_symbol(&(p_band->p_packed[sideband]), &(p_band->HuffmanState[sideband]));
						
						if(unpack_huffman_symbol_zero_run(pSymbol)) {
							nZeroRun = unpack_huffman_symbol_zero_length(pSymbol);
							if( nZeroRun >= nWordSkip ) {
								nZeroRun = (UINT16)(nZeroRun - nWordSkip);
								break;					// found more zero's than we wanted, so break
							}
							nWordSkip -= nZeroRun;		// else mark off these zeros as read
							nZeroRun = 0;
						}
					}
					p_band->prev_val[sideband] = nZeroRun;				/* keep track of outstanding zero's */
				}
				break;

				case ENCODE_RANGE : {
					UINT32	skip_bytes;
					// range encoder variables
					register int ch, syfreq;
					int	ltfreq;
					rangecoder *p_rc;
					qsmodel *p_qsm;

					/* init the model the same as in the compressor */
					p_rc  = &(p_band->rc[sideband]);
					p_qsm = &(p_band->qsm[sideband]);
					p_rc->p_packed = p_band->p_packed[sideband];	/* read encoded stream from this memory */
					initqsmodel(p_qsm,257,12,2000,NULL,0);
					start_decoding(p_rc);

					skip_bytes = (p_info->line_length * lines_to_skip * sizeof(UINT16));
					while(skip_bytes--) {
						// Read and throw away some data
						ltfreq = decode_culshift(p_rc,12);
						ch = qsgetsym(p_qsm, ltfreq);
						syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
						decode_update( p_rc, syfreq, ltfreq, 1<<12);
						qsupdate(p_qsm,ch);
					}
				}
				break;

				case ENCODE_RANGE8 : {
					UINT32	skip_bytes;
					// range encoder variables
					register int ch, syfreq;
					int ltfreq;
					register rangecoder *p_rc;
					register qsmodel *p_qsm;
					register	INT16	prev;

					// We first retrieve the 16 bit start value, then if skipping we
					// keep track of the previous value, for future use
					/* init the model the same as in the compressor */
					p_rc  = &(p_band->rc[sideband]);
					p_qsm = &(p_band->qsm[sideband]);
					p_rc->p_packed = p_band->p_packed[sideband];	/* read encoded stream from this memory */
					initqsmodel(p_qsm,257,12,2000,NULL,0);
					start_decoding(p_rc);
			
					// unpack start value for difference stream. It is 16 bits long
					ltfreq = decode_culshift(p_rc,12);
					ch = qsgetsym(p_qsm, ltfreq);
					prev = (INT16)((ch & 0xff) << 8);
					syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
					decode_update(p_rc, syfreq, ltfreq, 1<<12);
					qsupdate(p_qsm,ch);
					// lower 8 bits
					ltfreq = decode_culshift(p_rc,12);
					ch = qsgetsym(p_qsm, ltfreq);
					prev |= (ch & 0xff);
					syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
					decode_update(p_rc, syfreq, ltfreq, 1<<12);
					qsupdate(p_qsm,ch);
		
					// we skip in 8 bit units, as they were encoded as 8 bit differences
					skip_bytes = (p_info->line_length * lines_to_skip);
					while(skip_bytes--) {
						register	INT8	diff;
						// unpack a byte
						ltfreq = decode_culshift(p_rc,12);
						ch = qsgetsym(p_qsm, ltfreq);
						diff = (INT8) ch;		// we have to force it to signed first
						prev = prev + diff;		// then we can add it safely
						syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
						decode_update(p_rc, syfreq, ltfreq, 1<<12);
						qsupdate(p_qsm,ch);
					}
					p_band->prev_val[sideband] = prev;		/* keep track of our previous value */
				}
				break;
			}
			if(!((band == p_info->nr_bands -1) && (sideband == p_info->nr_sidebands - 1))) {
				/*
				** Don't read offset if finished [13]
				*/
				offset = sread_int32((UINT8 *) p_offset);
				p_offset++;
			}
		}	/* end sideband for loop */
	}	/* end band for loop */
	p_info->p_compressed_x_block = p_base_of_block;
	return(0);
}

/*
**	Unpack one line of the set of X blocks into the p_level->line1[] structure.
**	We must consider reflection on the left size.
*/

int	unpack_line(QmfRegionLevelStruct *p_level )
{
	UINT32			x_block_count;
	UnpackLineStruct *p_info;
	UINT32			band;
#ifdef PALM
	int				sideband;
#else
	Sideband		sideband;
#endif

	// Loop through each block for the input line
	for(x_block_count = 0; x_block_count < p_level->x_block_count; x_block_count++ ) {
		UINT32	block_offset;
		p_info = (UnpackLineStruct *) p_level->p_x_blocks;
		// while pointing to block 0, compute offset, as first block is a partial block
		if( x_block_count )
			block_offset = p_level->reflect_start_x + p_info->valid_line +
							((x_block_count - 1) * p_level->p_qmf->x_block_size);
		else
			block_offset = p_level->reflect_start_x;
		p_info = p_info + x_block_count;
		// Loop through each band for each block of the input line
		for( band=0; band < p_level->used_bands; band++ ) {
			UnpackBandStruct *p_band;
			p_band = p_info->p_bands + band;
			// Loop through each sideband for each band of each block of the input line
			for( sideband = p_info->first_sideband;	sideband < p_info->nr_sidebands; sideband++ ) {
				register IEEE4	*p_output;
				register IEEE4  fBinSize;

				p_output = p_level->p_p_line1[DECOMP_INDEX] + block_offset;

				// As of V2.0, p_qmf->scale_factor no longer has any meaning. It is always
				// 1. However, to be backward compatible with V1.0 files, we divide by scale_factor
				// to sort out scaling.

				fBinSize = ((IEEE4) p_level->p_qmf->p_band_bin_size[band]) /
								    p_level->p_qmf->scale_factor;

				switch( p_band->encode_format[sideband] ) {
					default :
						return(1);
//						return(ERS_setup_error(ERS_RASTER_ERROR,
//										"\nunpack_line: unpacking invalid compressed subband"));	
					break;

					case ENCODE_ZEROS :
						if( p_info->valid_line )	// just to be safe, check for non-zero
							memset(p_output, 0, p_info->valid_line * sizeof(INT32));
					break;

					case ENCODE_RAW : {
						register UNALIGNED INT16	*p_quantized;	// must be an INT16,do not change
						register UINT32	count;

						p_quantized = (INT16 *) p_band->p_packed[sideband];
						p_quantized += p_info->pre_skip;
						count = p_info->valid_line;
						
						if(p_level->p_qmf->scale_factor == 1.0) {
							INT32 nBinSize = (INT32)p_level->p_qmf->p_band_bin_size[band];
							while(count--) {
#ifdef NCSBO_MSBFIRST
								*p_output++  = (IEEE4)(((INT16)NCSByteSwap16(*p_quantized++)) * nBinSize);
#else
								*p_output++  = (IEEE4)(*p_quantized++ * nBinSize);
#endif							
							}
						} else {
							while(count--) {
#ifdef NCSBO_MSBFIRST
								*p_output++  = ((IEEE4) (INT16)NCSByteSwap16(*p_quantized++)) * fBinSize;
#else
								*p_output++  = ((IEEE4) (*p_quantized++)) * fBinSize;
#endif							
							}
						}
						p_band->p_packed[sideband] += 
									p_info->line_length * sizeof(UINT16);
					}
					break;

					case ENCODE_RUN_ZERO : {
						// [10] The problem here is the zero-run count.
						// We have to keep track of outstanding zero's
						register	UINT32	nWordCount   = p_info->pre_skip;
						register	UINT16	nZeroRun     = p_band->prev_val[sideband];	// outstanding zero count
						register	UNALIGNED UINT16	*pZeroPacked = (UINT16 *) p_band->p_packed[sideband];

						// PRESKIP: Have to pre-skip some values. Might have outstanding zeros to consider
						if( nZeroRun && nWordCount ) {
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZeroRun  = nZeroRun - (UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
						}
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						while(nWordCount--) {
							register	UINT16	nValue;
#ifdef NCSBO_MSBFIRST
							nValue = NCSByteSwap16(*pZeroPacked++);
#else
							nValue = *pZeroPacked++;
#endif
							if( nValue & RUN_MASK ) {
								// If a zero value, have to update word skip count
								nZeroRun = (nValue & MAX_RUN_LENGTH) - 1;	// already read 1 word
								if( nZeroRun >= nWordCount ) {
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									break;					// found more zero's than we wanted, so break
								}
								else {
									nWordCount -= nZeroRun;		// else mark off these zeros as read
									nZeroRun = 0;
								}
							}
						}

						// QUANTIZE: Now read the values actually needed. Again maybe with a run zero at the end
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						nWordCount = p_info->valid_line;
						if( nZeroRun && nWordCount ) {
							register	UINT16 nZero;
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZero = (UINT16)nWordCount;
								nZeroRun  = nZeroRun -(UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nZero = nZeroRun;
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
							// Set memory to zero. Use memclr if a large amount of memory
							if( nZero < MIN_ZERO_MEMSET ) {
								while(nZero--)
									*p_output++ = 0;
							}
							else {
								memset(p_output, 0, nZero * sizeof(IEEE4));
								p_output += nZero;
							}
						}
						while(nWordCount--) {
							register	UINT16	nValue;
#ifdef NCSBO_MSBFIRST
							nValue = NCSByteSwap16(*pZeroPacked++);
#else
							nValue = *pZeroPacked++;
#endif
							if( nValue & RUN_MASK ) {
								register	UINT16 nZero;
								// If a zero value, have to update word skip count
								nZeroRun = (nValue & MAX_RUN_LENGTH) - 1;
								if( nZeroRun >= nWordCount ) {
									nZero = (UINT16)nWordCount + 1;	// already decrememented by 1
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									nWordCount = 0;			// Force exit of the while() loop later
								}
								else {
									nZero = nZeroRun + 1;	// already decremented by 1
									nWordCount -= nZeroRun;		// else mark off these zeros as read
									nZeroRun = 0;
								}
								// Set memory to zero. Use memclr if a large amount of memory
								if( nZero < MIN_ZERO_MEMSET ) {
									while(nZero--)
										*p_output++ = 0;
								}
								else {
									memset(p_output, 0, nZero * sizeof(IEEE4));
									p_output += nZero;
								}
							}
							else {
								if( nValue & SIGN_MASK )
									*p_output++  = ((IEEE4) (nValue & VALUE_MASK)) * fBinSize * -1;
								else
									*p_output++  = ((IEEE4) nValue) * fBinSize;
							}
						}


						// POSTSKIP: Have to post-skip some values. Might have outstanding zeros to consider
						nWordCount = p_info->post_skip;
						if( nZeroRun && nWordCount ) {
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZeroRun  = nZeroRun -(UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
						}
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						while(nWordCount--) {
							register	UINT16	nValue;
#ifdef NCSBO_MSBFIRST
							nValue = NCSByteSwap16(*pZeroPacked++);
#else
							nValue = *pZeroPacked++;
#endif
							if( nValue & RUN_MASK ) {
								// If a zero value, have to update word skip count
								nZeroRun = (nValue & MAX_RUN_LENGTH) - 1;	// already read 1 word
								if( nZeroRun >= nWordCount ) {
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									break;					// found more zero's than we wanted, so break
								}
								nWordCount -= nZeroRun;		// else mark off these zeros as read
								nZeroRun = 0;
							}
						}


						p_band->prev_val[sideband] = nZeroRun;				/* keep track of outstanding zero's */
						p_band->p_packed[sideband] = (UINT8 *) pZeroPacked;	/* update packed pointer */
					}
					break;

					case ENCODE_HUFFMAN : {
						// [10] The problem here is the zero-run count.
						// We have to keep track of outstanding zero's
						register	UINT32	nWordCount   = p_info->pre_skip;
						register	UINT16	nZeroRun     = p_band->prev_val[sideband];	// outstanding zero count

						// PRESKIP: Have to pre-skip some values. Might have outstanding zeros to consider
						if( nZeroRun && nWordCount ) {
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZeroRun  = nZeroRun -(UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
						}
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						while(nWordCount--) {
							register	NCSHuffmanSymbol *pSymbol;
							pSymbol = unpack_huffman_symbol(&(p_band->p_packed[sideband]), &(p_band->HuffmanState[sideband]));
							if(unpack_huffman_symbol_zero_run(pSymbol)) {
								nZeroRun = unpack_huffman_symbol_zero_length(pSymbol);
								if( nZeroRun >= nWordCount ) {
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									break;					// found more zero's than we wanted, so break
								}
								else {
									nWordCount -= nZeroRun;		// else mark off these zeros as read
									nZeroRun = 0;
								}
							}
						}

						// QUANTIZE: Now read the values actually needed. Again maybe with a run zero at the end
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						nWordCount = p_info->valid_line;
						if( nZeroRun && nWordCount ) {
							register	UINT16 nZero;
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZero = (UINT16)nWordCount;
								nZeroRun  = nZeroRun -(UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nZero = nZeroRun;
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
							// Set memory to zero. Use memclr if a large amount of memory
							if( nZero < MIN_ZERO_MEMSET ) {
								while(nZero--)
									*p_output++ = 0;
							}
							else {
								memset(p_output, 0, nZero * sizeof(IEEE4));
								p_output += nZero;
							}
						}
						while(nWordCount--) {
							register	NCSHuffmanSymbol *pSymbol;
							pSymbol = unpack_huffman_symbol(&(p_band->p_packed[sideband]), &(p_band->HuffmanState[sideband]));
							if(unpack_huffman_symbol_zero_run(pSymbol)) {
								register	UINT16 nZero;
								nZeroRun = unpack_huffman_symbol_zero_length(pSymbol);
								// If a zero value, have to update word skip count
								if( nZeroRun >= nWordCount ) {
									nZero = (UINT16)nWordCount + 1;	// already decrememented by 1
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									nWordCount = 0;			// Force exit of the while() loop later
								}
								else {
									nZero = nZeroRun + 1;	// already decremented by 1
									nWordCount -= nZeroRun;		// else mark off these zeros as read
									nZeroRun = 0;
								}
								// Set memory to zero. Use memclr if a large amount of memory
								if( nZero < MIN_ZERO_MEMSET ) {
									while(nZero--)
										*p_output++ = 0;
								}
								else {
									memset(p_output, 0, nZero * sizeof(IEEE4));
									p_output += nZero;
								}
							}
							else {
//								*p_output++  = unpack_huffman_symbol_value(pSymbol) * fBinSize;
#ifdef NCSBO_MSBFIRST
								*p_output++  = ((IEEE4) (INT16)NCSByteSwap16(unpack_huffman_symbol_value(pSymbol))) * fBinSize;
#else
								*p_output++  = unpack_huffman_symbol_value(pSymbol) * fBinSize;
#endif
							}
						}


						// POSTSKIP: Have to post-skip some values. Might have outstanding zeros to consider
						nWordCount = p_info->post_skip;
						if( nZeroRun && nWordCount ) {
							// adjust by pre-zero run count
							if( nZeroRun >= nWordCount ) {
								nZeroRun  = nZeroRun -(UINT16)nWordCount;	// Note this limits block size to 2^15
								nWordCount = 0;
							}
							else {
								nWordCount -= nZeroRun;
								nZeroRun = 0;
							}
						}
						// now sorted out leading zeros, so now read-skip others, again maybe with
						// an outstanding run zero at the end of the run.
						while(nWordCount--) {
							register	NCSHuffmanSymbol *pSymbol;
							pSymbol = unpack_huffman_symbol(&(p_band->p_packed[sideband]), &(p_band->HuffmanState[sideband]));
							if(unpack_huffman_symbol_zero_run(pSymbol)) {
								nZeroRun = unpack_huffman_symbol_zero_length(pSymbol);
								if( nZeroRun >= nWordCount ) {
									nZeroRun = (UINT16)(nZeroRun - nWordCount);
									break;					// found more zero's than we wanted, so break
								}
								nWordCount -= nZeroRun;		// else mark off these zeros as read
								nZeroRun = 0;
							}
						}

						p_band->prev_val[sideband] = nZeroRun;				/* keep track of outstanding zero's */
					}
					break;


					case ENCODE_RANGE : {
							register UINT32	count;
							// range encoder variables
							register int ch;
							register int syfreq;
							int ltfreq;
							register rangecoder *p_rc;
							register qsmodel *p_qsm;

							p_rc  = &(p_band->rc[sideband]);
							p_qsm = &(p_band->qsm[sideband]);

							// Throw away any leading unwanted symbols
							count = p_info->pre_skip * 2;		// two bytes per symbols
							while(count--) {
								// Read and throw away some data
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update( p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
							}

							// read each symbol and write it to output
							count = p_info->valid_line;

							while(count--) {
								register INT16	squant;		// MUST be 16 bits, so sign gets extended correctly

								// bottom 8 bits
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								if (ch==256) { /* check for unexpected end-of-file */
									return(1);
//									return(ERS_setup_error(ERS_RASTER_ERROR,
//											"\nunpack_line: unexpected end of block during range line decoding"));
								}
								squant = (INT16)(ch & 0x00ff);
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update( p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
								// top 8 bits
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								squant |= (ch << 8) & 0xff00;
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update( p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);

								*p_output++ = ((IEEE4) squant) * fBinSize;
							}


							// Throw away any trailing unwanted symbols
							count = p_info->post_skip * 2;		// two bytes per symbols
							while(count--) {
								// Read and throw away some data
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update( p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
							}
					}
					break;

					case ENCODE_RANGE8 : {
							register UINT32	count;
							// range encoder variables
							register int ch;
							register INT16	prev;
							register INT8	diff;
							register int	syfreq;
							int ltfreq;
							register rangecoder *p_rc;
							register qsmodel *p_qsm;

							p_rc  = &(p_band->rc[sideband]);
							p_qsm = &(p_band->qsm[sideband]);
							prev  = p_band->prev_val[sideband];

							// Throw away any leading unwanted symbols
							count = p_info->pre_skip;		// ONE byte per symbols
							while(count--) {
								// unpack a byte
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								diff = (INT8) ch;		// we have to force it to signed first
								prev = prev + diff;		// then we can add it safely
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update(p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
							}

							// read each symbol and write it to output
							count = p_info->valid_line;

							while(count--) {
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								if (ch==256) { /* check for unexpected end-of-file */
									return(1);
//									return(ERS_setup_error(ERS_RASTER_ERROR,
//											"\nunpack_line: unexpected end of block during range 8 line decoding"));
								}
								diff = (INT8) ch;		// we have to force it to signed first
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update( p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
								prev = prev + (INT16) diff;		// then we can add it safely
								*p_output++ = ((IEEE4) prev) * fBinSize;
							}


							// Throw away any trailing unwanted symbols
							count = p_info->post_skip;		// ONE byte per symbols
							while(count--) {
								// unpack a byte
								ltfreq = decode_culshift(p_rc,12);
								ch = qsgetsym(p_qsm, ltfreq);
								diff = (INT8) ch;		// we have to force it to signed first
								prev = prev + diff;		// then we can add it safely
								syfreq = qsgetfreq(p_qsm,ch,&ltfreq);
								decode_update(p_rc, syfreq, ltfreq, 1<<12);
								qsupdate(p_qsm,ch);
							}
							p_band->prev_val[sideband] = prev;
					}
					break;
				}	/* end subband loop */
			}
		}	/* end band loop */
	}	/* end block loop */
	return(0);
}
