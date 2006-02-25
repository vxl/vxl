// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_image.h"

//:
// \file

#include <vcl_cassert.h>
#include <vcl_cstring.h> // for std::memcpy()
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include <vil/vil_property.h>
#include "vil_nitf2_data_mask_table.h"
#include "vil_nitf2_des.h"

#ifdef VIL_USE_FSTREAM64
#include <vil/vil_stream_fstream64.h>
#endif //VIL_USE_FSTREAM64

int debug_level = 0;

//--------------------------------------------------------------------------------
// class vil_nitf2_file_format

static char const nitf2_string[] = "nitf";

char const* vil_nitf2_file_format::tag() const
{
  return nitf2_string;
}

vil_image_resource_sptr  vil_nitf2_file_format::make_input_image(vil_stream *vs)
{
  vil_nitf2_image* im = new vil_nitf2_image( vs );
  if ( !im->parse_headers() ) {
    delete im;
    im = 0;
  }
  return im;
}

vil_image_resource_sptr
  vil_nitf2_file_format::make_output_image(vil_stream* /*vs*/,
                                          unsigned /*nx*/,
                                          unsigned /*ny*/,
                                          unsigned /*nplanes*/,
                                          enum vil_pixel_format /*format*/)
{
  //write not supported
  return 0;
}

//--------------------------------------------------------------------------------
// class vil_nitf2_image

vil_streampos vil_nitf2_image::get_offset_to( vil_nitf2_header::Section sec,
                                              vil_nitf2_header::Portion por, unsigned int index ) const
{
  vil_streampos p;
  if ( sec == vil_nitf2_header::FileHeader ) {
    //there is no data section in the file header and
    //there is only one
    assert( por == vil_nitf2_header::SubHeader );
    assert( index == 0 );
    //file header is the first thing
    p = 0;
  } else {
    vil_nitf2_header::Section preceding_section = (vil_nitf2_header::Section)(sec-1);
    p = get_offset_to( preceding_section, vil_nitf2_header::SubHeader, 0 ) +
        size_to( preceding_section, vil_nitf2_header::SubHeader, -1 ) +
        size_to( sec, por, index );
  }
  return p;
}

vil_streampos vil_nitf2_image::size_to( vil_nitf2_header::Section sec, vil_nitf2_header::Portion por, int index ) const
{
  if ( sec == vil_nitf2_header::FileHeader ) {
    if ( index == -1 ) {
      int file_header_size;
      m_file_header.get_property("HL", file_header_size);
      return (vil_streampos)file_header_size;
    } else {
      return 0;
    }
  }

  vil_streampos offset = 0;
  //if -1 specified, then we want to go past all of them... that is onto the next
  //section
  bool going_past_end = false;
  if ( index == -1 ) {
    int num_segments;
    m_file_header.get_property(vil_nitf2_header::section_num_tag(sec), num_segments);
    index = num_segments;
    going_past_end = true;
  }
  vcl_string sh = vil_nitf2_header::section_len_header_tag( sec );
  vcl_string s  = vil_nitf2_header::section_len_data_tag( sec );
  int i;
  for (i = 0 ; i < index ; i++) {
    int current_header_size;
    m_file_header.get_property(sh, i, current_header_size);
    offset += current_header_size;
    if ( sec == vil_nitf2_header::ImageSegments ){
      vil_nitf2_long current_data_size;
      m_file_header.get_property(s, i, current_data_size);
      offset += current_data_size;
    } else {
      int current_data_size;
      m_file_header.get_property(s, i, current_data_size);
      offset += current_data_size;
    }
  }
  //we are now at the proper index's subheader... if we need to get to the data
  //we've got one more jump to do
  if ( por == vil_nitf2_header::Data ) {
    //if we've skipped past all the segments, then it doesn't make any sens
    //to skip to the "data" section
    assert( !going_past_end );
    int current_header_size;
    m_file_header.get_property(sh, i, current_header_size);
    offset += current_header_size;
  }
  return offset;
}

vil_image_view_base_sptr ( *vil_nitf2_image::s_decode_jpeg_2000 )
( vil_stream* vs, unsigned i0, unsigned ni, unsigned j0, unsigned nj, double i_factor, double j_factor ) = 0;

vil_nitf2_image::vil_nitf2_image(vil_stream* is)
  : m_stream(is),
    m_current_image_index(0)
{
  m_stream->ref();
}

