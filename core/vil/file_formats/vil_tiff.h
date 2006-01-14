// This is core/vil/file_formats/vil_tiff.h
#ifndef vil_tiff_file_format_h_
#define vil_tiff_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
//  Modifications
//  3 Oct 2001 Peter Vanroose - Implemented get_property and set_property
//  5 Jan 2002 Ian Scott      - Converted to vil
//  9 Dec 2003 Peter Vanroose - Added support for 1-bit pixel (bitmapped) images
//  21 Dec 2005 J.L. Mundy - Substantial rewrite to handle a more 
//  complete tiff 6.0 standard. Files with tiles can now be read and
//  written. Only tiled images are considered blocked, i.e. not strips.
//  Block dimensions must be a multiple of 16, for compatibility with 
//  compression schemes. Tiff files with separate color bands are not handled
//
//  KNOWN BUG - 24bit samples for both nplanes = 1 and nplanes = 3
//
//\endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_memory_chunk.h>
#include <tiffio.h>
//: Loader for tiff files
class vil_tiff_file_format : public vil_file_format
{
 public:
  virtual char const *tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream *vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    unsigned size_block_i,
                                                    unsigned size_block_j,
                                                    enum vil_pixel_format);

  //: report if this format supports blocked image files
  virtual bool supports_blocking(){return true;}
};
struct tif_stream_structures;
class vil_tiff_header;

//: Generic image interface for TIFF files
class vil_tiff_image : public vil_image_resource
{
  friend class vil_tiff_file_format;
 public:

  vil_tiff_image(tif_stream_structures* tss,
                 vil_tiff_header* th);

  ~vil_tiff_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual enum vil_pixel_format pixel_format() const;


  virtual bool supports_blocking(){return true;}

  //: returns "tiff"
  char const *file_format() const;


  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Return true if the property given in the first argument has been set.
  // currently defined:
  //  "quantisation_depth" - number of relevant bits per pixel
  //  "size_block_i" and "size_block_j" - block dimensions
  
  bool get_property(char const *tag, void *prop = 0) const;

 private:
  tif_stream_structures* p_;
  vil_tiff_header* h_;

  //: Block size in columns
  unsigned sizeb_i() const;
  //: Block size in rows
  unsigned sizeb_j() const;
  //: Number of blocks in image width
  unsigned nbi() const;
  //: Number of blocks in image height
  unsigned nbj() const;

  //: the number of samples in a block
  unsigned samples_per_block() const;

  enum vil_pixel_format compute_pixel_format();

  void copy_byte_block(vxl_byte* data, const unsigned long nbytes,
                       vil_memory_chunk_sptr& cnk) const;
  //::convert a packed block to an expanded view
  vil_image_view_base_sptr 
    view_from_buffer(vil_pixel_format& fmt,
                     vil_memory_chunk_sptr const& buf,
                     unsigned samples_per_block,
                     unsigned bits_per_sample) const;
  
  //: the key methods for decoding the file data
  vil_image_view_base_sptr fill_block_from_tile(vil_memory_chunk_sptr const & buf) const;

  vil_image_view_base_sptr fill_block_from_strip(vil_memory_chunk_sptr const & buf, const unsigned long strip_index) const;

