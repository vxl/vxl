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
#include <vcl_cassert.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/file_formats/vil_tiff_header.h>
#include <tiffio.h>


//: Loader for tiff files
class vil_tiff_file_format : public vil_file_format
{
 public:
  virtual char const *tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream *vs);

  virtual vil_pyramid_image_resource_sptr 
    make_input_pyramid_image(char const* file);

  //: Construct a pyramid image resource from a base image. All levels
  //  are stored in the same resource file. Each level has the same 
  //  scale ratio (0.5) to the preceeding level. Level 0 is the original 
  //  base image. The resource is returned open for reading.
  //  The temporary directory is for storing intermediate image
  //  resources during the construction of the pyramid. Files are
  //  be removed from the directory after completion.  If temp_dir is 0
  //  then the intermediate resources are created in memory.
virtual vil_pyramid_image_resource_sptr 
  make_pyramid_image_from_base(char const* filename,
                               vil_image_resource_sptr const& base_image,
                               unsigned nlevels,
                               char const* temp_dir);

  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format);

  virtual vil_blocked_image_resource_sptr 
    make_blocked_output_image(vil_stream* vs,
                              unsigned ni,
                              unsigned nj,
                              unsigned nplanes,
                              unsigned size_block_i,
                              unsigned size_block_j,
                              enum vil_pixel_format);


  virtual vil_pyramid_image_resource_sptr
    make_pyramid_output_image(char const* file);

};

struct tif_stream_structures;
class vil_tiff_header;

//: Generic image interface for single image TIFF files
class vil_tiff_image : public vil_blocked_image_resource
{
  friend class vil_tiff_file_format;
 public:

  vil_tiff_image(TIFF* tif,
                 vil_tiff_header* th);

  ~vil_tiff_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual enum vil_pixel_format pixel_format() const;

  //: returns "tiff"
  char const *file_format() const;

  //Block interface

  //: Block size in columns
  virtual unsigned size_block_i() const;

  //: Block size in rows
  virtual unsigned size_block_j() const;

  //: Number of blocks in image width
  virtual unsigned n_block_i() const;

  //: Number of blocks in image height
  virtual unsigned n_block_j() const;

  virtual vil_image_view_base_sptr get_block( unsigned  block_index_i,
                                              unsigned  block_index_j ) const;

  virtual bool put_block( unsigned  block_index_i, unsigned  block_index_j,
                          const vil_image_view_base& blk );



  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  //: Return true if the property given in the first argument has been set.
  // currently defined:
  //  "quantisation_depth" - number of relevant bits per pixel
  //  "size_block_i" and "size_block_j" - block dimensions

  virtual bool get_property(char const *tag, void *prop = 0) const;

  friend class vil_tiff_pyramid_resource;

 private:
  //: the TIFF handle to the open resource file
  TIFF* t_;

  //: the TIFF header information
  vil_tiff_header* h_;

  //to keep the tiff file open during reuse of multiple tiff resources
  //in a single file otherwise the resource destructor would close the file
  void clear_TIFF(){t_ = 0;}

  //: the number of samples in a block
  unsigned samples_per_block() const;

  enum vil_pixel_format compute_pixel_format();

  void copy_byte_block(vxl_byte* data, const unsigned long nbytes,
                       vil_memory_chunk_sptr& cnk) const;

  //: convert a packed block to an expanded view
  vil_image_view_base_sptr
    view_from_buffer(vil_pixel_format& fmt,
                     vil_memory_chunk_sptr const& buf,
                     unsigned samples_per_block,
                     unsigned bits_per_sample) const;

  //: the key methods for decoding the file data
  vil_image_view_base_sptr fill_block_from_tile(vil_memory_chunk_sptr const & buf) const;

  vil_image_view_base_sptr fill_block_from_strip(vil_memory_chunk_sptr const & buf) const;

#if 0
  vil_image_view_base_sptr get_block_internal( unsigned block_index_i,
                                               unsigned block_index_j ) const;
  void
    get_blocks_internal( unsigned start_block_i,
                         unsigned end_block_i,
                         unsigned start_block_j,
                         unsigned end_block_j,
                         vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks ) const;
#endif
  bool put_block(unsigned bi, unsigned bj, unsigned i0,
                 unsigned j0, const vil_image_view_base& im);

