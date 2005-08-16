// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_H
#define VIL_NITF2_H

#include <vil/vil_image_resource.h>
#include <vcl_vector.h>

#include <vil/vil_stream.h>

#include "vil_nitf2_image_subheader.h"
#include "vil_nitf2_header.h"

/** 
  * Class for reading NITF 2.1 imagery files.  It works just like any other
  * vil_image_resource class except that it does support retrieving multiple
  * images from the same file.  Call nimages() to find out how many images 
  * are in this resource.  and then call set_current_image() to tell the class
  * which image you want to work with presently.  All regular vil_image_resource
  * functions will operate on the current image.
  *
  * you can access image header information through get_image_headers() and file
  * header information through get_header()
  *
  * Know reading limitations:
  *  - The only supported compression schemes are uncompressed (all 4 different
  *    types of data layouts: band sequential, band interleaved, band interleaved
  *    by row, and band interleaved by pixel) and JPEG 2000 compression (via plugin).
  *  Other forms of compression do not work (eg. JPEG (regular, non-j2k)
  *  - Writing is unsupported at this time
  *  - graphic segments are not read in
  *  - text segments are not read in
  *  - currently all TREs are read in as character arrays (ie. they are not parsed
  *    and validated).  This should be fixed soon. 
  *  - TREs that are located in the overflow section of the file are not read in at all.
  *  - The class was designed to handle images with PVTYPE=C (complex), but images of
  *    this type are completely untested.
  *  - The class does not have built-in support for JPEG2000 compressed NITF files
  *    If you set s_decode_jpeg_2000 to a function that can decompress a JPEG2000
  *    code stream, then this class will recognize it and use it whenever appropriate
  * 
  * Things that do work:
  *  - Reading uncompressed and image data (ints, shorts, 8 bit int, floats etc)
  *  - parsing and validating file headers, and image headers
  *  - Data masks (and pad pixels) are supported through vil_nitf2_data_mask_table
  *  - PJUST = L
  *  - Images with LUTS will read in correctly, but you will need to apply the LUT
  *    yourself.  You can access the lut information for p-th band of the current image by calling 
  *    current_image_header()->get_lut_info(p, out_n_luts, out_ne_lut, out_lut_data)  
  */
class vil_nitf2_image : public vil_image_resource
{
public:
  /**
    * Instantiate an image resource, but doesn't read anything.  You'll want
    * to call parse_headers() before you do anything with me (eg. before you 
    * ask for any image data).  If that returns false, then I am invalid and
    * useless to you in every way.
    */
  vil_nitf2_image( vil_stream* is );
  vil_nitf2_image( const vcl_string& filePath, const char* mode );

  virtual ~vil_nitf2_image();

  //:return the image info the the current image
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;
  virtual enum vil_pixel_format pixel_format () const;

  //: returns "nitf vM.N"
  //char const * file_format() const;

  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;
  virtual vil_image_view_base_sptr get_copy_view( ) const
  { return get_copy_view( 0, ni(), 0, nj() ); }
  
  virtual bool put_view (const vil_image_view_base& /* im */, unsigned /* i0 */, unsigned /* j0 */ )
  { return false; }

  virtual bool get_property (char const *tag, void *property_value=0) const;

  //const vil_nitf2_header& getFileHeader() const;
  const vcl_vector< vil_nitf2_image_subheader* >& get_image_headers() const
  { return m_image_headers; }
  const vil_nitf2_header& get_header() const
  { return m_file_header; }
  /**
    * Since the VIL API (eg. get_view()) for retrieving image data
    * doesn't support files with multiple images, clients will
    * need to call this function to tell get_view() which image to read in.
    * If you don't call this function, then the first image (index = 0) will be read in.
    * Note that this also affects the get methods that return image characteristics (eg ni(), nj(), nplanes())
    */
  void set_current_image( unsigned int index );
  unsigned int nimages() const;

  bool parse_headers();
  vil_nitf2_classification::file_version file_version() const;