vil_nitf2_image::vil_nitf2_image(const vcl_string& filePath, const char* mode)
  : m_current_image_index(0)
{
#ifdef VIL_USE_FSTREAM64
  m_stream = new vil_stream_fstream64(filePath.c_str(), mode);
#else //VIL_USE_FSTREAM64
  m_stream = new vil_stream_fstream(filePath.c_str(), mode);
#endif //VIL_USE_FSTREAM64
  m_stream->ref();
}

void vil_nitf2_image::clear_image_headers()
{
  for (unsigned int i = 0 ; i < m_image_headers.size() ; i++) {
    delete m_image_headers[i];
  }
  m_image_headers.clear();
}

void vil_nitf2_image::clear_des()
{
  for (unsigned int i = 0 ; i < m_des.size() ; i++) {
    delete m_des[i];
  }
  m_des.clear();
}


vil_nitf2_image::~vil_nitf2_image()
{
  m_stream->unref();
  clear_image_headers();
  clear_des();
}

unsigned int vil_nitf2_image::current_image() const
{
  return m_current_image_index;
}

void vil_nitf2_image::set_current_image(unsigned int index)
{
  assert(index < m_image_headers.size());
  m_current_image_index = index;
}

unsigned int vil_nitf2_image::nimages() const
{
  int num_images;
  if (m_file_header.get_property("NUMI", num_images)) return num_images;
  else return 0;
}

vil_streampos vil_nitf2_image::get_offset_to_image_data_block_band(
  unsigned int image_index, unsigned int block_index_x,unsigned int block_index_y, int bandIndex) const
{
  //band index is ignored when i_mode != "S"
  vcl_string i_mode;
  current_image_header()->get_property("IMODE", i_mode);

  //my image header precedes me.  Find out the offset to that, then add on the size of
  //that header... then you have the offset to me (the data)
  vil_streampos offset =
    get_offset_to( vil_nitf2_header::ImageSegments, vil_nitf2_header::Data, image_index );

  //////////////////////////////////////////////////
  // now get the position to the desired block/band
  //////////////////////////////////////////////////
  int bits_per_pixel_per_band;
  current_image_header()->get_property("NBPP", bits_per_pixel_per_band);
  unsigned int bytes_per_band = ni() * nj() * bits_per_pixel_per_band / 8;

  // What we do here depends on whether we have a data_mask_table or not and
  // whether i_mode == "S".  The most complex case is i_mode != "S" and we have
  // a dataMask table.  In that case, we get some information from the table and
  // compute some ourselves.  Here are all the possible scenarios handled here:
  //   i_mode == "S" and have data_mask_table: just ask data_mask_table for the offset
  //   i_mode == "S" and don't have data_mask_table: compute it ourselves vcl_right here
  //   i_mode != "S" and have data_mask_table: ask the data_mask_table for offset to the
  //      block we want; then compute the offset to the band ourselves here
  //   i_mode != "S" and don't have data_mask_table: compute both band and block offset
  //      ourselves here
  // If it sounds complex, blame the NITF 2.1 spec for that
  const vil_nitf2_data_mask_table* data_mask_table = current_image_header()->data_mask_table();
  if (data_mask_table) {
    offset += data_mask_table->blocked_image_data_offset();
  }
  if (data_mask_table && data_mask_table->has_offset_table()) {
    //have data mask table
    int bI = i_mode == "S" ? bandIndex : -1;
    if (data_mask_table->block_band_present(block_index_x, block_index_y, bI))
    {
      return 0;
    }
    offset += data_mask_table->block_band_offset(block_index_x, block_index_y, bI);
  } else {
      unsigned int pixels_per_block = size_block_i() * size_block_j();
      unsigned int bits_per_band = pixels_per_block * bits_per_pixel_per_band;
      unsigned int bytes_per_block_per_band = bits_per_band / 8;
      //round up if remainder left over (this assumes that band/block boundaries
      //always lie on byte boundaries.
      if (bits_per_band % 8 != 0) bytes_per_block_per_band++;
    if (i_mode == "S") {
      //i_mode == "S" and not have data_mask_table
      unsigned int offset_to_desired_band = bandIndex * bytes_per_band;
      unsigned int offset_to_desired_block = bytes_per_block_per_band * (block_index_y * n_block_i() + block_index_x);
      offset += offset_to_desired_band + offset_to_desired_block;
    } else {
      //i_mode != "S" and not have data_mask_table
      unsigned int block_size_bytes = bytes_per_block_per_band * nplanes();
      unsigned int offset_to_desired_block = block_size_bytes * (block_index_y * n_block_i() + block_index_x);
      offset += offset_to_desired_block;
    }
  }
  if (i_mode != "S") {
    //regardless of whether we had a data_mask_table or not, we've only computed
    //the offset to the desired block so far.  Now, we add on the offset to
    //the desired band.
    unsigned int offset_to_desired_band = bandIndex * bytes_per_band;
    offset += offset_to_desired_band;
  }
  return offset;
}

