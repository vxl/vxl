// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_image.h"

#include <vcl_cassert.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include "vil_nitf2_data_mask_table.h"

int debug_level = 0;

vil_image_view_base_sptr ( *vil_nitf2_image::s_decode_jpeg_2000 )
( vil_stream* vs, unsigned i0, unsigned ni, unsigned j0, unsigned nj ) = 0;

vil_nitf2_image::vil_nitf2_image(vil_stream* is)
  : m_stream(is),
    m_current_image_index(0)
{ 
  m_stream->ref();
}

vil_nitf2_image::vil_nitf2_image(const vcl_string& filePath, const char* mode)
  : m_current_image_index(0)
{
  m_stream = new vil_stream_fstream(filePath.c_str(), mode);
  m_stream->ref();
}

void vil_nitf2_image::clear_image_headers()
{
  for (unsigned int i = 0 ; i < m_image_headers.size() ; i++) {
    delete m_image_headers[i];
  }
  m_image_headers.clear();
}

vil_nitf2_image::~vil_nitf2_image()
{
  m_stream->unref();
  clear_image_headers();
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
  vil_streampos offset = get_offset_to_image_data(image_index);

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
      unsigned int pixels_per_block = get_pixels_per_block_x() * get_pixels_per_block_y();
      unsigned int bits_per_band = pixels_per_block * bits_per_pixel_per_band;
      unsigned int bytes_per_block_per_band = bits_per_band / 8;
      //round up if remainder left over (this assumes that band/block boundaries
      //always lie on byte boundaries.
      if (bits_per_band % 8 != 0) bytes_per_block_per_band++;     
    if (i_mode == "S") {
      //i_mode == "S" and not have data_mask_table
      unsigned int offset_to_desired_band = bandIndex * bytes_per_band;
      unsigned int offset_to_desired_block = bytes_per_block_per_band * (block_index_y * get_num_blocks_x() + block_index_x);
      offset += offset_to_desired_band + offset_to_desired_block;
    } else {
      //i_mode != "S" and not have data_mask_table
      unsigned int block_size_bytes = bytes_per_block_per_band * nplanes();
      unsigned int offset_to_desired_block = block_size_bytes * (block_index_y * get_num_blocks_x() + block_index_x);
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

vil_streampos vil_nitf2_image::get_offset_to_image_data(unsigned int image_index) const
{
  //my image header precedes me.  Find out the offset to that, then add on the size of 
  //that header... then you have the offset to me (the data)
  vil_streampos offset_to_data = get_offset_to_image_subheader(image_index);
  int header_size;
  m_file_header.get_property("LISH", image_index, header_size);
  offset_to_data += header_size;
  return offset_to_data;
}

vil_streampos vil_nitf2_image::get_offset_to_image_subheader(unsigned int image_index) const
{
  assert(image_index < nimages());

  //first get the length of the file header (that comes first)
  int offset_to_first_image_subheader;
  m_file_header.get_property("HL", offset_to_first_image_subheader);
  vil_streampos offset = offset_to_first_image_subheader;

  //now add up all the offsets of image headers and image data that precedes me
  for (unsigned int i = 0 ; i < image_index ; i++) {
    int current_header_size;
    long long current_data_size;
    m_file_header.get_property("LISH", i, current_header_size);
    m_file_header.get_property("LI", i, current_data_size);
    //WARNING: possibility for overflow... need to fix vil_streampos
    //to be 64 bits
    offset += current_header_size + current_data_size;
  }
  return offset;
}

bool vil_nitf2_image::parse_headers()
{
  if (!m_stream->ok()) return false;
  //parse file header
  m_stream->seek(0);
  if (!m_file_header.read(m_stream)) return false;
  
  //now parse each image header
  clear_image_headers();
  m_image_headers.resize(nimages());
  for (unsigned int i = 0 ; i < nimages() ; i++) {
    vil_streampos offset = get_offset_to_image_subheader(i);
    m_stream->seek(offset);
    m_image_headers[i] = new vil_nitf2_image_subheader(file_version());
    if (!m_image_headers[i]->read(m_stream)) return false;
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
      } else if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_UINT_64;
      }
    } else if (pixel_type == "B") {
      return VIL_PIXEL_FORMAT_BOOL;
    } else if (pixel_type == "SI") {
      if (bits_per_pixel == 8) {
        return VIL_PIXEL_FORMAT_SBYTE;
      } else if (bits_per_pixel == 16) {
        return VIL_PIXEL_FORMAT_INT_16;
      } else if (bits_per_pixel == 32) {
        return VIL_PIXEL_FORMAT_INT_32;
      } else if (bits_per_pixel == 64) {
        return VIL_PIXEL_FORMAT_INT_64;
      }
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

unsigned int vil_nitf2_image::get_pixels_per_block_x() const
{
return current_image_header()->get_pixels_per_block_x();
}

unsigned int vil_nitf2_image::get_pixels_per_block_y() const 
{
  return current_image_header()->get_pixels_per_block_y();
}

unsigned int vil_nitf2_image::get_num_blocks_x() const 
{
  return current_image_header()->get_num_blocks_x();
}

unsigned int vil_nitf2_image::get_num_blocks_y() const 
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
  } else if (  s_decode_jpeg_2000 && 
              ( compression_type == "C8" || compression_type == "M8" ) ) 
  {
    //ISO/IEC BIFF profile BPJ2k01.00 says that M8 is actually invalid
    //(ie. you can't use a data mask with jpeg 2000 compression)
    //not sure why it is an option though

    //it is my understanding from BIFF profile BPJ2k01.00 that JPEG compressed files
    //will only have on image block (ie. it will be clocked within the jp2 codestream),
    //so we can just pass all the work off to the vil_j2k_image class
    m_stream->seek(get_offset_to_image_data(m_current_image_index));
    return s_decode_jpeg_2000( m_stream, start_i, num_i, start_j, num_j );
  } else {
    return 0;
  }
}

