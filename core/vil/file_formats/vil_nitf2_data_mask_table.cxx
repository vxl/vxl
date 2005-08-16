// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_data_mask_table.h"

#include <vil/vil_stream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>

vil_nitf2_data_mask_table::vil_nitf2_data_mask_table( 
  unsigned int num_blocks_x, unsigned int num_blocks_y, 
  unsigned int num_bands, const vcl_string i_mode )
  : num_blocks_x( num_blocks_x ),
    num_blocks_y( num_blocks_y ),
    num_bands( num_bands ),
    i_mode( i_mode )
{ }

bool vil_nitf2_data_mask_table::parse( vil_stream* stream )
{
  //get the vcl_fixed width stuff first
  if ( stream->read( (void*)(&IMDATOFF),  4 ) != 4 ||
      stream->read( (void*)(&BMRLNTH),   2 ) != 2 || 
      stream->read( (void*)(&TMRLNTH),   2 ) != 2 || 
      stream->read( (void*)(&TPXCDLNTH), 2 ) != 2 )
  {
    return false;
  } else {
    maybe_endian_swap( (char*)&IMDATOFF, 4, 4 );
    maybe_endian_swap( (char*)&BMRLNTH, 2, 2 );
    maybe_endian_swap( (char*)&TMRLNTH, 2, 2 );
    maybe_endian_swap( (char*)&TPXCDLNTH, 2, 2 );
  }

  //reading in TPXCD is quite an ordeal because
  //the width of the field varies based on the value of 
  //TPXCDLNTH
  unsigned short width = TPXCDLNTH / 8;
  if ( TPXCDLNTH % 8 != 0 ) width++;
  void* val = malloc( width );
//  if ( stream->read( &val, width ) != width ) return false;
  if ( stream->read( val, width ) != width ) return false;
  maybe_endian_swap( (char*)val, width, width );
  if ( width == 1 ){
    TPXCD = *((vxl_byte*)val);
  } else if ( width == 2 ){
    TPXCD = *((vxl_uint_16*)val);
  } else if ( width == 4 ){
    TPXCD = *((vxl_uint_32*)val);
  }
#if VXL_HAS_INT_64
  else if ( width == 8 ){
    TPXCD = *((vxl_uint_64*)val);
  }
#endif //VXL_HAS_INT64

  //properly size and then read in our vectors
  ////do BMR_n_BND_m first
  unsigned int i, j, b;
  if ( BMRLNTH != 0 ){
    BMR_n_BND_m.resize( num_blocks_x );
    for( i = 0 ; i < num_blocks_y ; i++ ){
      BMR_n_BND_m[i].resize( num_blocks_y );
      for( j = 0 ; j < num_blocks_x ; j++ ){
        BMR_n_BND_m[i][j].resize( i_mode == "S" ? num_bands : 1 );
      }
    }
    for( i = 0 ; i < num_blocks_x ; i++ ){
      for( j = 0 ; j < num_blocks_y ; j++ ){
        for( b = 0 ; b < BMR_n_BND_m[i][j].size() ; b++ ){
          if ( stream->read( (void*)(&BMR_n_BND_m[i][j][b]), 4 ) != 4 ) return false;
          maybe_endian_swap( (char*)(&BMR_n_BND_m[i][j][b]), 4, 4 );
        }
      }
    }
  }


  //now do the pad pixel vcl_vector
  if ( TMRLNTH != 0 ){
    TMR_n_BND_m.resize( num_blocks_x );
    for( i = 0 ; i < num_blocks_y ; i++ ){
      TMR_n_BND_m[i].resize( num_blocks_y );
      for( j = 0 ; j < num_blocks_x ; j++ ){
        TMR_n_BND_m[i][j].resize( i_mode == "S" ? num_bands : 1 );
      }
    }
    for( i = 0 ; i < num_blocks_x ; i++ ){
      for( j = 0 ; j < num_blocks_y ; j++ ){
        for( b = 0 ; b < TMR_n_BND_m[i][j].size() ; b++ ){
          if ( stream->read( (void*)(&TMR_n_BND_m[i][j][b]), 4 ) != 4 ) return false;
          maybe_endian_swap( (char*)(&TMR_n_BND_m[i][j][b]), 4, 4 );
        }
      }
    }
  }

  return true;
}

