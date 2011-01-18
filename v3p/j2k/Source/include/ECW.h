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
** FILE:   	erswave.h
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Definitions for Wavelet compressed imagery
** EDITS:
** [01] sns	10sep98	Created file
** [02] sns	12Feb99	Modified from ECW to NCS
** [03] sns 22Mar99	Adding additional encoding techniques
** [04] sns 01Apr99	Updated to use Windows native file IO
** [05] sns	02apr99 Modified to use IEEE4 for decode instead of UINT32
**					so can better handle sub-integer errors, and later compress
**					non-integer datasets. As the number of multiplies for decompression
**					is quite low, going to IEEE4's from INT32's should not impact performance too much
** [06] sns 02Apr99	Updated to use fast floating point conversions on x86 platforms
** [07] sns 03Apr99	Adding RUN_ZERO and HUFFMAN encoding techniques
** [08] sns	06Apr99	Summing TAPs to exactly 1.0
** [09] sns 06Apr99 Adding 11 tap filter for speed of compression
** [10] sns 09Apr99 Added user control of compression block size
** [11] sns 08May99 Added on/off texture noise control flag to region structures
** [12] sns 21May99 Restructuring Compression front-end. Also using _RC_NEAR not _RC_CHOP
** [13] sns 21May99 Restructured QMF struct to prevent mis-aligned fetches
** [14] sns 04Jun99 Added internal random number generator instead of using rand()
** [15] sns 17Jun99 Changes to split Compression logic out to a separate SDK library
** [16] sjc 30Apr00 Merged Mac SDK port
** [17] ddi 14Nov00 Modified erw_decompress_read_region_line_bil() to take NCSEcwReadLineType parameter,
**					and moved NCSEcwReadLineType from ncsecw.h
** [18] rar	17-Jan-01 Mac port changes
** [19] rar 18Sep01 Modified erw_decompress_open so that if the Block Table is uncompressed (RAW)
**                  it is not passed through unpack_data (which just does a memcpy anyway)
** [20] sjc 30Apr03 Added "Low Memory" compression
** [21] jx  12Feb04 nCounter is added to QmfRegionStruct to fix rounding error in erw_decompress_read_region_line_bil().
** [22] jx  17Feb04 changed varibles: current_line, start_line, increment_y and increment_x to IEEE8 to fix rounding error in erw_decompress_read_region_line_bil().
** [23] tfl 05Jul04 Added #define for JPEG2000 file extensions
**
** NOTES
**
**	A wavelet compressed file consists of a normal .ERS file,
**	containing map projection details and so forth, and the
**	ER Mapper Compressed Wavelet (.ECW) file, which contains
**	the following structure:
**
**	ECW Header format is:
**
**	UINT8	ERW_HEADER_ID_TAG 'e'
**	UINT8	VERSION NUMBER (1..255)
**	UINT8	blocking format (BLOCKING_LEVEL = blocking individual levels)
**	UINT8	compress format	(COMPRESS_UINT8 = rescale back to 0..255 for each band.
**							 COMPRESS_YIQ = YIQ encoding of a 3 band RGB file. Number of bands must also = 3) 
**	UINT8	number of QMF levels (excluding file fake level)
**	UINT8	Number of subbands (sidebands). Currently always 4.
**	UINT32	X size of the file
**	UINT32	Y size of the file
**	UINT16	Number of bands of data (must be 1 or more)
**	UINT16	quantization scale factor
**	UINT16	X block size
**	UINT16	Y block size
**
**		Then for each level (other than the largest level, being the fake file level), the
**		following is written out:
**	UINT8		Level number
**	UINT32		x_size
**	UINT32		y_size
**	IEEE4		binsizes for each band in the file (so 1 for greyscale, 3 for RGB or YIQ)
**				(note that currently all sidebands for a band have the same binsize)
**
**		Next, the block offsets for all blocks, relative to the start of all level blocks (not relative to
**		the start of the file), are output.  As these can be a large number, they are output
**		as a compressed set. A block may contain multiple subbands. We only record the total
**		block length, as we always need all subbands.
**		Blocks are output to disk in the order of levels, so all level 0 blocks will be on disk
**		first, then level 1, and so on. The block offsets are stored in a single array, in
**		the same way. Because we want to read and write this array, for all levels, as a single
**		operation to/from disk, because we want to optimize compression by writing the entire
**		group, only one array for all QMF levels is allocated, and each QMF points into part of
**		that single array. The array is attached to the top level (which points to the first
**		entries), and is freed at that level.
**		NOTE WELL: We need to know the size of each block, which is the different between block X
**		and block X+1 offsets. Therefor, we have to write one more block offset than actually present,
**		which contains the offset to the end of the file (e.g. the file length). This could also
**		be used as a file consistency check. Eitherway, it must be there. So the block offset
**		buffers are allocated with +1 for this reason.
**
**		The length of the compressed block is written first.
**		Blocks are written with the offset to each subband other than the first,
*		then the ENCODED DATA for each subband, so the data stream is:
**	UINT32	Length of the compressed block
**	UINT8	encoding_style
**	<compressed stream of UINT64 block_offsets>
**
**		Then, the blocks of sideband data are output.
**		A block is defined as containing (level : MAX_SIDEBAND ? MAX_SIDEBAND-1) sidebands (e.g. 3 for
**		levels greater than zero, and 4 for the level 0).
**
**		As noted above, each block contains multiple sidebands. Each sideband is encoded in the
**		ENCODED DATA format as follows.
**		Blocks are written with the offset to each subband other than the first,
**		then the ENCODED DATA for each subband, so the data stream is:
**	UINT32	Offset to HL_SUBBAND	ONLY OUTPUT IF LEVEL 0 (as there will be a LL SUBBAND output)
**	UINT32	Offset to HH_SUBBAND
**		Then for each subband (4 for 1st level, 3 for all others):
**	UINT8	encoding_style
**	<compressed stream of UINT64 block_offsets>
**
**	After this, other routines write each compressed QMF level out to file.
********************************************************/

