/*
 * Copyright (c) 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#ifndef tiff_format_h
#define tiff_format_h

/*
 * Define constants for identifying different file formats.
 */

#define COMPRESS_MAGIC_NUM 	0x1f9d 	

#define TIFF1_MAGIC_NUM 0x4d4d /* TIFF with BIGENDIAN byte order */
#define TIFF2_MAGIC_NUM 0x4949 /* TIFF with LITLEENDIAN byte order */
#define SUN_MAGIC_NUM	0x59a6	/* Sun rasterfile (0x59a66a95) */
#define BM_MAGIC_NUM	'!'	/* bm format (!!) */
#define PBM_MAGIC_NUM	'P'	/* pbm file (P1) */
#define ATK_MAGIC_NUM	'\\'	/* ATK file (\begindata{raster,length}) */
#define MP_MAGIC_NUM	'\0'	/* MacPaint (titlength in short) */
#define X11_MAGIC_NUM	'#'	/* X bitmaps (#define) */
#define PCX_MAGIC_NUM	0xa	/* PCX, PC Paintbrush files */
#define IFF_MAGIC_NUM	'F'	/* Amiga IFF format ("FORM") */
#define GIF_MAGIC_NUM	'G'	/* GIF format (CompuServe) */
#define RLE_MAGIC_NUM	0x52	/* Utah RLE file (0x52 0xcc) */

#endif