  unsigned block_index(unsigned block_i, unsigned block_j) const;

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
                            vxl_byte*& block_buf);

  void bitpack_block(unsigned bytes_per_block,
                     vxl_byte* in_block_buf,
                     vxl_byte* out_block_buf);

  bool write_block_to_file(unsigned bi, unsigned bj,
                           unsigned block_size_bytes,
                           vxl_byte* block_buf);
};//End of single image TIFF resource
//
//
///--------- Representation of Pyramid Images by multi-image TIFF -------
//
//
// It is assumed that image scales are not necessarily ordered with
// respect to tiff header index. This data structure maintains essential
// information about each pyramid level. The struct can be sorted on scale
// to form an ordered pyramid.
struct tiff_pyramid_level
{
  public:
tiff_pyramid_level(unsigned header_index, unsigned ni, 
                   unsigned nj, unsigned nplanes, vil_pixel_format fmt):
  header_index_(header_index), scale_(1.0f), ni_(ni), nj_(nj), 
       nplanes_(nplanes), pix_fmt_(fmt), cur_level_(0)
  {}
  ~tiff_pyramid_level(){}

  //:the tiff header index
  unsigned header_index_;

  //:scale associated with level
  float scale_;

  //:the image width 
  unsigned ni_;

  //: the image length
  unsigned nj_;

  //:the nubmer of planes
  unsigned nplanes_;

  //: the pixel format
  vil_pixel_format pix_fmt_;

  //: temporary variable for current level
  unsigned cur_level_;

  void print(const unsigned l){vcl_cout << "level[" << l <<  "] hindex "
                                        << header_index_ << " scale: " 
                                        << scale_ 
                                        << "  width: " << ni_ << "\n";}
};

//:Pyramid resource built on the multi-image capability of the TIFF format
//If read is true then the resource is open for reading else it is 
//open for writing
class vil_tiff_pyramid_resource : public vil_pyramid_image_resource
{
 public:
  vil_tiff_pyramid_resource(TIFF* t, bool read = true); 

  virtual ~vil_tiff_pyramid_resource();

  //: The following methods refer to the base (max resolution) image 
  //: Dimensions:  Planes x ni x nj.
  // This concept is treated as a synonym to components.
  inline virtual unsigned nplanes() const
    {if(levels_[0]) return levels_[0]->nplanes_; return 1;}

  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each row.
  inline virtual unsigned ni() const
    {if(levels_[0]) return levels_[0]->ni_; return 0;}

  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each column.
  inline virtual unsigned nj() const
    {if(levels_[0]) return levels_[0]->nj_; return 0;}

  //: Pixel Format.
  inline virtual enum vil_pixel_format pixel_format() const
    {if(levels_[0]) return levels_[0]->pix_fmt_; return VIL_PIXEL_FORMAT_UNKNOWN;}
  
  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const
    {return "ptif";}

  //:Methods particular to pyramid resource

  //: number of pyramid levels
  virtual unsigned nlevels() const
    {return levels_.size();}

  //:Get a partial view from the image from a specified pyramid level
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 unsigned level) const;

  //:Get a partial view from the image in the pyramid closest to scale. The origin and size parameters are in the coordinate system of the base image.
  // The scale factor is with respect to the base image (base scale = 1.0).
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 const float scale,
                                                 float& actual_scale) const;


  //: Caution! The resource is assigned a header and the data is permanently
  // written into the file. Be sure you want to commit to the file.
  bool put_resource(vil_image_resource_sptr const& resc); 

  //: for debug purposes
  void print(const unsigned level)
    {if(level<levels_.size()) levels_[level]->print(level);}
 protected:  
  //:default constructor
  vil_tiff_pyramid_resource();  
  //utility methods
  //:normalize the scale factors so that the base image scale = 1.0
  void normalize_scales();

  //:find the image resource with scale closest to specified scale
  tiff_pyramid_level* closest(const float scale) const;

  //: If true resource is open for read, else open for write
   bool read_;

  //: the tiff handle
  TIFF* t_;

  //The set of images in the pyramid. levels_[0] is the base image
  vcl_vector<tiff_pyramid_level*> levels_;
};//End of pyramid image
//
//
//------------------------ Lifted from vil_nitf2_image ------------------------
//            If this happens again then maybe should elevate to a
//            utility class -- JLM
//

//:
// This function does a lot of work for \sa byte_align_data().  It will strip one value
// of ni bits and return it as a value of type T (with zero padding on the MSBs).
// Both io and ni are lengths (in bits - not bytes).
//
// \param i0: Offset (in bits - not bytes) from in_val[0].  This will be the start
//            of the bits extracted from in_val.
// \param ni: number of bits (starting from i0) that will be extracted from in_val.
template< class T >
T tiff_get_bits( const T* in_val, unsigned i0, unsigned ni )
{
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