vxl_uint_32 vil_nitf2_data_mask_table::blocked_image_data_offset() const
{
  return IMDATOFF;
}

vxl_uint_32 vil_nitf2_data_mask_table::block_band_offset( unsigned int block_x, 
                                                           unsigned int block_y, 
                                                           int band ) const
{
  int band_to_use = i_mode == "S" ? band : 0;
  assert( block_x < BMR_n_BND_m.size() &&
          block_y < BMR_n_BND_m[block_x].size() &&
          band_to_use < static_cast<int>(BMR_n_BND_m[block_x][band_to_use].size()) );
  assert( ( band < 0 && i_mode != "S" ) ||
          ( band >= 0 && i_mode == "S" ) );
  return BMR_n_BND_m[block_x][block_y][band_to_use];
}

vxl_uint_32 vil_nitf2_data_mask_table::pad_pixel( unsigned int block_x, 
                                                   unsigned int block_y, 
                                                   int band ) const
{
  int band_to_use = i_mode == "S" ? band : 0;
  assert( block_x < TMR_n_BND_m.size() &&
          block_y < TMR_n_BND_m[block_x].size() &&
          band_to_use < static_cast<int>(TMR_n_BND_m[block_x][band_to_use].size()) );
  assert( ( band < 0 && i_mode != "S" ) ||
          ( band >= 0 && i_mode == "S" ) );
  return TMR_n_BND_m[block_x][block_y][band_to_use];
}

#if VXL_LITTLE_ENDIAN
void swap16(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    vcl_swap( a[i+0], a[i+1] );
  }
}
void swap32(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    vcl_swap( a[i+0], a[i+3] );
    vcl_swap( a[i+1], a[i+2] );
  }
}
void swap64(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 8; i += 8)
  {
    vcl_swap( a[i+0], a[i+7] );
    vcl_swap( a[i+1], a[i+6] );
    vcl_swap( a[i+2], a[i+5] );
    vcl_swap( a[i+3], a[i+4] );
  }
}
#endif

void vil_nitf2_data_mask_table::maybe_endian_swap( char* a, unsigned size_of_a_in_bytes, 
                                                    vil_pixel_format pix_format )
{
#if VXL_LITTLE_ENDIAN
  maybe_endian_swap( a, size_of_a_in_bytes, vil_pixel_format_sizeof_components( pix_format ) );
#else
  (void)a;
  (void)size_of_a_in_bytes;  // silence unused parameter compiler warning
  (void)pix_format;
#endif //VXL_LITTLE_ENDIAN
}

void vil_nitf2_data_mask_table::maybe_endian_swap( char* a, unsigned size_of_a_in_bytes, 
                                                    unsigned int bytesPerSample )
{
#if VXL_LITTLE_ENDIAN
  switch( bytesPerSample ){
  case 8: swap64( a, size_of_a_in_bytes / 8 ); break; //64 bit
  case 4: swap32( a, size_of_a_in_bytes / 4 ); break; //32 bit
  case 2: swap16( a, size_of_a_in_bytes / 2 ); break; //16 bit
  }
#else
  (void)a;
  (void)size_of_a_in_bytes;  // silence unused parameter compiler warning
  (void)bytesPerSample;
#endif //VXL_LITTLE_ENDIAN
}

vxl_uint_32 vil_nitf2_data_mask_table::block_band_present( unsigned int block_x, unsigned int block_y, int band ) const
{
  int band_to_use = i_mode == "S" ? band : 0;
  assert( block_x < BMR_n_BND_m.size() &&
          block_y < BMR_n_BND_m[block_x].size() &&
          band_to_use < static_cast<int>(BMR_n_BND_m[block_x][band_to_use].size()) );
  return block_band_offset( block_x, block_y, band ) != 0xFFFFFFFF;
}

vxl_uint_32 vil_nitf2_data_mask_table::block_band_has_pad( unsigned int block_x, unsigned int block_y, int band ) const
{
  int band_to_use = i_mode == "S" ? band : 0;
  assert( block_x < TMR_n_BND_m.size() &&
          block_y < TMR_n_BND_m[block_x].size() &&
          band_to_use < static_cast<int>(TMR_n_BND_m[block_x][band_to_use].size()) );
  return pad_pixel( block_x, block_y, band ) != 0xFFFFFFFF;
}