bool vil_nitf2_image::parse_headers()
{
  if (!m_stream->ok()) return false;
  //parse file header
  m_stream->seek(0);
  if (!m_file_header.read(m_stream)) {
    return false;
  }
  //now parse each image header
  clear_image_headers();
  m_image_headers.resize(nimages());
  for (unsigned int i = 0 ; i < nimages() ; i++) {
    vil_streampos offset = get_offset_to( vil_nitf2_header::ImageSegments, vil_nitf2_header::SubHeader, i);
    m_stream->seek(offset);
    m_image_headers[i] = new vil_nitf2_image_subheader(file_version());
    if (!m_image_headers[i]->read(m_stream)) return false;
  }

  //now parse all the DESs (if any)
  clear_des();
  int num_des;
  m_file_header.get_property( "NUMDES", num_des );
  m_des.resize( num_des );
  for ( int j = 0 ; j < num_des ; j++ ){
    vil_streampos offset = get_offset_to( vil_nitf2_header::DataExtensionSegments, vil_nitf2_header::SubHeader, j);
    m_stream->seek(offset);
    int data_width;
    m_file_header.get_property( "LD", j, data_width );
    m_des[j] = new vil_nitf2_des(file_version(), data_width);
    if (!m_des[j]->read(m_stream)) return false;
  }
  return true;
}

vil_nitf2_classification::file_version vil_nitf2_image::file_version() const
{
  return m_file_header.file_version();
}

const vil_nitf2_image_subheader* vil_nitf2_image::current_image_header() const
{
  assert(m_current_image_index < m_image_headers.size());
  return m_image_headers[ m_current_image_index ];
}

unsigned vil_nitf2_image::nplanes() const
{
  return current_image_header()->nplanes();
}

unsigned vil_nitf2_image::ni() const
{
  //Note that we are choosing to return the number of significant columns
  //rather than NPPBH*NBPR which would be the total number of pixels in the
  //image.  if NPPBH*NBPR > NCOLS, then all the extra columns are blanked
  //out pad pixels.  Why would anyone want those?
  int num_significant_cols;
  if (current_image_header()->get_property("NCOLS", num_significant_cols))
  {
    return num_significant_cols;
  }
  return 0;
}

unsigned vil_nitf2_image::nj() const
{
  //Note that we are choosing to return the number of significant rows
  //rather than NPPBV*NBPC which would be the total number of pixels in the
  //image.  if NPPBV*NBPC > NROWS, then all the extra columns are blanked
  //out pad pixels.  Why would anyone want those?
  int num_significant_rows;
  if (current_image_header()->get_property("NROWS", num_significant_rows))
  {
    return num_significant_rows;
  }
  return 0;
}