vil_image_view_base_sptr vil_nitf2_image::get_copy_view_uncompressed(unsigned start_i, unsigned num_i,
                                                                      unsigned start_j, unsigned num_j) const
{
  vil_image_view_base_sptr image_view = 0;  // RETURN VALUE

  //figure out which blocks need to be read in
  unsigned start_block_x = 0;
  unsigned start_block_x_offset = 0;  // offset to first column in starting x block
  unsigned end_block_x = get_num_blocks_x()-1;
  unsigned end_block_x_offset = get_pixels_per_block_x();  //!< # of pixels to use in last block in row

  unsigned start_block_y = 0;
  unsigned start_block_y_offset = 0;  // offset to first row in starting y  block
  unsigned end_block_y = get_num_blocks_y()-1;
  unsigned end_block_y_offset = get_pixels_per_block_y();  //!< # of pixels to use in last block in column

  compute_block_and_offset(start_i,         get_pixels_per_block_x(), start_block_x, start_block_x_offset);
  compute_block_and_offset(start_i+num_i-1, get_pixels_per_block_x(), end_block_x,   end_block_x_offset);
  compute_block_and_offset(start_j,         get_pixels_per_block_y(), start_block_y, start_block_y_offset);
  compute_block_and_offset(start_j+num_j-1, get_pixels_per_block_y(), end_block_y,   end_block_y_offset);

  //read in all the blocks that we need to construct our image
  vcl_vector< vcl_vector< vil_image_view_base_sptr > > required_blocks;
  get_blocks(start_block_x, end_block_x, start_block_y, end_block_y, required_blocks);
  //if any of the blocks failed to be read in (e.g., for unsupported compression type)
  //then we can't create our output image.  Exit
  for (unsigned int i = 0 ; i < required_blocks.size() ; i++) {
    for (unsigned int j = 0 ; j < required_blocks.size() ; j++) {
      if (!required_blocks[i][j]) return 0;
    }
  }

  //trim the border blocks (ie. if the region doesn't lie exactly on block
  //boundaries
  trim_border_blocks(required_blocks, 
    start_block_x_offset, start_block_y_offset, 
    end_block_x_offset, end_block_y_offset);
  //now piece together the blocks that we need 
  return glue_blocks_together(required_blocks);
}