#ifndef ECW_H
#define ECW_H

#include "NCSJPCDefs.h"

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif

#ifdef MACINTOSH
#include <fenv.h>
#endif
#include <float.h>
#include <stdio.h>


#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif

typedef struct {
	NCS_FILE_HANDLE	hFile;
	void	*pClientData;
} ECWFILE;
#define NULL_ECWFILE	{ NCS_NULL_FILE_HANDLE, NULL }

#include "NCSECWCompress.h"

//#define	SAFE_FREE(ptr)		if( ptr ) free(ptr);
#define	NCS_SAFE_FREE(ptr)		if( ptr ) NCSFree(ptr);

#ifdef __cplusplus
extern "C" {
#endif


// FIXME! Move all range encoding code over to using UINT16/UINT32
#if !defined(MACINTOSH)
typedef UINT16 uint2;
#endif
typedef UINT32 uint4;
#if defined(WIN32)
typedef UINT32 uint;
#endif

#define ERS_WAVELET_DATASET_EXT	".ecw"		/* compressed wavelet format file */
#define ERS_JP2_DATASET_EXTS	{ ".jp2" , ".j2k" , ".j2c" , ".jpc", ".jpx", ".jpf" }  /*[23]*/
#define ECW_HEADER_ID_TAG   'e'				/* [02] first byte of a compressed file must be this */

// FIXME! Move all code over to using IEEE4 not FLOAT
typedef IEEE4 FLOAT;



/* Maximum amount of RAM to use while copying a file. More is faster */
#ifdef MACINTOSH // MDT 4 Meg is too big for Mac -- use 64K	/**[16]**/

#define MAX_FILE_COPY_MEMORY	(1024*64)

#else	/* MACINTOSH */

#define MAX_FILE_COPY_MEMORY	(1024*1024) * 4	// [02] 4MB is reasonable

#endif	/* MACINTOSH */

/* Maximum number of pyramid levels.
** We want enough room for terrabyte sized files
** This doesn't need to be larger than log2(MAX_IMAGE_DIM/FILTER_SIZE).
*/
#define MAX_LEVELS 20

//#define SCALE_FACTOR 1		// [05] was 256, no longer needed

/*
**	QMF sideband images are sub-blocked, so that sub-sections of sideband images
**	can be retrieved without having to read the entire sideband image.
**	So a block will be BLOCK_SIZE_X * BLOCK_SIZE_Y symbols in size.
**	The total must be less than or equal to 64K result (to ensure that encoders
**	have a fixed maximum to encode).  Smaller blocks means faster load time over
**	internet (for NCS technology), but more inefficient compression and overhead.
**	The best size is 1024(x)x64(y), which is fast, good compression, and small size.
**	Note that the compressor can compress an entire file without blocking; this
**	is a convenience definition related to decompression of images, it is not
**	essential for operation of the compressor.
**
**	The X block size is typically greater than the Y block size. This is because
**	once a block is found, it is fast to read it, and because increasing Y block
**	size increases the amount of memory required to store blocks prior to encoding
**	Both numbers must be even numbers.
**
**  [07] Note: The ENCODE_RUN_ZERO decoder currently limits block size in X to 2^15
**	in size - which should never be a problem as this is a huge value for a single
**	block, but is something  to be aware of. The RUN_ZERO unpack code is easily enhanced for larger
**	block sizes, at a slight expense in performance.
**
**	[10] Block size can now be set during compression via command line. This is the default.
*/



//#define	MIN_QMF_SIZE	128	// minimum size for the smallest LL
// Made smaller, for fast NCS initial loads
#define	MIN_QMF_SIZE	64		// minimum size for the smallest LL


/* Use these to set up nr_[x|y]_blocks in the qmf, then use those values */

#define QMF_LEVEL_NR_X_BLOCKS(p_qmf)	\
			(((p_qmf)->x_size + ((p_qmf)->x_block_size - 1)) / (p_qmf)->x_block_size)

#define QMF_LEVEL_NR_Y_BLOCKS(p_qmf)	\
			(((p_qmf)->y_size + ((p_qmf)->y_block_size - 1)) / (p_qmf)->y_block_size)

// size of filter tap bank, must be an odd number 3 or greater
// Filters are hardcode as defines, making multiplications later much faster.
// Compiler will detect and optimize symetric numbers

//#define FILTER_SIZE	15		// 15 tap filter banks.
#define FILTER_SIZE 11			// 11 tap filter banks

#if FILTER_SIZE == 15
// Lowpass sums to 1.0
#define	LO_FILTER_0		(IEEE4) -0.0012475221
#define	LO_FILTER_1		(IEEE4) -0.0024950907
#define	LO_FILTER_2		(IEEE4)  0.0087309530
#define	LO_FILTER_3		(IEEE4)  0.0199579580
#define	LO_FILTER_4		(IEEE4) -0.0505290000
#define	LO_FILTER_5		(IEEE4) -0.1205509700
#define	LO_FILTER_6		(IEEE4)  0.2930455800
#define	LO_FILTER_7		(IEEE4)  0.7061761836		// [08]
#define	LO_FILTER_8		(IEEE4)  0.2930455800
#define	LO_FILTER_9		(IEEE4) -0.1205509700
#define	LO_FILTER_10	(IEEE4) -0.0505290000
#define	LO_FILTER_11	(IEEE4)  0.0199579580
#define	LO_FILTER_12	(IEEE4)  0.0087309530
#define	LO_FILTER_13	(IEEE4) -0.0024950907
#define	LO_FILTER_14	(IEEE4) -0.0012475221
// Highpass sums to 0.0
#define	HI_FILTER_0		(IEEE4)  0.0012475221
#define	HI_FILTER_1		(IEEE4) -0.0024950907
#define	HI_FILTER_2		(IEEE4) -0.0087309530
#define	HI_FILTER_3		(IEEE4)  0.0199579580
#define	HI_FILTER_4		(IEEE4)  0.0505290000
#define	HI_FILTER_5		(IEEE4) -0.1205509700
#define	HI_FILTER_6		(IEEE4) -0.2930455800
#define	HI_FILTER_7		(IEEE4)  0.7061762272		// [08]
#define	HI_FILTER_8		(IEEE4) -0.2930455800
#define	HI_FILTER_9		(IEEE4) -0.1205509700
#define	HI_FILTER_10	(IEEE4)  0.0505290000
#define	HI_FILTER_11	(IEEE4)  0.0199579580
#define	HI_FILTER_12	(IEEE4) -0.0087309530
#define	HI_FILTER_13	(IEEE4) -0.0024950907
#define	HI_FILTER_14	(IEEE4)  0.0012475221
#endif

#if FILTER_SIZE == 11
// Lowpass sums to 1.0
#define	LO_FILTER_0		(IEEE4)  0.007987761489921101
#define	LO_FILTER_1		(IEEE4)  0.02011649866148413
#define	LO_FILTER_2		(IEEE4) -0.05015758257647976
#define	LO_FILTER_3		(IEEE4) -0.12422330961337678
#define	LO_FILTER_4		(IEEE4)  0.29216982108655865
#define	LO_FILTER_5		(IEEE4)  0.7082136219037853
#define	LO_FILTER_6		(IEEE4)  0.29216982108655865
#define	LO_FILTER_7		(IEEE4) -0.12422330961337678
#define	LO_FILTER_8		(IEEE4) -0.05015758257647976 
#define	LO_FILTER_9		(IEEE4)  0.02011649866148413
#define	LO_FILTER_10	(IEEE4)  0.007987761489921101
// Highpass sums to 0.0
#define	HI_FILTER_0		(IEEE4) -0.007987761489921101
#define	HI_FILTER_1		(IEEE4)  0.02011649866148413
#define	HI_FILTER_2		(IEEE4)  0.05015758257647976
#define	HI_FILTER_3		(IEEE4) -0.12422330961337678
#define	HI_FILTER_4		(IEEE4) -0.29216982108655865
#define	HI_FILTER_5		(IEEE4)  0.7082136219037853
#define	HI_FILTER_6		(IEEE4) -0.29216982108655865
#define	HI_FILTER_7		(IEEE4) -0.12422330961337678
#define	HI_FILTER_8		(IEEE4)  0.05015758257647976 
#define	HI_FILTER_9		(IEEE4)  0.02011649866148413
#define	HI_FILTER_10	(IEEE4) -0.007987761489921101
#endif

/* The type of the quantized images. Must be SIGNED, and capable of holding    values  in the range [-MAX_BINS, MAX_BINS] */
typedef INT16 BinIndexType;  

/* The type used to represent the binsizes. Should be UNSIGNED. If this is
   changed, be sure to change the places where 
   binsizes are written or read from files.  */
typedef UINT16 BinValueType;

typedef UINT8 Byte;

/*
** Number of possible values for a symbol.  This must be at least
**(MAX_BINS * 4)  (one sign bit, one tag bit)...
*/
#define NUM_SYMBOL_VALUES 65536
#define SIGN_MASK	0x4000
#define RUN_MASK	0x8000
#define MAX_BIN_VALUE	16383	// 0x3fff
#define MIN_BIN_VALUE	-16383	// Prior to masking
#define VALUE_MASK	0x3fff		/* without the sign or run bits set */
#define MAX_RUN_LENGTH 0x7fff	/* can't be longer than this or would run into mask bits */

/*
**	The blocking format used for a file. Currently only one supported -
**	block each level.
*/

typedef enum {
	BLOCKING_LEVEL	= 1
} BlockingFormat;

/*
**	The encoding format for a sideband within a block. More encoding
**	standards might be added later.
*/
#ifdef NOTDEF
typedef enum {
	ENCODE_INVALID	= 0,	// invalid encoding style
	ENCODE_RAW		= 1,	// data is not encoded, and is in a raw format
	ENCODE_HUFFMAN	= 2,	// zero's run length encoded, and then huffman encoded
	ENCODE_RANGE	= 3,	// range encoding
	ENCODE_RANGE8	= 4,	// 8 bit difference then range encoding
	ENCODE_ZEROS	= 5,	// encode the entire block as zero's
	ENCODE_RUN_ZERO	= 6		// Run length encoded as zero's
} EncodeFormat;
#endif
typedef UINT16 EncodeFormat;
#define	ENCODE_INVALID	0
#define	ENCODE_RAW		1
#define	ENCODE_HUFFMAN	2
#define	ENCODE_RANGE	3
#define	ENCODE_RANGE8	4
#define	ENCODE_ZEROS	5
#define	ENCODE_RUN_ZERO	6

#define MAX_SIDEBAND	4

typedef enum {
	LL_SIDEBAND = 0,	// must be in order as used for array offsets
	LH_SIDEBAND = 1,
	HL_SIDEBAND = 2,
	HH_SIDEBAND = 3
} Sideband;

/*
**	Some basic information about a QMF file, pointed to by all levels,
**	designed to make life easier for higher level applications.
**	NOTE!! NCSECWClient.h has this structure, defined as "NCSFileViewFileInfo".
**	Any changes must be kept in synch between the two structures
**
*/

typedef NCSFileViewFileInfo ECWFileInfo;
typedef NCSFileViewFileInfoEx ECWFileInfoEx;

/*
**
**	The QmfLevelBandStruct is only allocated for COMPRESSION. Because the bin size is
**	needed for decompression, it is pulled out of the band structure (binsize can be
**	different for each band) and put into the main level structure. So the bin_size
**	is the only multi-band value in the QMF level structure; the rest of the band
**	specific information is held here at the band level. This also makes the QMF
**	structure much smaller for decompression.
**
**	There is a multi-band, compression only, buffer allocated at the QMF level,
**	which is a pointer to the p_input_ll_line for all bands. This is so recursion,
**	including the top level, is easier to structure.
*/

typedef struct qmf_level_band_struct {
	//
	// These buffers are not allocated for the largest (file level) QMF level
	//
	IEEE4	*p_p_lo_lines[FILTER_SIZE+1];	// pointer to enough larger input lines for lowpass input
	IEEE4	*p_p_hi_lines[FILTER_SIZE+1];	// pointet to enough larger input lines for highpass input
	IEEE4	*p_low_high_block;				// the block of memory indexed into by the above. Used for free()
	// this is allocated with enough room on the X sides to handle reflection
	IEEE4	*p_input_ll_line;				// pointer to a single input line needed for this level

	// This points to the Y_BLOCK_SIZE buffer of output lines, quantized, stored for
	// LL (smallest level 0 only), LH, HL and HH. The index into the block for a given line
	// is (for maximum memory access performance):
	// (y_line_offset * x_line_size)
		/* These are used when p_file_qmf->bLowMemCompress is TRUE [20]*/
	UINT32 **p_p_ll_lengths;
	UINT8 ***p_p_p_ll_segs;
	UINT32 **p_p_lh_lengths;
	UINT8 ***p_p_p_lh_segs;
	UINT32 **p_p_hl_lengths;
	UINT8 ***p_p_p_hl_segs;
	UINT32 **p_p_hh_lengths;
	UINT8 ***p_p_p_hh_segs;
		/* These are used when p_file_qmf->bLowMemCompress is TRUE (1 line) or FALSE (Y_BLOCK_SIZE lines) [20]*/
	INT16	*p_quantized_output_ll_block;	// Only allocated for level 0 LL, NULL for all other levels
	INT16	*p_quantized_output_lh_block;	// LH block
	INT16	*p_quantized_output_hl_block;	// HL block
	INT16	*p_quantized_output_hh_block;	// HH block

	UINT32	packed_length[MAX_SIDEBAND];	// the packed length for a single set of sidebands for this band
} QmfLevelBandStruct;

/*
**	One level of a QMF tree
**
** The QMF levels are as follows:
**	QMF level (level 0..file_level-1)
**		Normal QMF levels. Contain temporary buffers for line processing, and a temporary file
**		pointer for writing the compressed sidebands to.
**	File level:
**		A place-holder level. Contains no buffers, but does contain information about the
**		file to be read from.
*/

struct qmf_level_struct {
	UINT16	level;
	UINT8	nr_levels;			// Number of levels, excluding file level
	UINT8	version;			// [13] Top-level only: compressed file version
	UINT8	nr_sidebands;		// [13] Top-level only: set to MAX_SIDEBANDS during write of a file
	UINT8	bPAD1;				// [13] pad to long-word
	UINT16	nr_bands;			// [13] number of bands (not subbands) in the file, e.g. 3 for a RGB file
	UINT32	x_size, y_size;
	UINT32	next_output_line;	// COMPRESSION ONLY: next output line to construct. Will go from 0 to x_size - 1
	UINT32	next_input_line;	// COMPRESSION ONLY: next line of input to read. 2 lines read for every output line
	struct	qmf_level_struct	*p_larger_qmf; 
	struct	qmf_level_struct	*p_smaller_qmf;
	struct	qmf_level_struct	*p_top_qmf;		/* pointer to top (smallest) level QMF */
	struct	qmf_level_struct	*p_file_qmf;	/* pointer to the fake (largest) file level QMF */

	UINT32	*p_band_bin_size;			/* bin size for each band during quantization. Each sideband for a band has same binsize */
	UINT16	x_block_size;				/* size of a block in X direction */
	UINT16	y_block_size;				/* size of a block in Y direction */
	UINT32	nr_x_blocks;				/* number of blocks in the X direction */
	UINT32	nr_y_blocks;				/* number of blocks in the Y direction */
	UINT32	nFirstBlockNumber;			/* [02] First block number for this level, 0 for level 0 */

	// These values are valid for the top (smallest) level QMF only
	BlockingFormat blocking_format;		// typically BLOCKING_LEVEL
	CompressFormat compress_format;		// typically COMPRESS_UINT8 or COMPRESS_YIQ
	IEEE4	compression_factor;			// a number between 1 and 1000


	// These are SHARED entries by all QMFs. They all point to the same data,
	// and only the top level QMF allocates and frees these. The p_a_block points
	// to a single block (valid compression only);
	// all QMF's point to the same block. The p_block_offsets is
	// an single array for all levels. It is allocated at p_top_qmf, and freed
	// at p_top_qmf only.  The other levels point to offsets in the array
	// There is +1 block than actually present, to calculate the final block offset.
	// These offsets are relative to the start of the first block, not the start of the file
	UINT64	first_block_offset;			/* offset from the start of the file to the first block */	
	UINT64	*p_block_offsets;			/* offset for each block for this level of the file */
	NCS_FILE_HANDLE	outfile;					/* output file */
	UINT32	next_block_offset;			/* incremented during writing */
	UINT8	*p_a_block;					/* pointer to a single block during packing/unpacking */
	BOOLEAN bRawBlockTable;				/* used to indicated weather the block table was store as
										   RAW or compressed.  Required when freeing the memory
										   allocated for the Block Table in delete_qmf_levels */ //[19]
	BOOLEAN	bLowMemCompress;			/* Use "Low Memory" compression techniques [20]*/

	// COMPRESSION ONLY: nr_bands of the following pointers, which point into the
	// p_input_ll_line offset at [FILTERSIZE/2] for this QMF. Used for recursive calls. 
	IEEE4	**p_p_input_ll_line;		// pointer to nr_bands, a single input line needed for this level
	//
	// These buffers are not allocated for the largest (file level) QMF level
	// These are also ONLY allocated for compression, not for decompression. There is one
	// per band being compressed
	QmfLevelBandStruct	*p_bands;
	INT16	next_output_block_y_line;	// a number going from 0 to Y_BLOCK_SIZE-1
	//	The place to write the compressed sidebands for this level to. Each level has MAX_SIDEBAND files
	//	We don't encode the LL sideband image, so the [LL_SIDEBAND] item is normally unused except for debug
	char	*tmp_fname;					// temporary file to write this level's blocks, so we can set up block order
	NCS_FILE_HANDLE	tmp_file;					// file pointer to the above block image disk file

	//	if we are doing transmission encoding instead of blocked encoding
	IEEE4		scale_factor;			// scale factor, for decompression only
	UINT64		file_offset;			// offset to start of this level in the file

	//	[02] Addition information for NCS usage. Valid for the TopQmf (smallest level) only
	UINT8	*pHeaderMemImage;		// Pointer to in memory copy of header if not NULL
	UINT32	nHeaderMemImageLen;		// Length of the above in memory header structure

	ECWFILE	hEcwFile;				// input ECW, TopQmf level only, for decompression only
	BOOLEAN	bEcwFileOpen;			// TRUE if above file handle is valid (open), otherwise FALSE

	ECWFileInfoEx	*pFileInfo;			// Valid at top level only

	// Compression specific information
	struct EcwCompressionTask	*pCompressionTask;	// [15]
};

//
//	Region to decode/decompress within a compressed file for a given line.
//
//
// For a region, we keep local line buffers for each level within the region's depth. There
// may be less of these than QMF's, if we are extracting a reduced resolution view of the data.
//
// The upsampling logic results in 4x4 output values from 2x2 input values, so we keep
// line 0 and line 1 of each level's LL.  During processing, line 1 is rolled
// to line 1, and line 1 is requested from the smaller level recursively.
//
// Note that LH, HL and HH are loaded from the compressed file, LL is regenerated recursively
// from smaller levels.
//
// the line0 and line1 are +1 larger in X than required by this level; this handles
// different level sizes correctly. For example, we might expand expand a 11x12
// image up to a 22 x 23 image.

// The following returns an index into the line buffer, for the indicated
// band number (0..p_qmf->nr_bands), sideband (LL_SIDEBAND..HH_SIDEBAND) and line (0 or 1)
//
// NOTE WELL!! The "used_bands" is the value used for all band loops during decompression.
// This is so that (later) we can set up the decompress logic to uncompress only some bands.
// Only the unpack() routines will need to be modified for this to work. For now, though,
// we have to unpack all bands.

// Used to compute initial indexes for pointers
#define DECOMP_LEVEL_LINE01(p_level, band, sideband, line)	\
		((p_level)->buffer_ptr + (((2 * (band * MAX_SIDEBAND + sideband)) + (line))	* ((p_level)->level_size_x+2)))
// Used to compute index for a given band and subband
#define	DECOMP_INDEX	\
		(band * MAX_SIDEBAND + sideband)

typedef struct {
	UINT32	used_bands;
	IEEE4	**p_p_line0;			// [05] this level's LL, LH, HL and HH for line N+0. Must be signed
	IEEE4	**p_p_line1;			// [05] this level's LL, LH, HL and HH for line N+1. Must be signed
	IEEE4	**p_p_line1_ll_sideband;// [05] This level's LL sideband with x_reflection for line 1, all bands

	UINT32  start_read_lines;		// [11] starting read_lines state for ResetView()
	UINT32	read_lines;				// # of lines of above level to read. Starts as 2, then will be 0 or 1
	// The following is the next line of this level to read.
	// So a level with sidebands of 32x32 could have this number range from 0..31,
	// as this level would be called up to 64 times to generate the 0..64 lines for the larger level
	// In other words, this is from 0 .. (# lines-1) in the side bands for this level.
	UINT32	current_line;			// line currently being read at this level
	UINT32	start_line;				// [11] starting line for ResetView()
	// this is the next block line to read (can be from 0..y_block_size). Only valid if have_blocks = TRUE
	INT16	next_input_block_y_line;// a number going from 0 to Y_BLOCK_SIZE-1
	UINT8	have_blocks;			// true if the next Y block number is valid
	UINT32	start_x_block;			// first X block, from 0..p_pqm->x_size/p_qmf->x_block_size
	UINT32	x_block_count;			// count of X blocks across

	void	*p_x_blocks;			// pointer to a set of decompressions in progress for a set of X blocks
	UINT32	first_block_skip;		// how many symbols to skip at start of line in the first block in the set of X blocks
	UINT32	last_block_skip;		// how many symbols to skip at end of line in the last block in the X set

	QmfLevelStruct	*p_qmf;				// handy pointer to the QMF for this level
	struct qmf_region_struct *p_region;	// handy pointer to the region that contains these levels
	// the reflection amounts. Need to add these to start/end/size to get true size
	// Always 0 or 1, so can be shorter byte values
	UINT8	reflect_start_x, reflect_end_x;
	UINT8	reflect_start_y, reflect_end_y;
	// NOTE WELL: These start/end/size values EXCLUDE the reflection values
	UINT32	level_start_y, level_end_y, level_size_y;
	UINT32	level_start_x, level_end_x, level_size_x;
	UINT32	output_level_start_y, output_level_end_y, output_level_size_y;
	UINT32	output_level_start_x, output_level_end_x, output_level_size_x;

	IEEE4	*buffer_ptr;			// [05] Band/sideband/line[01] buffer.
									// for speed we allocate a single buffer, and point into it.
									// So a single malloc/free per level
} QmfRegionLevelStruct;

// One of these is created for each start_region()/end_region() call pair
typedef struct qmf_region_struct {
	QmfLevelStruct	*p_top_qmf;		// pointer to the smallest level in the QMF
	QmfLevelStruct	*p_largest_qmf;	// pointer to the largest level we have to read for this region resolution
	// region location and size to read
	UINT32	start_x, start_y;
	UINT32	end_x, end_y;
	UINT32	number_x, number_y;

	UINT32	random_value;			// [14] random value for texture generation
	// floating point versions of current line and incrememtn. This is because
	// we read power of 2 lines, but need to feed non-power of two lines to caller
	UINT32	read_line;				// non-zero if have to read line(s), 0 if not (duplicating line)
	//[22]
	IEEE8	current_line;			// the current output line being worked on
	IEEE8	start_line;				// [11] starting line for ResetView()
	IEEE8	increment_y;			// the increment of input lines to add for each output line. Will be <= 1.0
	IEEE8	increment_x;			// the increment of input pixels to add for each output pixel. Will be <= 1.0
	// recursive information. This array is p_largest_level->level+1 in size
	// The in_line[0|1] points are set up for the size of each level, so increase in size for each level
	QmfRegionLevelStruct	*p_levels;
	IEEE4	**p_p_ll_line;			// [05] the power of two size, INT32 format, output line. Must be signed
	IEEE4	*p_ll_buffer;			// [05] the buffer that the above indexes into
	UINT32	used_bands;				// bands actually read - may be different from p_qmf->nr_bands in future versions
	// The band_list is allocated and freed by the caller. We do NOT allocate or free it
	UINT32	nr_bands_requested;		// the number of bands requested from caller; may be differed to used_bands
	UINT32	*band_list;				// array of used_bands, indicating actual band numbers to use
	struct NCSFileViewStruct *pNCSFileView;	// [02] pointer to the NCS File View for this QMF Region
	BOOLEAN	bAddTextureNoise;		// [11] TRUE if add texture noise during decompression
	UINT32 nCounter;                // [21] to fix rounding error in erw_decompress_read_region_line_bil()
} QmfRegionStruct;


/*
**	QMF tree management functions
*/

// QMF tree management routines used by both build/extract

#ifdef ECW_COMPRESS	// [15]
extern NCSError setup_qmf_tree(QmfLevelStruct **pp_qmf, struct EcwCompressionTask *pCompressionTask,
						  UINT32 nBlockSizeX, UINT32 nBlockSizeY, 
						  UINT16 *num_levels, UINT32 x_size, UINT32 y_size, UINT32 nr_bands,
						  UINT32 nInputBands, CompressHint eCompressHint,
						  IEEE4 compression_factor, CompressFormat compress_format, int compress );
#endif

extern void delete_qmf_levels(QmfLevelStruct *p_qmf);
extern UINT32 get_qmf_tree_nr_blocks( QmfLevelStruct *p_top_level );

QmfLevelStruct *new_qmf_level(UINT32 nBlockSizeX, UINT32 nBlockSizeY,
						UINT16 level, UINT32 x_size, UINT32 y_size, UINT32 nr_bands,
						QmfLevelStruct *p_smaller_qmf, QmfLevelStruct *p_larger_qmf, int compress);

NCSError allocate_qmf_buffers(QmfLevelStruct *p_top_qmf, int compress);



// Data unpack routines

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
extern int unpack_ecw_block(QmfLevelStruct *pQmfLevel, UINT32 nBlockX, UINT32 nBlockY,
					 Handle *ppUnpackedECWBlock, UINT32	*pUnpackedLength,
					 UINT8 *pPackedBlock);
int align_ecw_block(NCSFile *pFile, NCSBlockId nBlockID,
					 Handle *ppAlignedECWBlock, UINT32	*pAlignedLength,
					 UINT8 *pPackedBlock, UINT32 nPackedLength);
#else
extern int unpack_ecw_block(QmfLevelStruct *pQmfLevel, UINT32 nBlockX, UINT32 nBlockY,
					 UINT8 **ppUnpackedECWBlock, UINT32	*pUnpackedLength,
					 UINT8 *pPackedECWBlock);
int align_ecw_block(NCSFile *pFile, NCSBlockId nBlockID, 
					 UINT8 **ppAlignedECWBlock, UINT32	*pAlignedLength,
					 UINT8 *pPackedBlock, UINT32 nPackedLength);
#endif //MACINTOSH

extern int	unpack_data(UINT8 **p_raw,
						UINT8  *p_packed, UINT32 raw_length,
						UINT8 nSizeOfEncodeFormat);


int unpack_init_lines( QmfRegionLevelStruct *p_level );

int unpack_start_line_block( QmfRegionLevelStruct *p_level, UINT32 x_block,
							UINT8 *p_packed_block, UINT32	lines_to_skip);

int	unpack_line( QmfRegionLevelStruct *p_level );

void unpack_finish_lines( QmfRegionLevelStruct *p_level );

void unpack_free_lines( QmfRegionLevelStruct *p_level );

/*
**	Encoder functions
*/

#ifdef ECW_COMPRESS

extern NCSError	build_qmf_compress_file(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile);

extern NCSError	build_qmf_level_qencode_line(QmfLevelStruct *p_qmf, UINT32 y, IEEE4 **p_p_ll_line);

extern NCSError qencode_qmf(QmfLevelStruct *p_top_qmf, int num_levels,
			  FLOAT compression_factor, NCS_FILE_HANDLE outfile);

extern NCSError	pack_data(QmfLevelStruct *p_top_qmf,
					  UINT8 **p_packed,	UINT32 *packed_length,
					  UINT8 *p_raw,		UINT32 raw_length, UINT8 symbol_size_hint, BOOLEAN bTryHuffman);


/*
**	File IO functions
*/

// writes the preamble header in the compressed ERW file
NCSError write_compressed_preamble(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile);
// convert the block table to LSB offsets
NCSError convert_block_table(QmfLevelStruct *p_top_qmf, NCS_FILE_HANDLE outfile);
// writes the level to the compressed ERW file
NCSError write_compressed_level(QmfLevelStruct *p_qmf, NCS_FILE_HANDLE outfile);

#endif	/* ECW_COMPRESS */

/*
**	Decoder functions
*/

// Visible functions that can be called

extern QmfLevelStruct *erw_decompress_open(
			char *p_input_filename,		// File to open
			UINT8	*pMemImage,			// if non-NULL, open the memory image not the file
			BOOLEAN bReadOffsets,		// TRUE if the client wants the block Offset Table
			BOOLEAN bReadMemImage );	// TRUE if the client wants a Memory Image of the Header
			

extern QmfRegionStruct *erw_decompress_start_region( QmfLevelStruct *p_top_qmf,
				 UINT32	nr_bands_requested,			// number of bands to read
				 UINT32	*p_band_list,				// list of bands to be read. Caller allocs/frees this
				 UINT32 start_x, UINT32 start_y,
				 UINT32 end_x, UINT32 end_y,
				 UINT32 number_x, UINT32 number_y);

typedef enum {														/**[17]**/
	NCSECW_READLINE_INVALID	= 0,	// invalid type					/**[17]**/
	NCSECW_READLINE_RGB		= 1,	// RGB Triplet					/**[17]**/
	NCSECW_READLINE_BGR		= 2,	// BGR Triplet					/**[17]**/
	NCSECW_READLINE_RGBA	= 3,	// RGBA 32-bit					/**[17]**/
	NCSECW_READLINE_BGRA	= 4,	// BGRA 32-bit					/**[17]**/
	NCSECW_READLINE_IEEE4	= 5,	// single band float			/**[17]**/
	NCSECW_READLINE_UINT8	= 6,	// single band unsigned 8bit	/**[17]**/
	NCSECW_READLINE_UINT16	= 7		// single band unsigned 16bit	/**[17]**/
} NCSEcwReadLineType;												/**[17]**/

int erw_decompress_read_region_line_bil ( QmfRegionStruct *p_region, UINT8  **p_p_output_line, NCSEcwReadLineType nOutputType); // [17]
int erw_decompress_read_region_line_rgb ( QmfRegionStruct *p_region, UINT8  *pRGBTriplets);
int erw_decompress_read_region_line_bgr ( QmfRegionStruct *p_region, UINT8  *pRGBTriplets);
int erw_decompress_read_region_line_rgba( QmfRegionStruct *p_region, UINT32 *pRGBAPixels);
int erw_decompress_read_region_line_bgra( QmfRegionStruct *p_region, UINT32 *pBGRAPixels);


void erw_decompress_end_region( QmfRegionStruct *p_region );
extern void erw_decompress_close( QmfLevelStruct *p_top_qmf );

// handy argument parsing for stand alone program
extern int setup_decompress(int argc, char *argv[],
			char **p_p_input_erw_filename,
			char **p_p_output_bil_filename);

// Internal functions. Do not call these

extern int qdecode_qmf_level_line( QmfRegionStruct *p_region, UINT16 level, UINT32 y_line,
								  IEEE4 **p_p_output_line);

/*
**	GUI functions (gui_io.c)
*/
void gui_percent( UINT32 percent );
int gui_cancel( void );
void gui_msg_ok( char *msg, char *title );


/*
**	ECW Header File IO routines
**	Use these only for small numbers of header bytes, as they are quite slow
*/

#ifdef ECW_COMPRESS		// [02] only compression needs the ermapper.lib library
int write_int8(UINT8   value, FILE *stream);
int write_int16(UINT16 value, FILE *stream);
int write_int32(UINT32 value, FILE *stream);
int write_int64(UINT64 value, FILE *stream);
int write_ieee4(IEEE4  value, FILE *stream);
#endif
/*
**	ECW File reading Abstraction Routines
**	These are non-ER Mapper specific (as ECW gets linked stand alone),
**	and CAN NOT use fopen() to due # of file limitations on platforms like Windows
*/

/*
**	The Unix vs WIN32 calls are so different, we wrap the reads with functions,
**	that return FALSE if all went well, and TRUE if there was an error.
**	Note that the ECWFILE *can* be NULL (windows madness), so you will have to
**	keep a separate variable to decide if the file is open or not.
*/
BOOLEAN EcwFileOpenForRead(char *szFilename, ECWFILE *pFile);		// Opens file for binary reading
BOOLEAN EcwFileClose(ECWFILE hFile);								// Closes file
BOOLEAN EcwFileRead(ECWFILE hFile, void *pBuffer, UINT32 nLength);	// reads nLength bytes into existing pBuffer
BOOLEAN EcwFileSetPos(ECWFILE hFile, UINT64 nOffset);				// Seeks to specified location in file
BOOLEAN EcwFileGetPos(ECWFILE hFile, UINT64 *pOffset);				// Returns current file position
BOOLEAN EcwFileReadUint8(ECWFILE hFile, UINT8 *sym);
BOOLEAN EcwFileReadUint16(ECWFILE hFile, UINT16 *sym16);
BOOLEAN EcwFileReadUint32(ECWFILE hFile, UINT32 *sym32);
BOOLEAN EcwFileReadUint64(ECWFILE hFile, UINT64 *sym);
BOOLEAN EcwFileReadIeee8(ECWFILE hFile, IEEE8 *fvalue);
BOOLEAN EcwFileReadIeee4(ECWFILE hFile, IEEE4 *fsym32);
void sread_ieee8(IEEE8 *sym, UINT8 *p_s);


UINT32 sread_int32(UINT8 *p_s);
UINT16 sread_int16(UINT8 *p_s);

/*
**	Memory functions
*/

extern char *check_malloc (int size);
extern void check_free (char *ptr);
extern FILE *check_fopen (char *filename, char *read_write_flag);
char *concatenate(char *string1, char *string2);

//	For MAC Port --

#ifdef MACINTOSH
extern void *vmalloc( size_t cb );
extern void *vcalloc( size_t c, size_t cb );
#else
#define vmalloc(p) malloc(p)
#define vcalloc(p,q) calloc(p,q)
#endif

#ifdef __cplusplus
}
#endif

#endif	// ECW_H