enum vil_pixel_format vil_nitf2_image::pixel_format () const
{
  vcl_string pixel_type;
  int bits_per_pixel;
  if (current_image_header()->get_property("PVTYPE", pixel_type) &&
      current_image_header()->get_property("NBPP", bits_per_pixel))
  {
    //if bits_per_pixel isn't divisible by 8, round up to nearest byte
    int bytesPerPixel = bits_per_pixel / 8;
    if (bits_per_pixel % 8 != 0) bytesPerPixel++;
    bits_per_pixel = bytesPerPixel * 8;
    if (pixel_type == "INT") {
      if (bits_per_pixel == 8) {
        return VIL_PIXEL_FORMAT_BYTE;
      } else if (bits_per_pixel == 16) {
        return VIL_PIXEL_FORMAT_UINT_16;
      } else if (bits_per_pixel == 32) {
        return VIL_PIXEL_FORMAT_UINT_32;
      }
#if VXL_HAS_INT_64
      else if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_UINT_64;
      }
#endif //VXL_HAS_INT_64
    } else if (pixel_type == "B") {
      return VIL_PIXEL_FORMAT_BOOL;
    } else if (pixel_type == "SI") {
      if (bits_per_pixel == 8) {
        return VIL_PIXEL_FORMAT_SBYTE;
      } else if (bits_per_pixel == 16) {
        return VIL_PIXEL_FORMAT_INT_16;
      } else if (bits_per_pixel == 32) {
        return VIL_PIXEL_FORMAT_INT_32;
      }
#if VXL_HAS_INT_64
      else if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_INT_64;
      }
#endif //VXL_HAS_INT_64
    } else if (pixel_type == "R") {
      if (bits_per_pixel == 32) {
        return VIL_PIXEL_FORMAT_FLOAT;
      } else if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_DOUBLE;
      }
    } else if (pixel_type == "C") {
      //two 32 bit floats (real followed by complex)
      if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_COMPLEX_FLOAT;
      }// else if (bits_per_pixel == 64) {
       // return VIL_PIXEL_FORMAT_COMPLEX_DOUBLE;
      //}
    }
  }
  return VIL_PIXEL_FORMAT_UNKNOWN;
}

unsigned int vil_nitf2_image::size_block_i() const
{
return current_image_header()->get_pixels_per_block_x();
}

unsigned int vil_nitf2_image::size_block_j() const
{
  return current_image_header()->get_pixels_per_block_y();
}

unsigned int vil_nitf2_image::n_block_i() const
{
  return current_image_header()->get_num_blocks_x();
}

unsigned int vil_nitf2_image::n_block_j() const
{
  return current_image_header()->get_num_blocks_y();
}

void  compute_block_and_offset(unsigned j0, unsigned long block_size,
                                unsigned int& block, unsigned int& offset)
{
  block = 0;
  offset = 0;

  if (j0 != 0) {
    block = (j0 / block_size);
    if (j0 > 0 && j0 % block_size != 0) {
      offset = j0 - (block * block_size);
    }
  }
}

bool vil_nitf2_image::is_jpeg_2000_compressed() const
{
  vcl_string compression_type;
  //ISO/IEC BIFF profile BPJ2k01.00 says that M8 is actually invalid
  //(ie. you can't use a data mask with jpeg 2000 compression)
  //not sure why it is an option though
  return ( current_image_header()->get_property("IC", compression_type) ) &&
         ( compression_type == "C8" || compression_type == "M8" );
}

vil_image_view_base_sptr vil_nitf2_image::get_copy_view_decimated_j2k(
  unsigned start_i, unsigned num_i, unsigned start_j, unsigned num_j, double i_factor, double j_factor ) const
{
  // ACCORDING TO DOCUMENTATION, IF PARAMETERS ARE BAD, WE SHOULD RETURN NULL POINTER.
  if ((start_i + num_i > ni()) || (start_j + num_j > nj())) {
    return 0;
  }
  assert( is_jpeg_2000_compressed() );

  //it is my understanding from BIFF profile BPJ2k01.00 that JPEG compressed files
  //will only have on image block (ie. it will be clocked within the jp2 codestream),
  //so we can just pass all the work off to the vil_j2k_image class
  m_stream->seek(get_offset_to( vil_nitf2_header::ImageSegments, vil_nitf2_header::Data, m_current_image_index));
  return s_decode_jpeg_2000( m_stream, start_i, num_i, start_j, num_j, i_factor, j_factor );
}

vil_image_view_base_sptr vil_nitf2_image::get_copy_view(unsigned start_i, unsigned num_i,
                                                         unsigned start_j, unsigned num_j) const
{
  // ACCORDING TO DOCUMENTATION, IF PARAMETERS ARE BAD, WE SHOULD RETURN NULL POINTER.
  if ((start_i + num_i > ni()) || (start_j + num_j > nj())) {
    return 0;
  }

  vcl_string compression_type;
  if (!current_image_header()->get_property("IC", compression_type)) return 0;

  //vcl_right now we only plan to support uncompressed and JPEG2000
  if (compression_type == "NC" || compression_type == "NM") {
    return get_copy_view_uncompressed(start_i, num_i, start_j, num_j);
  } else if (  s_decode_jpeg_2000 && is_jpeg_2000_compressed() ) {
    return get_copy_view_decimated_j2k( start_i, num_i, start_j, num_j, 1.0, 1.0 );
  } else {
    return 0;
  }
}