vil_image_view_base_sptr vil_nitf2_image::glue_blocks_together(
  const vcl_vector< vcl_vector< vil_image_view_base_sptr > >& data_blocks) const
{
  //first calculate the overall size of the output image (all blocks glued together)
  unsigned int output_width  = 0;
  unsigned int output_height = 0;
  unsigned int i;
  for (i = 0 ; i < data_blocks.size() ; i++) {
    output_width += data_blocks[i][0]->ni();   
  }
  for (i = 0 ; i < data_blocks[0].size() ; i++) {
    output_height += data_blocks[0][i]->nj();   
  }

  //now paste all the image blocks into their proper location in outImage
  unsigned int curr_pos_x = 0;
  unsigned int curr_pos_y = 0;
  vil_image_view_base_sptr result;
  switch(vil_pixel_format_component_format(pixel_format())) {

#define GLUE_BLOCK_CASE(FORMAT, T) \
  case FORMAT: { \
    vil_image_view< T >* output_image = new vil_image_view< T >(output_width, output_height, nplanes()); \
    for (unsigned int x = 0 ; x < data_blocks.size() ; x++) { \
      for (unsigned int y = 0 ; y < data_blocks[x].size() ; y++) { \
        vil_copy_to_window(static_cast<vil_image_view< T >&>(*data_blocks[x][y]), *output_image, curr_pos_x, curr_pos_y); \
        curr_pos_y += data_blocks[x][y]->nj(); \
      } \
      curr_pos_y = 0; \
      curr_pos_x += data_blocks[x][0]->ni(); \
    } \
    result = output_image; \
    return result; \
  } break;

  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)
#if VXL_HAS_INT_64
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)
#endif
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double)
  GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_COMPLEX_FLOAT, vcl_complex<float>)
#undef GLUE_BLOCK_CASE

  default: 
    assert(0);
    //"Unknown vil data type.");
    return 0;
  }
}

template< class T >
vil_memory_chunk_sptr maybe_byte_align_data(vil_memory_chunk_sptr in_data, unsigned int num_samples, unsigned int in_bits_per_sample)
{
  if (in_bits_per_sample != sizeof(T)*8) {
    vil_memory_chunk_sptr new_memory = new vil_memory_chunk(num_samples*sizeof(T), in_data->pixel_format());
    byte_align_data<T>((T*)in_data->data(), num_samples, in_bits_per_sample, (T*)new_memory->data());
    return new_memory;
  }
  return in_data;
}
// don't do anything for float and double (bit shifting isn't allowed)
template<> vil_memory_chunk_sptr maybe_byte_align_data<float> (
  vil_memory_chunk_sptr in_data, unsigned int /* num_samples */, unsigned int /* in_bits_per_sample */)
{ return in_data; }

template<> vil_memory_chunk_sptr maybe_byte_align_data<double> (
  vil_memory_chunk_sptr in_data, unsigned int /* num_samples */, unsigned int /* in_bits_per_sample */)
{ return in_data; }

template<> vil_memory_chunk_sptr maybe_byte_align_data< vcl_complex< float > > (
  vil_memory_chunk_sptr in_data, unsigned int /* num_samples */, unsigned int /* in_bits_per_sample */)
{ return in_data; }