  vil_image_view_base_sptr  
    glue_blocks_together(const vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const;

  vil_image_view_base_sptr get_block_internal( unsigned block_index_i,
                                               unsigned block_index_j ) const;
void 
get_blocks_internal( unsigned start_block_i,
                     unsigned end_block_i,
                     unsigned start_block_j,
                     unsigned end_block_j,
                     vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks ) const;
                                               
  bool put_block(unsigned bi, unsigned bj, unsigned i0,
                 unsigned j0, const vil_image_view_base& im);

  //: Get the offset from the start of the block row for pixel position i
  bool block_i_offset(unsigned block_i, unsigned i,
                      unsigned& i_offset) const;

  //: Get the offset from the start of the block column for pixel position j
  bool block_j_offset(unsigned block_j, unsigned j,
                      unsigned& j_offset) const;
  
  unsigned block_index(unsigned block_i, unsigned block_j) const;

  bool trim_border_blocks(unsigned i0, unsigned ni,
                          unsigned j0, unsigned nj,
                          unsigned start_block_i,
                          unsigned start_block_j,
                          vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const;

  //: fill out the block with leading zeros or trailing zeros if necessary
  void pad_block_with_zeros(unsigned ioff, unsigned joff,
                            unsigned iclip, unsigned jclip,
                            unsigned bytes_per_pixel,
                            vxl_byte* block_buf);

  //: fill the block with view data
  void fill_block_from_view(unsigned bi, unsigned bj,
                            unsigned i0, unsigned j0,
                            unsigned ioff, unsigned joff,
                            unsigned iclip, unsigned jclip,
                            const vil_image_view_base& im,
                            vxl_byte* block_buf);

  void bitpack_block(unsigned bytes_per_block,
                     vxl_byte* in_block_buf,
                     vxl_byte* out_block_buf);

  bool write_block_to_file(unsigned bi, unsigned bj,
                           unsigned block_size_bytes,
                           vxl_byte* block_buf);

};

//
//
//------------------------ Lifted from vil_nitf2_image ------------------------
//            If this happens again then maybe should elevate to a 
//            utility class -- JLM
//:
// This function does a lot of work for \sa byte_align_data().  It will strip one value
// of ni bits and return it as a value of type T (with zero padding on the MSBs).
// Both io and ni are lengths (in bits - not bytes).
//
// @param i0: Offset (in bits - not bytes) from in_val[0].  This will be the start
//            of the bits extracted from in_val.
// @param ni: number of bits (starting from i0) that will be extracted from in_val.
template< class T >
T tiff_get_bits( const T* in_val, unsigned i0, unsigned ni )
{
  //JLM DEBUG
  // THE EFFECT OF SHIFT
  unsigned one = 1;
  unsigned four = 4;
  unsigned left_one = one << 1;// left_one = 2
  unsigned right_one = one >> 1;// right_one = 0
  unsigned left_four = four << 1;//left_four = 8
  unsigned right_four = four >> 1;//right_four = 2

  unsigned sample_offset = i0 / ( sizeof(T)*8 );
  unsigned bit_offset = i0 % ( sizeof(T)*8 );

  unsigned strip_left = bit_offset;
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
    for ( int i = 0 ; i < strip_right ; i++ ) temp /= 2;
    //temp = temp >> strip_right;
  } else if ( strip_right < 0 ){
    //we didn't have enough bits in the first element of the in_val array
    //need to get some from the next element

    for ( int i = 0 ; i < (-strip_right) ; i++ ) temp *= 2;
    temp += tiff_get_bits<T>( in_val+sample_offset+1, 0, -strip_right );

#if 0
    T next = in_val[sample_offset+1];
    //shift right a bigger amount
    int new_strip_right = strip_right + (sizeof(T)*8);
    for ( int i = 0 ; i < new_strip_right ; i++ ) next /= 2;
    // next is the LSB part
    unsigned new_strip_left = 1- strip_right;
    temp = temp << new_strip_left;
    temp += next;
#endif
  }
  //vcl_cout << "Out val = " << vcl_hex << temp << '\n';
  return temp;
}

//:
// This function will byte align the data in in_data and store the result in out_data.  For example, let's
// say that you had in_data is of type unsigned char and contains the following data: 110010111001111010000110.
// In other words:
// in_data[0] = 203 (11001011)
// in_data[1] = 158 (10011110)
// in_data[2] = 134 (10000110)
// Let's further say you called this function like this: byte_align_data( in_data, 8, 3, out_data ).
// Then, when the function finished, out_data would look like this:
// out_data[0] = 6 (00000110)
// out_data[1] = 2 (00000010)
// out_data[2] = 7 (00000111)
// out_data[3] = 1 (00000001)
// out_data[4] = 7 (00000111)
// out_data[5] = 2 (00000010)
// out_data[6] = 0 (00000000)
// out_data[7] = 6 (00000110)
//
// Basically, what the function did was group the bitstream into groups of three and then store all of the
// values into out_data.  It had to zero pad all the values (on the MSB side) to get them into out_data.  That's
// why out_data is bigger.
//
// This function works with other unsigned types of data too.  For example, let's say in_data was of type unsigned int
// and contained the following bits: 0100110010111000 0111101100000000 1111000011110000 (note that this bitstream is shown
// in big endian notation, that will not be the case if you are on a little endian machine -- this is just for illustration)
// in other words:
// in_data[0] = 19640 (0100110010111000) [shown in big endian for illustrative purposes only]
// in_data[1] = 31488 (0111101100000000) [shown in big endian for illustrative purposes only]
// in_data[2] = 61680 (1111000011110000) [shown in big endian for illustrative purposes only]
// Let's further say, you called this function like this byte_align_data( in_data, 4, 12, out_data ).
// Then out_data would be aligned along two byte (sixteen bit) boundaries and would look like this:
// out_data[0] = 1227 (0000010011001011) [shown in big endian for illustrative purposes only]
// out_data[1] = 2171 (0000100001111011) [shown in big endian for illustrative purposes only]
// out_data[2] = 15   (0000000000001111) [shown in big endian for illustrative purposes only]
// out_data[3] = 240  (0000000011110000) [shown in big endian for illustrative purposes only]
//
// Because of the fact that this function uses bit shifting operators, and the behavior of the vcl_right
// shift operator is implementation specific when applied to a negative number, you should probably
// only use this function on unsigned data.
//
// @param in_data: The input data.  It must be at least (num_samples*in_bits_per_sample\8) bytes long.
//                The values should have the endianness of your platform.
// @param num_samples: The number of actual samples in in_data.
// @param in_bits_per_sample: The bits per sample in in_data
// @param out_data: I'll store the output data here.  It must be at least (num_samples*sizeof(T)) bytes long
//                 The values will have the endianness of your platform.
//
// Note that inBitsPerSampe must not be >= sizeof(T).  If they were to be equal, then this function
// would have nothing to do (in_data is already byte aligned).  If in_bits_per_sample were less than sizeof(T),
// then each element of out_data would be too small to store the corresponding elements in in_data.
//
// Note that there is a specialization for the bool case which just casts it to an 8 bit quantity then calls
// this same function.  This is because the logic in get_bits<> doesn't work for the bool case.
template< class T >
T* tiff_byte_align_data( T* in_data, unsigned num_samples, unsigned in_bits_per_sample, T* out_data )
{
  unsigned out_bytes_per_sample = sizeof(T);
  unsigned out_bits_per_sample = out_bytes_per_sample * 8;
  assert( in_bits_per_sample < out_bits_per_sample );

  //grab each value from the bitstream (in_data) that we need... one
  //at a time
  unsigned bit_offset = 0;
  for ( unsigned o = 0 ; o < num_samples ; o++ ){
    out_data[o] = tiff_get_bits<T>( in_data, bit_offset, in_bits_per_sample );
    //printf("bo = %i, in = %x, out =  %x\n", bit_offset, in_data[o], out_data[o]);
    bit_offset+=in_bits_per_sample;
  }

  return out_data;
}

template<> bool* tiff_byte_align_data<bool>( bool* in_data, unsigned num_samples, unsigned in_bits_per_sample, bool* out_data );

#endif // vil_tiff_file_format_h_