vil_image_view_base_sptr vil_nitf2_image::get_copy_view_uncompressed(unsigned start_i, unsigned num_i,
                                                                      unsigned start_j, unsigned num_j) const
{
  return vil_blocked_image_resource::get_copy_view(start_i, num_i, start_j, num_j);
}

template< class T >
vil_memory_chunk_sptr maybe_byte_align_data(vil_memory_chunk_sptr in_data, unsigned int num_samples,
                                            unsigned int in_bits_per_sample, T /*dummy*/)
{
  if (in_bits_per_sample != sizeof(T)*8) {
    vil_memory_chunk_sptr new_memory = new vil_memory_chunk(num_samples*sizeof(T), in_data->pixel_format());
    byte_align_data((T*)in_data->data(), num_samples, in_bits_per_sample, (T*)new_memory->data());
    return new_memory;
  }
  return in_data;
}

// don't do anything for float and double (bit shifting isn't allowed)
template<> vil_memory_chunk_sptr maybe_byte_align_data<float> (
  vil_memory_chunk_sptr in_data, unsigned int /* num_samples */, unsigned int /* in_bits_per_sample */, float /*dummy*/)
{ return in_data; }

template<> vil_memory_chunk_sptr maybe_byte_align_data<double> (
  vil_memory_chunk_sptr in_data, unsigned int /* num_samples */, unsigned int /* in_bits_per_sample */, double /*dummy*/)
{ return in_data; }

template<> vil_memory_chunk_sptr maybe_byte_align_data< vcl_complex< float > > (
  vil_memory_chunk_sptr in_data, unsigned int /*num_samples*/, unsigned int /*in_bits_per_sample*/, vcl_complex<float> /*dummy*/)
{ return in_data; }


//:
//  This function handles the case where the actual bits per pixel per band
//  is less then the actual bpppb AND where the data is vcl_left justified.  This
//  shifts the data so that it it vcl_right justified.
//  As of now, this function is untests as I don't have any vcl_left justified data
//  (the NITF spec discourages using it -- probably because it is such a PITA)
template< class T >
void right_justify(T* data, unsigned int num_samples, unsigned int bitsToMove)
{
  for (unsigned int i = 0 ; i < num_samples ; i++) {
    data[i] = data[i] >> bitsToMove;
  }
}

//don't do anything for bool, float and double (bit shifting isn't allowed)
template<> void right_justify<bool>(bool* /* data */, unsigned int /* num_samples */, unsigned int /* bitsToMove */) {}
template<> void right_justify<float>(float* /* data */, unsigned int /* num_samples */, unsigned int /* bitsToMove */) {}
template<> void right_justify<double>(double* /* data */, unsigned int /* num_samples */, unsigned int /* bitsToMove */) {}
template<> void right_justify< vcl_complex< float > >(vcl_complex< float >* /*data*/, unsigned int /*num_samples*/,
                                                      unsigned int /* bitsToMove */) {}

template< class T >
unsigned int get_index(T in_val)
{ return (T)in_val; }

template<> unsigned int get_index<bool>(bool in_val)
{ return in_val ? 1 : 0; }

template< class T >
vil_image_view_base_sptr get_block_vcl_internal(vil_pixel_format pix_format, vil_memory_chunk_sptr image_memory,
                                                unsigned int pixels_per_block_x, unsigned int pixels_per_block_y,
                                                unsigned int nplanes,
                                                unsigned int i_step, unsigned int j_step, unsigned int plane_step,
                                                bool need_to_right_justify,
                                                unsigned int extra_bits, unsigned int bits_per_pixel_per_band,
                                                bool data_is_all_blank, const vil_nitf2_image_subheader* /*image_header*/, T dummy)
{
  //may have to byte align data (only valid for integer type data)
  unsigned int num_samples = pixels_per_block_x * pixels_per_block_y * nplanes; //all bands of image

  if (data_is_all_blank) {
    //this entire block is blank
    T* data_ptr = reinterpret_cast<T*>(image_memory->data());
    for (unsigned int i = 0 ;
         i < pixels_per_block_x * pixels_per_block_y * nplanes ;
         i++)
    {
      data_ptr[i] = (T)0;
    }
  } else {
    //in the rare case the the actual number of bits per pixel value (ABPP) is less than the number of bits
    //used in the data (NBPP) AND the data is vcl_left justified... then we correct that here
    if (need_to_right_justify)
      right_justify<T>(static_cast<T*>(image_memory->data()), image_memory->size()/sizeof(T), extra_bits);
    //Nitf files store data in big endian... little endian machines need to convert
    vil_nitf2_data_mask_table::maybe_endian_swap(static_cast< char* >(image_memory->data()), image_memory->size(), pix_format);
    //if the data is not byte aligned (ie. the actual bits per pixel per band is not divisible
    //by 8),then we need to correct that
    image_memory = maybe_byte_align_data(image_memory, num_samples, bits_per_pixel_per_band, dummy);
  }

  vil_image_view< T >* result =
    new vil_image_view< T > (image_memory, reinterpret_cast<T*>(image_memory->data()),
                              pixels_per_block_x, pixels_per_block_y, nplanes, i_step, j_step, plane_step);

  return result;
}

