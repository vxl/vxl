/********************************************************
** Copyright 2004 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/include/NCSGeoTIFFBoxUtil.h $
** CREATED:  12/07/2004 3:27:34 PM
** AUTHOR:   Tom Lynch
** PURPOSE:  Static GeoTIFF file values for JPEG 2000 GeoTIFF UUID Box (GeoJP2)
**			 GeoTIFF related #defines
**			 Utility CNCSAffineTransform class definition
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
**			 [01]	08Jul04	tfl		Changes to write GeoJP2 headers correctly
**			 [02]   03Aug04 tfl	    More GeoTIFF Box header changes
**			 [03]	22Nov04 tfl		Added photometric interpretation tag to GeoJP2 data to fix ERDAS load
**           [04]   25Jan04 tfl     Added GeoTIFFMiddleData and reworked default tag/IFD content
 *******************************************************/

#ifndef NCSGEOTIFFBOXUTIL_H
#define NCSGEOTIFFBOXUTIL_H

#define	TIFF_BIGENDIAN_CODE		0x4949	//Two byte TIFF header codes flagging endianness
#define TIFF_LITTLEENDIAN_CODE	0x4d4d	
#define TIFF_IDENTITY_CODE		0x2a	//Further magic number identifying file as a TIFF (42)

#ifndef ECW_MAX_PROJECTION_LEN
#define ECW_MAX_PROJECTION_LEN 16
#endif

#define NUM_IFD_ENTRIES 0x0e
#define NUM_IFD_ENTRIES_OFFSET	24
#define STRIP_BYTE_COUNTS_TAG_OFFSET_OFFSET		82
#define XRESOLUTION_TAG_OFFSET_OFFSET			118
#define YRESOLUTION_TAG_OFFSET_OFFSET			130

#define GEOTIFF_HEADER_LENGTH_BYTES				10
#define STRIP_BYTE_COUNTS_VALUE_LENGTH_BYTES	1
#define RESOLUTION_VALUE_LENGTH_BYTES			8
#define TIFF_TAG_LENGTH_BYTES					12
#define GEOKEY_LENGTH_BYTES						8
#define IFD_TERMINATOR_LENGTH_BYTES				4
#define BASE_NUMBER_TIFF_TAGS					10
//This static array contains most of the default byte values in the GeoTIFF box
static UINT8 GeoTIFFInitialData[] =
{
					0xb1,0x4b,0xf8,0xbd,0x8,0x3d,0x4b,0x43,0xa5,0xae,0x8c,0xd7,0xd5,0xa6,0xce,0x3,//UUID

					//GeoTIFF file header
					0x49,0x49,//TIFF_BIGENDIAN_CODE (in accordance with GeoJP2 spec)
					0x2a,0x00,//TIFF_IDENTITY_CODE	(42)
					0x08,0x00,0x00,0x00,//Offset of first IFD (8 bytes so follows on immediately)

					//First IFD
					NUM_IFD_ENTRIES,0x00,//0x0e == 14 IFD entries: this might vary based on output file 

					//Entry 0
					0x00,0x01,//TIFF tag value == 256, image width
					0x03,0x00,//Type of field == 3, UINT16
					0x01,0x00,0x00,0x00,//Only one such value
					0x01,0x00,0x00,0x00,//Width is 1

					//Entry 1
					0x01,0x01,//TIFF tag value == 257, image height (otherwise same as above)
					0x03,0x00,
					0x01,0x00,0x00,0x00,
					0x01,0x00,0x00,0x00,

					//Entry 2
					0x03,0x01,//TIFF tag value == 259, compression technique
					0x03,0x00,
					0x01,0x00,0x00,0x00,
					0x01,0x00,0x00,0x00,//no compression used

					//Entry 3
					0x06,0x01,		//Photometric Interpretation [03]
					0x03,0x00, 		//Type is SHORT
					0x01,0x00,0x00,0x00,
					0x01,0x00,0x00,0x00, 	//value 0x01 == BlackIsZero

					//Entry 4
					0x11,0x01,//TIFF tag value == 273, strip offsets
					0x04,0x00,//Type is UINT32 (TIFF_LONG)
					0x01,0x00,0x00,0x00,
					0xb6,0x00,0x00,0x00,//can guarantee offset b6

                    //Entry 5
					0x16,0x01,//TIFF tag value == 0x116, RowsPerStrip
					0x03,0x00,
					0x01,0x00,0x00,0x00,
					0x01,0x00,0x00,0x00,

					//Entry 6
					0x17,0x01,//TIFF tag value == 0x117, StripByteCounts
					0x04,0x00,
					0x01,0x00,0x00,0x00,
					0x01,0x00,0x00,0x00,

                    //Entry 7
					0x1a,0x01,//TIFF tag value = 0x11a, XResolution
					0x05,0x00,
					0x01,0x00,0x00,0x00,
					0xb7,0x00,0x00,0x00,//can guarantee offset b7 (immediately after 0x111 value)

                    //Entry 8
                    0x1b,0x01,//TIFF tag value = 0x11b, YResolution
					0x05,0x00,
					0x01,0x00,0x00,0x00,
					0xbf,0x00,0x00,0x00,//can guarantee offset bf (immediately after 0x1a value)

                    //Entry 9
                    0x28,0x01,//TIFF tag value = 0x128, ResolutionUnit
                    0x03,0x00,
                    0x01,0x00,0x00,0x00,
                    0x01,0x00,0x00,0x00

                    //Before outputting to file, it is necessary to ensure that the correct
                    //offset values are placed in TIFF tags 0x111, 0x11a and 0x11b, and the
                    //correct number of IFD entries is noted at the head of the IFD.  After
                    //outputting these default tags we see either a ModelTiePoint tag, a
                    //ModelTransformation OR a ModelPixelScale tag, a GeoKeyDirectory tag and
                    //a GeoASCIIParams tag (for citation) - the tags do not appear in the order
                    //just mentioned.

                    //They are immediately followed by some more default data, then the tie
                    //point and transformation/pixelscale values, then the GeoKey values, then
                    //the citation.  Because the number of additional tags is always four, it's
                    //possible to work out the right offset values for the 0x111, 0x11a and 0x11b
                    //tags at start, and the number of IFD entries.  So these don't change.
                    
                    //There follow four tags, which are either
                    //ModelPixelScale-ModelTiePoint-GeoKeyDirectory-GeoASCIIParams, OR
                    //ModelTiePoint-ModelTransformation-GeoKeyDirectory-GeoASCIIParams
                    
                    //immediately after these comes the fixed data in GeoTIFFMiddleData, then the 
                    //actual tag values, which contain the meat of the georeferencing.
 };
 
 static UINT8 GeoTIFFMiddleData[] =
 {
                    0x00,0x00,0x00,0x00, //indicates no more IFDs
                    0x00,                //tag 0x111 value
                    0x01,0x00,0x00,0x00, //tag 0x11a value (RATIONAL)
                    0x01,0x00,0x00,0x00,
                    0x01,0x00,0x00,0x00, //tag 0x11b value (RATIONAL)
                    0x01,0x00,0x00,0x00
 };

#endif
