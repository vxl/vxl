// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_IMAGE_SUBHEADER_H
#define VIL_NITF2_IMAGE_SUBHEADER_H

#include <vcl_vector.h>

#include "vil_nitf2_field_sequence.h"
#include "vil_nitf2_classification.h"

class vil_stream;
class vil_nitf2_data_mask_table;
/**
  * This class is responsible for parsing a NITF 2.1 image header.
  * It is used by vil_nitf_image for this purpose.  You can access
  * any of the information in the header via the appropriate
  * get_property() function.  There are also some shortcut
  * functions for getting commonly needed data like nplanes()
  * etc.
  */
class vil_nitf2_image_subheader 
{
public:
  vil_nitf2_image_subheader( vil_nitf2_classification::file_version version );
  virtual ~vil_nitf2_image_subheader();
  
  ///read the image header starting at stream's current position
  ///returns false if failed
  virtual bool read( vil_stream* stream );
  //virtual bool write( vil_stream* stream );

  // Sets out_value to the value of field specified by tag. 
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, T& out_value) const
  {
    return m_field_sequence.get_value( tag, out_value );
  }

  // Sets out_value to the value of vcl_vector field element specified by tag and index.
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, int i, T& out_value) const
  {
    return m_field_sequence.get_value( tag, i, out_value );
  }

  unsigned int nplanes() const;
  unsigned int get_pixels_per_block_x() const;
  unsigned int get_pixels_per_block_y() const;
  unsigned int get_num_blocks_x() const; 
  unsigned int get_num_blocks_y() const; 
  const bool has_data_mask_table() const { return data_mask_table() != 0; }
  const vil_nitf2_data_mask_table* data_mask_table() const { return m_data_mask_table; }

  /**
    * Returns true if the ith image band in this image subheader has LUT info
    * and false otherwise.  If it returns true, then all the out parameters
    * will be populated from the header data.
    * @param n_luts: number of luts for this image band (NITF spec says 
    *                it's in the range [1-4] (if luts present)
    * @param ne_lut: number of elements in each lut
    * @param lut_d: the LUT data indexed first by lut, then lut element
    *               ie, the last element would be lut_d[n_luts-1][ne_lut-1]
    *
    * See the NITF spec for exactly how this data is to be interpretted.  Here's
    * a summary:
    * LUTS always contain 8 bit unsigned data.  If n_luts is 1, then this band
    * is to be transformed into an 8-bit image plane.  Each pixel value in the 
    * original image can be used as an index into lut_d.  (ie. lut_d[0][pixelVal])
    * If n_luts is 2, then the output image plate will be 16 bits.  The MSBs of
    * each of the output pixels will be obtained from lut_d[0] and the LSBs will
    * be obtained from lut_d[1].  
    * If n_luts is 3, then this plane will be transformed into 3 planes: R, G and B.
    * lut_d[0] shall vcl_map to Red, lut_d[1] shall vcl_map to Green, and lut_d[2] shall vcl_map 
    * to Blue.  
    */
  bool get_lut_info( unsigned int i, int& n_luts, int& ne_lut, 
                     vcl_vector< vcl_vector< unsigned char > >& lut_d ) const;
protected:

  vil_nitf2_field_sequence m_field_sequence;
  vil_nitf2_data_mask_table* m_data_mask_table;

  vil_nitf2_classification::file_version m_version;

  static const vil_nitf2_field_definitions* get_field_definitions_21();
  static const vil_nitf2_field_definitions* get_field_definitions_20();

private:
  static void add_shared_field_defs_1( vil_nitf2_field_definitions* defs );
  static void add_shared_field_defs_2( vil_nitf2_field_definitions* defs );
  static void add_geo_field_defs( vil_nitf2_field_definitions* defs, const vil_nitf2_classification::file_version& version );
  static void add_shared_field_defs_3( vil_nitf2_field_definitions* defs );

  static vil_nitf2_field_definitions* s_field_definitions_21;
  static vil_nitf2_field_definitions* s_field_definitions_20;
};


#endif // VIL_NITF2_IMAGE_SUBHEADER_H