vil_image_view_base_sptr vil_nitf2_image::get_block(unsigned int block_index_x, unsigned int block_index_y) const
{
  if (pixel_format() == VIL_PIXEL_FORMAT_UNKNOWN) return 0;
  vcl_string image_mode_type;
  if (!current_image_header()->get_property("IMODE", image_mode_type)) return 0;

  //calculate the start position of the block that we need
  int bits_per_pixel_per_band, actualBitsPerPixelPerBand;
  vcl_string bitJustification;
  if (!current_image_header()->get_property("NBPP", bits_per_pixel_per_band) ||
      !current_image_header()->get_property("ABPP", actualBitsPerPixelPerBand) ||
      !current_image_header()->get_property("PJUST", bitJustification)) {
    return 0;
  }
  int extra_bits = bits_per_pixel_per_band - actualBitsPerPixelPerBand;
  bool need_to_right_justify = bitJustification == "L" && (extra_bits > 0);

  //bytes per pixel... round up to nearest byte
  //unsigned int bytesPerPixelPerBand = bits_per_pixel_per_band / 8;
  //if (bits_per_pixel_per_band % 8 != 0) bytesPerPixelPerBand++;

  unsigned int pixels_per_block = size_block_i() * size_block_j();
  unsigned int bits_per_band = pixels_per_block * bits_per_pixel_per_band;
  unsigned int bytes_per_block_per_band = bits_per_band / 8;
  if (bits_per_band % 8 != 0) bytes_per_block_per_band++;     //round up if remainder vcl_left over
  unsigned int block_size_bytes = bytes_per_block_per_band * nplanes();
  //allocate the memory that we need
  vil_memory_chunk_sptr image_memory = new vil_memory_chunk(block_size_bytes, pixel_format());


  unsigned int i_step(0), j_step(0), plane_step(0);
  bool data_is_all_blank = false;
  if (image_mode_type == "S") {
#if 0 // NOT USED
    unsigned int bytes_per_band = ni() * nj() * bits_per_pixel_per_band / 8;
    unsigned int offset_to_desired_block = bytes_per_block_per_band * (block_index_y * n_block_i() + block_index_x);
#endif // 0
    //blocks are not contiguous... we'll have to do one read for each band
    for (unsigned int i = 0 ; i < nplanes() ; i++) {
      vil_streampos current_offset = get_offset_to_image_data_block_band(m_current_image_index, block_index_x, block_index_y, i);
      if (current_offset == 0) {
        //this block isn't in the stream (ie. it's all blank)... just blank out the memory
        data_is_all_blank = true;
      } else {
        m_stream->seek(current_offset);
        char* position_to_read_to = static_cast<char*>(image_memory->data());
        position_to_read_to += i*bytes_per_block_per_band;
        if (m_stream->read((void*)position_to_read_to, bytes_per_block_per_band) != static_cast<int>(bytes_per_block_per_band)) {
          return 0;
        }
      }
    }
    i_step = 1;
    j_step = size_block_i();
    plane_step = size_block_i() * size_block_j();
  } else {
    //calculate the offset we need
    vil_streampos current_offset = get_offset_to_image_data_block_band(m_current_image_index, block_index_x, block_index_y, 0);
    if (current_offset == 0) {
      //this block isn't in the stream (ie. it's all blank)... just blank out the memory
      data_is_all_blank = true;
    } else {
      //seek to the correct position in the stream
      m_stream->seek(current_offset);
      //read in the data
      if (m_stream->read(image_memory->data(), block_size_bytes) != static_cast<int>(block_size_bytes)) {
        return 0;
      }
    }

    //figure out the layout of the data in the memory chunk we just read in
    if (image_mode_type == "B") {
      //band interleaved by Block
      i_step = 1;
      j_step = size_block_i();
      plane_step = size_block_i() * size_block_j();
    } else if (image_mode_type == "P") {
      //band interleaved by Pixel
      i_step = nplanes();
      j_step = nplanes() * size_block_i();
      plane_step = 1;
    } else if (image_mode_type == "R") {
      //band interleaved by Row
      i_step = 1;
      j_step = nplanes() * size_block_i();
      plane_step = size_block_i();
    }
  }

  //create image view of the data
  vil_image_view_base_sptr view = 0;
  switch (vil_pixel_format_component_format(image_memory->pixel_format()))
  {
#define GET_BLOCK_CASE(FORMAT, T)\
   case FORMAT:{ \
    T t= (T)0; \
    return get_block_vcl_internal(\
       FORMAT, image_memory, size_block_i(),size_block_j(), nplanes(),\
       i_step, j_step, plane_step, need_to_right_justify, extra_bits, bits_per_pixel_per_band,\
       data_is_all_blank, current_image_header(), t);\
   } break

    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);

#if VXL_HAS_INT_64
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_COMPLEX_FLOAT, vcl_complex<float>);
#undef GET_BLOCK_CASE

   default:
    assert(!"Unknown vil data type.");
    break;
  }
  return view;
}
template<> bool* byte_align_data<bool>(bool* in_data, unsigned int num_samples, unsigned int in_bits_per_sample, bool* out_data)
{
  switch (sizeof(bool))
  {
   case 1:
    byte_align_data((vxl_byte*)in_data, num_samples, in_bits_per_sample, (vxl_byte*)out_data);
    break;
   case 2:
    byte_align_data((vxl_uint_16*)in_data, num_samples, in_bits_per_sample, (vxl_uint_16*)out_data);
    break;
   case 4:
    byte_align_data((vxl_uint_32*)in_data, num_samples, in_bits_per_sample, (vxl_uint_32*)out_data);
    break;
#if VXL_HAS_INT_64
   case 8:
    byte_align_data((vxl_uint_64*)in_data, num_samples, in_bits_per_sample, (vxl_uint_64*)out_data);
    break;
#endif //VXL_HAS_INT_64
   default:
    assert(!"Unsupported size of bool.");
  }

#if 0
  //dignostic info
  vcl_cout << "\nBools: ";
  for (unsigned int i = 0 ; i < num_samples ; i++) {
    vcl_cout << (out_data[i] ?  '1' : '0');
  }
  vcl_cout << vcl_endl;
#endif //0
  return out_data;
}

