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
** FILE:   	ecw_read.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Read line routines for ECW compressed image files
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
** [10] sns 04Jun99	Updated to use an internal Random function
** [11] sjc 20Jun00 Out-of-Order block fix
** [12] rdh 04Oct00 Added generic fucntion "erw_decompress_read_region_line" and provided wrapper functions for:
**					erw_decompress_read_region_line_rgb()	backward compatible
**					erw_decompress_read_region_line_bgr()	backward compatible
**					erw_decompress_read_region_line_rgba()	new 32-bit pixel UNIX etc (UINT32 interface)
**					erw_decompress_read_region_line_bgra()	new 32-bit pixel UNIX etc (UINT32 interface)
** [13] ddi 14nov00 Modified erw_decompress_read_region_line_bil() to take NCSEcwReadLineType parameter
** [14] rar	17-Jan-01 Mac port changes
** [15] sjc 21Mar01 Fixed YUV->green band calc - was "-" instead of "+"!
** [16] jmp 04Jul01 Change to ADD_TEXTURE_NOISE requested by Stuart. "If ( (value) > 1.0 )"
** [17] jx  12Feb04 nCounter is added to QmfRegionStruct to fix rounding error.
** [18] jx  17Feb04 changed varibles: current_line, start_line, increment_y and increment_x to IEEE8 to fix rounding error in erw_decompress_read_region_line_bil().
** [19] tfl 02May06 worked around a gcc optimisation problem
 *******************************************************/


#include "NCSDefs.h" // For bytes order macros
#include "NCSEcw.h"

#if !defined(_WIN32_WCE)
	#ifdef WIN32 //[14]
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif

#define RANDOM_MASK 0xd4c5c239				// [10] for random number mask

// [10] updated to use new inline random number generator
#define ADD_TEXTURE_NOISE(value) {				\
	register UINT32	nNoise;						\
	register UINT8	nSign;						\
	nNoise = p_region->random_value;			\
	nNoise = (nNoise & 1) ? (nNoise>>1)^RANDOM_MASK : nNoise >>1;	\
	if( !nNoise ) nNoise = 1;					\
	p_region->random_value = nNoise;			\
	nSign = (UINT8) nNoise;						\
	nNoise &= 0x1f;								\
	if( nNoise > 10 )							\
		nNoise = nNoise & 0x3;					\
	if( (value) > 1.0 )	{						\
		if( nSign & 0x20 )						\
			(value) -= nNoise;					\
		else									\
			(value) += nNoise;					\
	}											\
}

#define WRITE_MULTI_BYTE(pIn)				\
	if( pIn ) {								\
		rescale = *((pIn) + nTrueXoffset);	\
		if( rescale < 0 )					\
			*p_out++ = 0;					\
		else if( rescale > 255 )			\
			*p_out++ = 255;					\
		else								\
			FLT_TO_UINT8(*p_out++, rescale);\
	}										\
	else									\
		*p_out++ = 0;

//#define WRITE_RESCALE(pOut)					\
//	if( rescale < 0 )						\
//		*pOut++ = 0;						\
//	else if( rescale > 255 )				\
//		*pOut++ = 255;						\
//	else									\
//		FLT_TO_UINT8(*pOut++, rescale);
#define WRITE_RESCALE(pOut)					\
	{										\
		INT32 nRescale;						\
		FLT_TO_INT32(nRescale, rescale);	\
		if( nRescale < 0 )					\
			*pOut++ = 0;					\
		else if( nRescale > 255 )			\
			*pOut++ = 255;					\
		else								\
			*pOut++ = (UINT8)nRescale;				\
	}
//#define WRITE_RESCALE(pOut)					\
//	{										\
//		INT32 nRescale;						\
//		FLT_TO_INT32(nRescale, rescale);	\
//		nRescale &= 0xffffffff ^ (nRescale >> 31);		\
//		nRescale = (nRescale & 0xff) | (0x000000ff ^ ((nRescale - 255) >> 31));		\
//		*pOut++ = nRescale;				\
//	}

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line_bil().  Reads the next sequentional
**	line from the region, in bil format.
**
**	Returns non-zero if an error
**
**	Because floating point is very slow, we use fixed point scaled
**	integer maths to work out the X increment. The lower 32 bits
**	are the precision; the higher 32 bits the actual offset
**
**  [13] nOutputType is only relevant if the ECW file is COMPRESS_MULTI
**  
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