/**
  * This function handles the case where the actual bits per pixel per band
  * is less then the actual bpppb AND where the data is vcl_left justified.  This
  * shifts the data so that it it vcl_right justified.  
  * As of now, this function is untests as I don't have any vcl_left justified data
  * (the NITF spec discourages using it -- probably because it is such a PITA)
  */
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
template<> void right_justify< vcl_complex< float > >(vcl_complex< float >* /* data */, unsigned int /* num_samples */, unsigned int /* bitsToMove */) {}

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
                                             bool data_is_all_blank, const vil_nitf2_image_subheader* /* image_header */) 
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
    if (need_to_right_justify) right_justify<T>(static_cast<T*>(image_memory->data()), image_memory->size() / sizeof(T), extra_bits); 
    //Nitf files store data in big endian... little endian machines need to convert
    vil_nitf2_data_mask_table::maybe_endian_swap(static_cast< char* >(image_memory->data()), image_memory->size(), pix_format); 
    //if the data is not byte aligned (ie. the actual bits per pixel per band is not divisible
    //by 8),then we need to correct that
    image_memory = maybe_byte_align_data<T>(image_memory, num_samples, bits_per_pixel_per_band); 
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

  unsigned int pixels_per_block = get_pixels_per_block_x() * get_pixels_per_block_y();
  unsigned int bits_per_band = pixels_per_block * bits_per_pixel_per_band;
  unsigned int bytes_per_block_per_band = bits_per_band / 8;
  if (bits_per_band % 8 != 0) bytes_per_block_per_band++;     //round up if remainder vcl_left over
  unsigned int block_size_bytes = bytes_per_block_per_band * nplanes();
  //allocate the memory that we need
  vil_memory_chunk_sptr image_memory = new vil_memory_chunk(block_size_bytes, pixel_format());
  

  unsigned int i_step, j_step, plane_step;
  bool data_is_all_blank = false;
  if (image_mode_type == "S") {
//NOT USED    unsigned int bytes_per_band = ni() * nj() * bits_per_pixel_per_band / 8;
//NOT USED    unsigned int offset_to_desired_block = bytes_per_block_per_band * (block_index_y * get_num_blocks_x() + block_index_x);
    //block's are not contiguous... we'll have to do one read for each band
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
    j_step = get_pixels_per_block_x();
    plane_step = get_pixels_per_block_x() * get_pixels_per_block_y();
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
      j_step = get_pixels_per_block_x();
      plane_step = get_pixels_per_block_x() * get_pixels_per_block_y();
    } else if (image_mode_type == "P") {
      //band interleaved by Pixel
      i_step = nplanes();
      j_step = nplanes() * get_pixels_per_block_x();
      plane_step = 1; 
    } else if (image_mode_type == "R") {
      //band interleaved by Row
      i_step = 1;
      j_step = nplanes() * get_pixels_per_block_x();
      plane_step = get_pixels_per_block_x(); 
    }
  }
  
  //create image view of the data
  vil_image_view_base_sptr view = 0;
  switch(vil_pixel_format_component_format(image_memory->pixel_format())) {

#define GET_BLOCK_CASE(FORMAT, T)\
  case FORMAT:\
    return get_block_vcl_internal< T >(\
       FORMAT, image_memory, get_pixels_per_block_x(),get_pixels_per_block_y(), nplanes(),\
       i_step, j_step, plane_step, need_to_right_justify, extra_bits, bits_per_pixel_per_band,\
       data_is_all_blank, current_image_header());\
    break;

  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)

#if VXL_HAS_INT_64
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)

#endif
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double)
  GET_BLOCK_CASE(VIL_PIXEL_FORMAT_COMPLEX_FLOAT, vcl_complex<float>)

#undef GET_BLOCK_CASE

  default: 
    assert(0);
    //"Unknown vil data type.");
    break;
  }
  return view;
}
void vil_nitf2_image::get_blocks(unsigned int start_block_x, unsigned int end_block_x, 
                                   unsigned int start_block_y, unsigned int end_block_y,
                                   vcl_vector< vcl_vector< vil_image_view_base_sptr > >& out_data_blocks) const
{
  out_data_blocks.clear();
  out_data_blocks.resize(end_block_x - start_block_x + 1);
  for (unsigned int curr_block_x = start_block_x ; curr_block_x <= end_block_x ; curr_block_x++) {
    unsigned int i = curr_block_x - start_block_x;
    out_data_blocks[i].resize(end_block_y - start_block_y + 1);
    for (unsigned int curr_block_y = start_block_y ; curr_block_y <= end_block_y ; curr_block_y++) {
      unsigned int j = curr_block_y - start_block_y;
      out_data_blocks[i][j] = get_block(curr_block_x, curr_block_y);
    }
  }
}