bool vil_nitf2_image::get_property (char const *tag, void *property_value) const
{
  if (vcl_strcmp(vil_property_size_block_i, tag)==0)
  {
    if (property_value)
      *static_cast<unsigned*>(property_value) = this->size_block_i();
    return true;
  }

  if (vcl_strcmp(vil_property_size_block_j, tag)==0)
  {
    if (property_value)
      *static_cast<unsigned*>(property_value) = this->size_block_j();
    return true;
  }
  vcl_string result;
  if (m_file_header.get_property(tag, result) ||
    (current_image_header() && current_image_header()->get_property(tag, result)))
  {
    property_value = malloc(result.size());
    vcl_memcpy(property_value, result.c_str(), result.size());
    return true;
  }
  return false;
 }

vil_nitf2_field::field_tree* vil_nitf2_image::get_tree( ) const
{
  vil_nitf2_field::field_tree* t = new vil_nitf2_field::field_tree;
  t->columns.push_back( "NITF File" );
  t->children.push_back( get_header().get_tree() );
  unsigned int i;
  for ( i = 0 ; i < m_image_headers.size() ; i++ ){
    t->children.push_back( m_image_headers[i]->get_tree(i+1) );
  }
  for ( i = 0 ; i < m_des.size() ; i++ ){
    t->children.push_back( m_des[i]->get_tree(i+1) );
  }
  return t;
}