#ifdef POSIX
void gcc_optimisation_workaround(IEEE8 value); //[19]
#endif POSIX

int erw_decompress_read_region_line_bil( QmfRegionStruct *p_region, UINT8 **p_p_output_line, NCSEcwReadLineType nOutputType) /**[13]**/
{
	IEEE8	 y_line; //[18]
	UINT32	 nLineY;
	
	register UINT32	x;
	register UINT64 increment_x, x_offset;

	if( !p_region )
		return( 1 );	//	ERS_setup_error(ERS_RASTER_ERROR,
//									"\nerw_decompress_read_region_line: Region pointer is NULL"));
	
	y_line = p_region->current_line;
	nLineY = (UINT32) y_line;

	if(nLineY == (UINT32)p_region->start_line) {	/**[11]**/
		p_region->pNCSFileView->nNextDecodeMissID++;
	}

	while( p_region->read_line ) {
		if( qdecode_qmf_level_line( p_region, p_region->p_largest_qmf->level,
			(nLineY - p_region->read_line) + 1, p_region->p_p_ll_line) )
			return(1);
		p_region->read_line--;
	}


	/*
	**	Decode the data. Depends on the format data was encoded in
	*/
	switch( p_region->p_top_qmf->compress_format ) {
	default :
	case COMPRESS_UINT8: {		// input file is single band, UINT8, greyscale
			register UINT8 *p_out = p_p_output_line[0];
			register IEEE4 *p_in = p_region->p_p_ll_line[0];
			// copy to output line
			x = p_region->number_x;
			x_offset = 0;		// actually 0 << 32
			// This gives us 1/(2^16) accuracy per line, which should be fine
			increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

			FLT_TO_INT_INIT();
			while(x--) {
				register IEEE4 rescale;
				rescale = *(p_in + (UINT32) (x_offset >> 32));
				if( p_region->bAddTextureNoise ) 	/* [08] */
					ADD_TEXTURE_NOISE(rescale);

				WRITE_RESCALE(p_out);
//				if( rescale < (IEEE4) 0.0 )
//					*p_out++ = 0;
//				else if( rescale > (IEEE4) 255.0 )
//					*p_out++ = 255;
//				else
//					FLT_TO_UINT8(*p_out++, rescale);
//					*p_out++ = (UINT8) rescale;
				x_offset += increment_x;
			}
			FLT_TO_INT_FINI();
		}
		break;

	// FIXME!! Assumes that all bands were read, and picks out the needed bands.
	// need to work on pack.c to be able to extract selected bands.
//	case COMPRESS_YUV:		// DEBUG to return YUV directly
	case COMPRESS_MULTI: {
			UINT32	band;

			FLT_TO_INT_INIT();			// [04]
			for( band = 0; band < p_region->nr_bands_requested; band++ ) {
				register UINT8 *p_out = p_p_output_line[band];
				register IEEE4 *p_in = p_region->p_p_ll_line[p_region->band_list[band]];

				// copy to output line
				x = p_region->number_x;
				x_offset = 0;		// actually 0 << 32
				// This gives us 1/(2^16) accuracy per line, which should be fine
				increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

				while(x--) {
					register IEEE4 rescale;
					rescale = *(p_in + (UINT32) (x_offset >> 32));

					switch(nOutputType) {									/**[13]**/
					case NCSECW_READLINE_IEEE4:								/**[13]**/
						*((IEEE4 *)p_out) = rescale;						/**[13]**/
						p_out += sizeof(IEEE4);								/**[13]**/
						break;												/**[13]**/
					case NCSECW_READLINE_UINT16:							/**[13]**/
	//					*((UINT16 *)p_out) = (UINT16) rescale;				/**[13]**/
						if( rescale < 0 )									/**[13]**/
							*((UINT16 *)p_out) = 0;							/**[13]**/
						else if( rescale > 65535 )							/**[13]**/
							*((UINT16 *)p_out) = 65535;						/**[13]**/
						else												/**[13]**/
							*((UINT16 *)p_out) = (UINT16) rescale;			/**[13]**/
	//						*p_out++ = (UINT8) rescale;						/**[13]**/
						p_out += sizeof(UINT16);							/**[13]**/
						break;												/**[13]**/
					case NCSECW_READLINE_UINT8:								/**[13]**/
					default:												/**[13]**/
						WRITE_RESCALE(p_out);
//						if( rescale < 0 )									/**[13]**/
//							*p_out++ = 0;									/**[13]**/
//						else if( rescale > 255 )							/**[13]**/
//							*p_out++ = 255;									/**[13]**/
//						else												/**[13]**/
//							FLT_TO_UINT8(*p_out++, rescale);	// [04]		/**[13]**/
	//						*p_out++ = (UINT8) rescale;						/**[13]**/
					}														/**[13]**/

					x_offset += increment_x;
				}
			}
			FLT_TO_INT_FINI();			// [04]
		 }
		break;

	// Input must always be RGB 3 band. Converts to YIQ for better compression
	// Think of YIQ mode as RGB mode. The only time you would not use this
	// for a 3 band image is where the image is not color, and there is no
	// correlation between the bands (so the bands are not R,G,B).
	// We are using the JPEG standard YUV here (digital YUV). See:
	//	http://icib.igd.fhg.de/icib/it/iso/is_10918-1/pvrg-descript/chapter2.5.html
	case COMPRESS_YUV: {
			register IEEE4 *p_in_y = p_region->p_p_ll_line[0];	// always YIQ input
			register IEEE4 *p_in_u = p_region->p_p_ll_line[1];
			register IEEE4 *p_in_v = p_region->p_p_ll_line[2];
			register IEEE4 y,u,v;

			if( p_region->nr_bands_requested == 3 ) {
				// RGB, so optimize for this
				register UINT8 *p_out_r = p_p_output_line[0];		// always RGB output
				register UINT8 *p_out_g = p_p_output_line[1];
				register UINT8 *p_out_b = p_p_output_line[2];
				// copy to output line
				x = p_region->number_x;
				x_offset = 0;		// actually 0 << 32
				// This gives us 1/(2^16) accuracy per line, which should be fine
				increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

				FLT_TO_INT_INIT();			// [04]
				while(x--) {
					register IEEE4 rescale;
					y = *(p_in_y + (UINT32) (x_offset >> 32));
					u = *(p_in_u + (UINT32) (x_offset >> 32));
					v = *(p_in_v + (UINT32) (x_offset >> 32));
					if( p_region->bAddTextureNoise ) 	/* [08] */
						ADD_TEXTURE_NOISE(y);

					rescale = y + ((IEEE4) 1.402 * v);
					WRITE_RESCALE(p_out_r);
//					if( rescale < 0 )
//						*p_out_r++ = 0;
//					else if( rescale > 255 )
//						*p_out_r++ = 255;
//					else
//						FLT_TO_UINT8(*p_out_r++, rescale);	// [04]
//						*p_out_r++ = (UINT8) rescale;

					rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
					WRITE_RESCALE(p_out_g);
//					if( rescale < 0 )
//						*p_out_g++ = 0;
//					else if( rescale > 255 )
//						*p_out_g++ = 255;
//					else
//						FLT_TO_UINT8(*p_out_g++, rescale);	// [04]
//						*p_out_g++ = (UINT8) rescale;

					rescale = y + ((IEEE4) 1.772 * u);
					WRITE_RESCALE(p_out_b);
//					if( rescale < 0 )
//						*p_out_b++ = 0;
//					else if( rescale > 255 )
//						*p_out_b++ = 255;
//					else
//						FLT_TO_UINT8(*p_out_b++, rescale);	// [04]
//						*p_out_b++ = (UINT8) rescale;

					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			// [04]

			}
			else if( p_region->nr_bands_requested == 1 ) { 
				// greyscale, so optimize for this
				register UINT8 *p_out = p_p_output_line[0];
				register UINT32 band  = p_region->band_list[0];
				// copy to output line
				x = p_region->number_x;
				x_offset = 0;		// actually 0 << 32
				// This gives us 1/(2^16) accuracy per line, which should be fine
				increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

				FLT_TO_INT_INIT();			// [04]
				while(x--) {
					register IEEE4 rescale;
					y = *(p_in_y + (UINT32) (x_offset >> 32));
					u = *(p_in_u + (UINT32) (x_offset >> 32));
					v = *(p_in_v + (UINT32) (x_offset >> 32));
					switch( band ) {
						default :
						case 0 :	/* RED */
							rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	/* GREEN */
							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	/* BLUE */
							rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out);
//					if( rescale < 0 )
//						*p_out++ = 0;
//					else if( rescale > 255 )
//						*p_out++ = 255;
//					else
//						FLT_TO_UINT8(*p_out++, rescale);	// [04]
//						*p_out++ = (UINT8) rescale;
					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			// [04]

			}
			else {
				// Some random 2 band combination of bands. We will just ramble through them
				// greyscale, so optimize for this
				register UINT8 *p_out0 = p_p_output_line[0];
				register UINT8 *p_out1 = p_p_output_line[1];
				register UINT32 band0  = p_region->band_list[0];
				register UINT32 band1  = p_region->band_list[1];
				// copy to output line
				x = p_region->number_x;
				x_offset = 0;		// actually 0 << 32
				// This gives us 1/(2^16) accuracy per line, which should be fine
				increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

				FLT_TO_INT_INIT();			// [04]
				while(x--) {
					register IEEE4 rescale;
					y = *(p_in_y + (UINT32) (x_offset >> 32));
					u = *(p_in_u + (UINT32) (x_offset >> 32));
					v = *(p_in_v + (UINT32) (x_offset >> 32));
					switch( band0 ) {
						default :
						case 0 :	rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out0);
//					if( rescale < 0 )
//						*p_out0++ = 0;
//					else if( rescale > 255 )
//						*p_out0++ = 255;
//					else
//						FLT_TO_UINT8(*p_out0++, rescale);	// [04]
//						*p_out0++ = (UINT8) rescale;

					switch( band1 ) {
						default :
						case 0 :	rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out1);
//					if( rescale < 0 )
//						*p_out1++ = 0;
//					else if( rescale > 255 )
//						*p_out1++ = 255;
//					else
//						FLT_TO_UINT8(*p_out1++, rescale);	// [04]
//						*p_out1++ = (UINT8) rescale;

					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			// [04]
			}
		}
		break;
	}	/* end switch compression format */


	/*
	** work out if next line is duplicate or read
	*/
	p_region->nCounter++;//[17]
	y_line = p_region->start_line +  p_region->increment_y * p_region->nCounter; //[17]

	/*
	** without this line subsequent fp calculations are optimised by gcc producing an 
	** incorrect result for p_region->read_line, manifesting itself as an apparent 
	** shift in vertical resampling
	*/
#ifdef POSIX
	gcc_optimisation_workaround(y_line); //[19]
#endif

	if( (UINT32) y_line <= (UINT32) p_region->current_line )
		p_region->read_line = 0;	// we don't need to read a line
	else {
		// in a rare case, the user might request 1:1 of X output, but N:1 of Y output.
		// The only way to handle this is to read multiple lines for the next read,
		// and return up the last line read. So we compute the number of lines to read
		// here, rather than just one line (which is what will normally be requested)

		p_region->read_line = (UINT32) y_line - (UINT32) p_region->current_line;
	}
	p_region->current_line = y_line;
 
   return(0);
}


/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line().  Reads the next sequentional
**	line from the region.
**
**	Returns non-zero if an error
**
**	The image input is always converted to BGRA triplets
**	(for example a greyscale image will be output to R,G, B and Alpha)
**	
**	Added for platforms that need 32 bit RGBA images to blit from (UNIX X)
**  For now, alpha is not supported and will be 0. Ideally, before texturing
**  occurs, you could set the alpha to be the background color and get transparent images.
**
**	**[12]**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

int erw_decompress_read_region_line( QmfRegionStruct *p_region, UINT8 *pPixelBundle, NCSEcwReadLineType nOutputType)
{
	IEEE8	y_line;   //[18]
	UINT32	 nLineY;

	register UINT32	x;
	register UINT64 increment_x, x_offset;
	register UINT8 *p_out = pPixelBundle;

	if ((nOutputType != NCSECW_READLINE_RGB) && (nOutputType != NCSECW_READLINE_BGR) &&
		(nOutputType != NCSECW_READLINE_RGBA) && (nOutputType != NCSECW_READLINE_BGRA))
		return(1);
	
	if( !p_region )
		return( 1 );

	y_line = p_region->current_line;
	nLineY = (UINT32) y_line;

	if(nLineY == (UINT32)p_region->start_line) {		/**[11]**/
		p_region->pNCSFileView->nNextDecodeMissID++;
	}

	while( p_region->read_line ) {
		if( qdecode_qmf_level_line( p_region, p_region->p_largest_qmf->level,
			(nLineY - p_region->read_line) + 1, p_region->p_p_ll_line) )
			return(1);
		p_region->read_line--;
	}

	// copy to output line
	x = p_region->number_x;
	x_offset = 0;		// actually 0 << 32
	
	// This gives us 1/(2^16) accuracy per line, which should be fine
	increment_x = ((UINT64) ((p_region->increment_x) * 0x10000) << 16);

	//	Decode the data. Depends on the format data was encoded in
	switch( p_region->p_top_qmf->compress_format ) {
	default :
	case COMPRESS_UINT8: {		// input file is single band, UINT8, greyscale
			register IEEE4 *p_in = p_region->p_p_ll_line[0];
			FLT_TO_INT_INIT();

			while(x--) {
				register IEEE4 rescale;
				rescale = *(p_in + (UINT32) (x_offset >> 32));
				if( p_region->bAddTextureNoise ) 	/**[08]**/
					ADD_TEXTURE_NOISE(rescale);

				WRITE_RESCALE(p_out);
				WRITE_RESCALE(p_out);
				WRITE_RESCALE(p_out);
//				if( rescale < 0 ) {
//					*p_out++ = 0;
//					*p_out++ = 0;
//					*p_out++ = 0;
//				} else if( rescale > 255 ) {
//					*p_out++ = 255;
//					*p_out++ = 255;
//					*p_out++ = 255;
//				} else {
//					FLT_TO_UINT8(*p_out++, rescale);	// greyscale to RGB
//					FLT_TO_UINT8(*p_out++, rescale);
//					FLT_TO_UINT8(*p_out++, rescale);
//				}
				if ((nOutputType == NCSECW_READLINE_RGBA) || (nOutputType == NCSECW_READLINE_BGRA))
					*p_out++ = 0;	// alpha value
				
				x_offset += increment_x;
			}
			FLT_TO_INT_FINI();
		}
		break;

	// They could have requested any number of bands. We always use the first bands
	// requested, into RGB, and zero-fill if there were not enough bands
	// We NEVER add texture noise to MULTI compressed files
	case COMPRESS_MULTI: {
			UINT32	band =  p_region->nr_bands_requested;
			register IEEE4 *p_in_red, *p_in_green, *p_in_blue;
			p_in_red   = ( band > 0 ? p_region->p_p_ll_line[p_region->band_list[0]] : NULL);
			p_in_green = ( band > 1 ? p_region->p_p_ll_line[p_region->band_list[1]] : NULL);
			p_in_blue  = ( band > 2 ? p_region->p_p_ll_line[p_region->band_list[2]] : NULL);

			FLT_TO_INT_INIT();			/**[04]**/
			while(x--) {
				register UINT32	nTrueXoffset = (UINT32) (x_offset >> 32);
				register IEEE4 rescale;

				if ((nOutputType == NCSECW_READLINE_RGB) || (nOutputType == NCSECW_READLINE_RGBA)) {
					WRITE_MULTI_BYTE(p_in_red)
					WRITE_MULTI_BYTE(p_in_green)
					WRITE_MULTI_BYTE(p_in_blue)
				} else if ((nOutputType == NCSECW_READLINE_BGR) || (nOutputType == NCSECW_READLINE_BGRA)) {
					WRITE_MULTI_BYTE(p_in_blue)
					WRITE_MULTI_BYTE(p_in_green)
					WRITE_MULTI_BYTE(p_in_red)
				}
	
				if ((nOutputType == NCSECW_READLINE_RGBA) || (nOutputType == NCSECW_READLINE_BGRA))
					*p_out++ = 0;	// alpha value

				x_offset += increment_x;
			}
			FLT_TO_INT_FINI();			/**[04]**/
		}
		break;

	// Input must always be RGB 3 band. Converts to YIQ for better compression
	// Think of YIQ mode as RGB mode. The only time you would not use this
	// for a 3 band image is where the image is not color, and there is no
	// correlation between the bands (so the bands are not R,G,B).
	// We are using the JPEG standard YUV here (digital YUV). See:
	//	http://icib.igd.fhg.de/icib/it/iso/is_10918-1/pvrg-descript/chapter2.5.html
	case COMPRESS_YUV: {
			register IEEE4 *p_in_y = p_region->p_p_ll_line[0];	// always YIQ input
			register IEEE4 *p_in_u = p_region->p_p_ll_line[1];
			register IEEE4 *p_in_v = p_region->p_p_ll_line[2];
			register IEEE4 y,u,v;

			if( p_region->nr_bands_requested == 3 ) { // RGB, so optimize for this
				FLT_TO_INT_INIT();			/**[04]**/
				while(x--) {
					register IEEE4 rescale;
					register UINT32	nTrueXoffset = (UINT32) (x_offset >> 32);
					y = *(p_in_y + nTrueXoffset);
					u = *(p_in_u + nTrueXoffset);
					v = *(p_in_v + nTrueXoffset);
					if( p_region->bAddTextureNoise ) 	/**[08]**/
					//	ADD_TEXTURE_NOISE(y);
					{
	register UINT32	nNoise;						
	register UINT8	nSign;						
	nNoise = p_region->random_value;			
	nNoise = (nNoise & 1) ? (nNoise>>1)^RANDOM_MASK : nNoise >>1;	
	if( !nNoise ) nNoise = 1;					
	p_region->random_value = nNoise;			
	nSign = (UINT8) nNoise;						
	nNoise &= 0x1f;								
	if( nNoise > 10 )							
		nNoise = nNoise & 0x3;					
	if( (y) > 1.0 )	{						
		if( nSign & 0x20 )						
			(y) -= nNoise;					
		else									
			(y) += nNoise;					
	}											
}
					switch (nOutputType) {

						case NCSECW_READLINE_RGB:
							rescale = y + ((IEEE4) 1.402 * v);
							WRITE_RESCALE(p_out);				// RED

							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
							WRITE_RESCALE(p_out);				// GREEN

							rescale = y + ((IEEE4) 1.772 * u);
							WRITE_RESCALE(p_out);				// BLUE
						break;

						case NCSECW_READLINE_RGBA:
							rescale = y + ((IEEE4) 1.402 * v);
							WRITE_RESCALE(p_out);				// RED

							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
							WRITE_RESCALE(p_out);				// GREEN

							rescale = y + ((IEEE4) 1.772 * u);
							WRITE_RESCALE(p_out);				// BLUE

							*p_out++ = 0;					// ALPHA
						break;

						case NCSECW_READLINE_BGR:
							rescale = y + ((IEEE4) 1.772 * u);
							WRITE_RESCALE(p_out);				// BLUE

							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
							WRITE_RESCALE(p_out);				// GREEN

							rescale = y + ((IEEE4) 1.402 * v);
							WRITE_RESCALE(p_out);				// RED
						break;

						case NCSECW_READLINE_BGRA:
							rescale = y + ((IEEE4) 1.772 * u);
							WRITE_RESCALE(p_out);				// BLUE

							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
							WRITE_RESCALE(p_out);				// GREEN

							rescale = y + ((IEEE4) 1.402 * v);
							WRITE_RESCALE(p_out);				// RED

							*p_out++ = 0;                                   // ALPHA
						break;
					}	

#ifdef NOTDEF
					if ((nOutputType == NCSECW_READLINE_RGB) || (nOutputType == NCSECW_READLINE_RGBA)) {
						rescale = y + ((IEEE4) 1.402 * v);
						WRITE_RESCALE(p_out);				// RED

						rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						WRITE_RESCALE(p_out);				// GREEN

						rescale = y + ((IEEE4) 1.772 * u);
						WRITE_RESCALE(p_out);				// BLUE
					} else if ((nOutputType == NCSECW_READLINE_BGR) || (nOutputType == NCSECW_READLINE_BGRA)) {
						rescale = y + ((IEEE4) 1.772 * u);
						WRITE_RESCALE(p_out);				// BLUE

						rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						WRITE_RESCALE(p_out);				// GREEN

						rescale = y + ((IEEE4) 1.402 * v);
						WRITE_RESCALE(p_out);				// RED
					}

					if ((nOutputType == NCSECW_READLINE_RGBA) || (nOutputType == NCSECW_READLINE_BGRA))
						*p_out++ = 0;	// alpha value
#endif

					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			/**[04]**/

			} else if ( p_region->nr_bands_requested == 1 ) {	// greyscale, so optimize for this
				register UINT32 band  = p_region->band_list[0];

				FLT_TO_INT_INIT();			/**[04]**/
				while(x--) {
					register IEEE4 rescale;
					register UINT32	nTrueXoffset = (UINT32) (x_offset >> 32);
					y = *(p_in_y + nTrueXoffset);
					u = *(p_in_u + nTrueXoffset);
					v = *(p_in_v + nTrueXoffset);
					switch( band ) {
						default :
						case 0 :	// RED 
							rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	// GREEN 
							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	// BLUE 
							rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out);
					WRITE_RESCALE(p_out);
					WRITE_RESCALE(p_out);
//					if ( rescale < 0 ) {
//						*p_out++ = 0;
//						*p_out++ = 0;
//						*p_out++ = 0;
//					} else if ( rescale > 255 ) {
//						*p_out++ = 255;
//						*p_out++ = 255;
//						*p_out++ = 255;
//					}else {
//						FLT_TO_UINT8(*p_out++, rescale);	/**[04]**/
//						FLT_TO_UINT8(*p_out++, rescale);	/**[04]**/
//						FLT_TO_UINT8(*p_out++, rescale);	/**[04]**/
//					}

					if ((nOutputType == NCSECW_READLINE_RGBA) || (nOutputType == NCSECW_READLINE_BGRA))
						*p_out++ = 0;	// alpha value
					
					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			/**[04]**/
			} else {
				// Some random 2 band combination of bands. We will just ramble through them
				// greyscale, so optimize for this
				// BLUE is set to zero in this mode
				register UINT32 band0  = p_region->band_list[0];
				register UINT32 band1  = p_region->band_list[1];

				FLT_TO_INT_INIT();			/**[04]**/
				while(x--) {
					register IEEE4 rescale;
					register UINT32	nTrueXoffset = (UINT32) (x_offset >> 32);
					y = *(p_in_y + nTrueXoffset);
					u = *(p_in_u + nTrueXoffset);
					v = *(p_in_v + nTrueXoffset);
					
					if ((nOutputType == NCSECW_READLINE_BGR) || (nOutputType == NCSECW_READLINE_BGRA))
						*p_out++ = 0;		// set BLUE to zero (first UINT8 if BGR or BGRA)
					
					switch( band0 ) {
						default :
						case 0 :	// RED
							rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	// GREEN
							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	// BLUE
							rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out);

					switch( band1 ) {
						default :
						case 0 :	// RED
							rescale = y + ((IEEE4) 1.402 * v);
						break;
						case 1 :	// GREEN 
							rescale = y + ((IEEE4) -0.34414 * u) + ((IEEE4) -0.71414 * v);
						break;
						case 2 :	// BLUE
							rescale = y + ((IEEE4) 1.772 * u);
						break;
					}
					WRITE_RESCALE(p_out);

					if ((nOutputType == NCSECW_READLINE_RGB) || (nOutputType == NCSECW_READLINE_RGBA))
						*p_out++ = 0;		// set BLUE to zero (last UINT8 if RGB, third UINT8 if RGBA)

					if ((nOutputType == NCSECW_READLINE_RGBA) || (nOutputType == NCSECW_READLINE_BGRA))
						*p_out++ = 0;		// alpha value

					x_offset += increment_x;
				}
				FLT_TO_INT_FINI();			/**[04]**/
			}
		}
		break;
	}	// end switch compression format

	// work out if next line is duplicate or read
	p_region->nCounter++; //[17]
	y_line = p_region->start_line +  p_region->increment_y * p_region->nCounter;
#ifdef POSIX
	gcc_optimisation_workaround(y_line); //[19]
#endif

	if( (UINT32) y_line <= (UINT32) p_region->current_line )
		p_region->read_line = 0;	// we don't need to read a line
	else {
		// in a rare case, the user might request 1:1 of X output, but N:1 of Y output.
		// The only way to handle this is to read multiple lines for the next read,
		// and return up the last line read. So we compute the number of lines to read
		// here, rather than just one line (which is what will normally be requested)
		p_region->read_line = (UINT32) y_line - (UINT32) p_region->current_line;
	}
	
	p_region->current_line = y_line; //[17]
  
   return(0);
}


/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line_rgb().  Wrapper function
**	for erw_decompress_read_region_line().
**	
**  Reads the next sequentional	line from the region.
**
**	Returns non-zero if an error
**
**	The image input is always converted to RGB triplets
**	(for example a greyscale image will be output to R,G and B)
**
**	**[12]**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
int erw_decompress_read_region_line_rgb( QmfRegionStruct *p_region, UINT8 *pRGBTriplet)
{
	return (erw_decompress_read_region_line(p_region, pRGBTriplet, NCSECW_READLINE_RGB));
}


/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line_bgr().  Wrapper function
**	for erw_decompress_read_region_line().
**	
**  Reads the next sequentional	line from the region.
**
**	Returns non-zero if an error
**
**	The image input is always converted to BGR triplets
**	(for example a greyscale image will be output to B,G and R)
**
**	**[12]**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
int erw_decompress_read_region_line_bgr( QmfRegionStruct *p_region, UINT8 *pBGRTriplet)
{
	return (erw_decompress_read_region_line(p_region, pBGRTriplet, NCSECW_READLINE_BGR));
}


/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line_rgba().  Wrapper function
**	for erw_decompress_read_region_line().
**	
**  Reads the next sequentional	line from the region.
**
**	Returns non-zero if an error
**
**	The image input is always converted to RGBA UINT32
**	(for example a greyscale image will be output to R,G,B and A)
**
**	**[12]**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
int erw_decompress_read_region_line_rgba( QmfRegionStruct *p_region, UINT32 *pRGBAPixel)
{
	return (erw_decompress_read_region_line(p_region, (UINT8*)pRGBAPixel, NCSECW_READLINE_RGBA));
}


/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	
**
**	erw_decompress_read_region_line_bgra().  Wrapper function
**	for erw_decompress_read_region_line().
**	
**  Reads the next sequentional	line from the region.
**
**	Returns non-zero if an error
**
**	The image input is always converted to BGRA UINT32
**	(for example a greyscale image will be output to B,G,R and A)
**
**	**[12]**
**
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/
int erw_decompress_read_region_line_bgra( QmfRegionStruct *p_region, UINT32 *pBGRAPixel)
{
	return (erw_decompress_read_region_line(p_region, (UINT8*)pBGRAPixel, NCSECW_READLINE_BGRA));
}

#ifdef POSIX
void gcc_optimisation_workaround(IEEE8 value) {} //[19]
#endif //POSIX