  /**
    * All instances of vil_nitf2_image will use s_decode_jpeg_2000() to decode JPEG 2000
    * streams if you set the function.  If unset, then the library will not be able
    * to read JPEG 2000 compressed NITF files.
    */
  static vil_image_view_base_sptr ( *s_decode_jpeg_2000 )( vil_stream* vs,
                                                           unsigned i0, unsigned ni,
                                                           unsigned j0, unsigned nj );
protected:
  virtual vil_image_view_base_sptr get_copy_view_uncompressed(unsigned i0, unsigned ni,
                                                              unsigned j0, unsigned nj) const;
  ///helper function for get_copy_view_uncompressed
  vil_image_view_base_sptr get_block( unsigned int blockIndexX, unsigned int blockIndexY ) const;
  void get_blocks( unsigned int startBlockX, unsigned int endBlockX, 
                   unsigned int startBlockY, unsigned int endBlockY,
                   vcl_vector< vcl_vector< vil_image_view_base_sptr > >& outDataBlocks ) const;
  /**
    * Glue together all the blocks in dataBlocks into one image and return it.
    * Optionally, specify if you don't want to use all of the pixels in the border
    * blocks.
    * 
    * helper function for get_copy_view_uncompressed
    */
  vil_image_view_base_sptr glue_blocks_together( 
    const vcl_vector< vcl_vector< vil_image_view_base_sptr > >& dataBlocks ) const;
  ///helper function for get_copy_view_uncompressed
  bool trim_border_blocks( 
      vcl_vector< vcl_vector< vil_image_view_base_sptr > >& dataBlocks, 
      unsigned int start_block_x_offset, unsigned int start_block_y_offset, 
      unsigned int end_block_x_offset, unsigned int end_block_y_offset ) const;


  /**
    * Returns the overall offset to the specified image/block/band combination.
    * If this block isn't present in the stream (ie. it's all blank), then
    * I'll return 0;
    */
  vil_streampos get_offset_to_image_data_block_band( unsigned int imageIndex, 
    unsigned int blockIndexX,unsigned int blockIndexY, int bandIndex ) const;
  vil_streampos get_offset_to_image_data( unsigned int imageIndex ) const;
  vil_streampos get_offset_to_image_subheader( unsigned int imageIndex ) const;

  unsigned int get_pixels_per_block_x() const;
  unsigned int get_pixels_per_block_y() const;
  unsigned int get_num_blocks_x() const; 
  unsigned int get_num_blocks_y() const; 

  vil_nitf2_header m_file_header;
  void clear_image_headers();
  vcl_vector< vil_nitf2_image_subheader* > m_image_headers;
  const vil_nitf2_image_subheader* current_image_header() const;
  vil_stream* m_stream;
  unsigned int m_current_image_index;
};

/**
  * This function does a lot of work for \sa byte_align_data().  It will strip one value
  * of ni bits and return it as a value of type T (with zero padding on the MSBs).  
  * Both io and ni are lengths (in bits - not bytes).
  *
  * @param i0: Offset (in bits - not bytes) from in_val[0].  This will be the start 
  *            of the bits extracted from in_val.  
  * @param ni: number of bits (starting from i0) that will be extracted from in_val.
  */
template< class T >
T get_bits( const T* in_val, unsigned int i0, unsigned int ni )
{
  unsigned int sample_offset = i0 / ( sizeof(T)*8 );
  unsigned int bit_offset = i0 % ( sizeof(T)*8 );

  unsigned int strip_left = bit_offset;
  int strip_right = ( sizeof( T ) * 8 ) - ( bit_offset + ni );
  T temp = in_val[sample_offset];
  if ( strip_left > 0 ){
    //strip off the appropriate bits from the vcl_left (replacing them with zeros)
    temp = temp << strip_left; 
    temp = temp >> strip_left;
  }
  if ( strip_right > 0 ){
    //strip off the appropriate bits from the vcl_right
    //the bit shift operator wasn't having the correct effect, so that'w
    //why the for loop
    for( int i = 0 ; i < strip_right ; i++ ) temp /= 2;
    //temp = temp >> strip_right;
  } else if ( strip_right < 0 ){
    //we didn't have enough bits in the first element of the in_val array
    //need to get some from the next element
    for( int i = 0 ; i < abs(strip_right) ; i++ ) temp *= 2;
    temp += get_bits<T>( in_val+sample_offset+1, 0, abs( strip_right ) );
  }
  return temp;
}