bool vil_nitf2_image::trim_border_blocks(
  vcl_vector< vcl_vector< vil_image_view_base_sptr > >& data_blocks, 
  unsigned int start_block_x_offset, unsigned int start_block_y_offset, 
  unsigned int end_block_x_offset, unsigned int end_block_y_offset) const
{
  //loop thorugh all the boxes and trim the boxes around the border
  //if necessary.
  for (unsigned int curr_block_x = 0 ; curr_block_x < data_blocks.size() ; curr_block_x++) {
    for (unsigned int curr_block_y = 0 ; curr_block_y < data_blocks[curr_block_x].size() ; curr_block_y++) {
      if (!data_blocks[curr_block_x][curr_block_y]) continue;
      //booleans that tell me whether this box is some sort of border box
      bool first_block_in_row = curr_block_x == 0; 
      bool first_block_in_col = curr_block_y == 0; 
      bool last_block_in_row = curr_block_x == data_blocks.size()-1;
      bool last_block_in_col = curr_block_y == data_blocks[curr_block_x].size()-1;

      //nothing to do if this isn't a border box
      if (!first_block_in_row && !first_block_in_col && !last_block_in_row && !last_block_in_col) continue;

      //the region that we are going to carve out of the current data block
      unsigned int i0 = first_block_in_row ? start_block_x_offset : 0;
      unsigned int in = last_block_in_row  ? end_block_x_offset   : data_blocks[curr_block_x][curr_block_y]->ni()-1;
      unsigned int j0 = first_block_in_col ? start_block_y_offset : 0;
      unsigned int jn = last_block_in_col  ? end_block_y_offset : data_blocks[curr_block_x][curr_block_y]->nj()-1;
      
      switch(vil_pixel_format_component_format(pixel_format())) {

#define TRIM_BORDER_BLOCK_CASE(FORMAT, T) \
      case FORMAT: { \
        vil_image_view< T > currBlock = static_cast<vil_image_view< T >&>(*data_blocks[curr_block_x][curr_block_y]);\
        vil_image_view< T >* croppedBlock = new vil_image_view< T >();\
        *croppedBlock = vil_crop(currBlock, i0, in-i0+1, j0, jn-j0+1);\
        data_blocks[curr_block_x][curr_block_y] = croppedBlock;\
      } break;

      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)
#if VXL_HAS_INT_64
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)
#endif
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double)
      TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_COMPLEX_FLOAT,  vcl_complex<float>)
#undef TRIM_BORDER_BLOCK_CASE

      default: 
        assert(0);
        //"Unknown vil data type.");
        return false;;
      }
    }
  }
  return true;
}

template<> bool* byte_align_data<bool>(bool* in_data, unsigned int num_samples, unsigned int in_bits_per_sample, bool* out_data)
{
  switch(sizeof(bool)) {
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
      assert(0);
  }

#if 0
  //dignostic info
  vcl_cout << "\nBools: ";
  for (unsigned int i = 0 ; i < num_samples ; i++) {
    vcl_cout << (out_data[i] == true ?  "1" : "0");
  }
  vcl_cout << vcl_endl;
#endif //0
  return out_data;
}

bool vil_nitf2_image::get_property (char const *tag, void *property_value) const
{
  vcl_string result;
  if (m_file_header.get_property(tag, result) ||
    (current_image_header() && current_image_header()->get_property(tag, result)))
  {
    property_value = malloc(result.size());
    memcpy(property_value, result.c_str(), result.size());
    return true;
  }
  return false;
 }