// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_DATA_MASK_TABLE_H
#define VIL_NITF2_DATA_MASK_TABLE_H

#include <vcl_vector.h>
#include <vxl_config.h>
#include <vcl_string.h>
#include <vil/vil_pixel_format.h>

class vil_stream;

/**
  * This class is responsible for parsing a NITF 2.1 data mask table.
  * when present, a vil_nitf2_image_subheader() will use this class
  * to extract the table from the file.  It reads a vil_stream, via
  * the parse() method and will return false if it failed.
  */
class vil_nitf2_data_mask_table
{
public:
  vil_nitf2_data_mask_table( unsigned int numBlocksX, unsigned int numBlocksY, 
                              unsigned int numBands, const vcl_string imode );
  bool parse( vil_stream* stream );

  /**
    * If these functions return true, then you may call \sa block_band_offset()
    * and \sa pad_pixel()
    */
  bool has_offset_table() const { return BMR_n_BND_m.size() > 0; }
  bool has_pad_pixel_table() const { return TMR_n_BND_m.size() > 0; }

  vxl_uint_32 blocked_image_data_offset() const;
  /**
    * Returns true iff this block is present in the data.  False otherwise.
    * If this returns false, then there is not sense calling block_band_offset
    * for this band/block combination. It will return 0xFFFFFFFF.
    * If this returns false, then this entire block/band is considered to be blank
    */
  vxl_uint_32 block_band_present( unsigned int block_x, unsigned int block_y, int band = -1) const;
  /**
    * if imode == "S", then the band argument is used and I will return the offset to 'band'
    * if imode != "S", then the band argument is ignored, and I will return the offset to the 
    * beginning of the requested block
    */
  vxl_uint_32 block_band_offset( unsigned int block_x, unsigned int block_y, int band = -1) const;
  ///band argument is ignored if imode != "S"... ie. all bands have the same pad pixel in that case
  vxl_uint_32 pad_pixel( unsigned int block_x, unsigned int block_y, int band ) const;
  vxl_uint_32 block_band_has_pad( unsigned int block_x, unsigned int block_y, int band = -1) const;

  static void maybe_endian_swap( char* a, unsigned sizeOfAInBytes, vil_pixel_format pixFormat );
  static void maybe_endian_swap( char* a, unsigned sizeOfAInBytes, unsigned int bytesPerSample );
protected:
  //Blocked Image Data Offset (in bytes)
  vxl_uint_32 IMDATOFF;
  //Block Mask Record Length (in bytes)
  vxl_uint_16 BMRLNTH;
  //Pad Pixel Mask Record Length (in bytes)
  vxl_uint_16 TMRLNTH;
  //Pad Output Pixel Code Lenth (in bits)
  vxl_uint_16 TPXCDLNTH;
  //Pad Output Pixel Code (it's an integer,
  //but it's length is TPXCDLNTH rounded up
  //to the nearest byte)
#if VXL_HAS_INT_64
  vxl_uint_64 TPXCD;
#else
  vxl_uint_32 TPXCD;
#endif //VXL_HAS_INT_64

  //Block n, Band m Offset
  //indexed BMR_n_BND_m[row][col][band] for i_mode = "S" else
  //indexed BMR_n_BND_m[row][col]
  vcl_vector< vcl_vector< vcl_vector< vxl_uint_32 > > > BMR_n_BND_m;
  //Pad Pixel n, Band m
  //indexed TMR_n_BND_m[row][col][band] for i_mode = "S" else
  //indexed TMR_n_BND_m[row][col]
  vcl_vector< vcl_vector< vcl_vector< vxl_uint_32 > > > TMR_n_BND_m;

  unsigned int num_blocks_x;
  unsigned int num_blocks_y;
  unsigned int num_bands;
  vcl_string i_mode;
};

#endif // VIL_NITF2_DATA_MASK_TABLE_H