/**
  * This function will byte align the data in in_data and store the result in out_data.  For example, lets
  * say that you had in_data is of type unsigned char and contains the following data: 110010111001111010000110.  
  * In other words:
  * in_data[0] = 203 (11001011)
  * in_data[1] = 158 (10011110)
  * in_data[2] = 134 (10000110)
  * Let's further say you called this function like this: byte_align_data( in_data, 8, 3, out_data ).
  * Then, when the function finished, out_data would look like this:
  * out_data[0] = 6 (00000110)
  * out_data[1] = 2 (00000010)
  * out_data[2] = 7 (00000111)
  * out_data[3] = 1 (00000001)
  * out_data[4] = 7 (00000111)
  * out_data[5] = 2 (00000010)
  * out_data[6] = 0 (00000000)
  * out_data[7] = 6 (00000110)
  *
  * Basically, what the function did was group the bitstream into groups of three and then store all of the
  * values into out_data.  It had to zero pad all the values (on the MSB side) to get them into out_data.  That's
  * why out_data is bigger.
  *
  * This function works with other unsigned types of data too.  For example, let's say in_data was of type unsigned int
  * and contained the following bits: 0100110010111000 0111101100000000 1111000011110000 (note that this bitstream is shown
  * in big endian notation, that will not be the case if you are on a little endian machine -- this is just for illustration)
  * in other words:
  * in_data[0] = 19640 (0100110010111000) [shown in big endian for illustrative purposes only]
  * in_data[1] = 31488 (0111101100000000) [shown in big endian for illustrative purposes only]
  * in_data[2] = 61680 (1111000011110000) [shown in big endian for illustrative purposes only]
  * Let's further say, you called this function like this byte_align_data( in_data, 4, 12, out_data ).
  * Then out_data would be aligned along two byte (sixteen bit) boundaries and would look like this:
  * out_data[0] = 1227 (0000010011001011) [shown in big endian for illustrative purposes only]
  * out_data[1] = 2171 (0000100001111011) [shown in big endian for illustrative purposes only]
  * out_data[2] = 15   (0000000000001111) [shown in big endian for illustrative purposes only]
  * out_data[3] = 240  (0000000011110000) [shown in big endian for illustrative purposes only]
  *
  * Because of the fact that this function uses bit shifting operators, and the behavior of the vcl_right
  * shift operator is implementation specific when applied to a negative number, you should probably
  * only use this function on unsigned data.  
  *
  * @param in_data: The input data.  It must be at least (num_samples*in_bits_per_sample\8) bytes long.
  *                The values should have the endianness of your platform.
  * @param num_samples: The number of actual samples in in_data.  
  * @param in_bits_per_sample: The bits per sample in in_data
  * @param out_data: I'll store the output data here.  It must be at least (num_samples*sizeof(T)) bytes long
  *                 The values will have the endianness of your platform.
  *
  * Note that inBitsPerSampe must not be >= sizeof(T).  If they were to be equal, then this function
  * would have nothing to do (in_data is already byte aligned).  If in_bits_per_sample were less than sizeof(T),
  * then each element of out_data would be too small to store the corresponding elements in in_data.
  *
  * Note that there is a specialization for the bool case which just casts it to an 8 bit quantity then calls 
  * this same function.  This is because the logic in get_bits<> doesn't work for the bool case.  
  */
template< class T >
T* byte_align_data( T* in_data, unsigned int num_samples, unsigned int in_bits_per_sample, T* out_data )
{
  unsigned int out_bytes_per_sample = sizeof(T);
  unsigned int out_bits_per_sample = out_bytes_per_sample * 8;
  assert( in_bits_per_sample < out_bits_per_sample );

  //grab each value from the bitstream (in_data) that we need... one
  //at a time
  unsigned int bit_offset = 0;
  for( unsigned int o = 0 ; o < num_samples ; o++ ){
    out_data[o] = get_bits<T>( in_data, bit_offset, in_bits_per_sample );
    bit_offset+=in_bits_per_sample;
  }

  return out_data;
}
template<> bool* byte_align_data<bool>( bool* in_data, unsigned int num_samples, unsigned int in_bits_per_sample, bool* out_data );

#endif // VIL_NITF2_H
