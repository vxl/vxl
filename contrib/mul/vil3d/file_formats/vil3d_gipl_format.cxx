// This is mul/vil3d/file_formats/vil3d_gipl_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for GIPL format images.
// \author Tim Cootes - Manchester

#include "vil3d_gipl_format.h"
#include <vcl_cassert.h>
#include <vil2/vil2_stream_16bit.h>
#include <vil2/vil2_stream_32bit.h>
#include <vil3d/vil3d_image_view.h>

// GIPL magic number
const unsigned GIPL_MAGIC = 719555000;

// GIPL header size
#define GIPL_HEADERSIZE   256

// GIPL filter types
#define GIPL_BINARY       1
#define GIPL_CHAR         7
#define GIPL_U_CHAR       8
#define GIPL_SHORT        15
#define GIPL_U_SHORT      16
#define GIPL_U_INT        31
#define GIPL_INT          32
#define GIPL_FLOAT        64
#define GIPL_DOUBLE       65
#define GIPL_C_SHORT      144
#define GIPL_C_INT        160
#define GIPL_C_FLOAT      192
#define GIPL_C_DOUBLE     193


// The following function should be moved to relevant places in vil2 soon
static void swap16(char *a, unsigned n)
{
  char c;
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    c = a[i]; a[i] = a[i+1]; a[i+1] = c;
  }
}

// The following function should be moved to relevant places in vil2 soon
static void swap32(char *a, unsigned n)
{
  char c;
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    c = a[i];
    a[i] = a[i+3];
    a[i+3] = c;
    c = a[i+1];
    a[i+1] = a[i+2];
    a[i+2] = c;
  }
}

// The following function should be moved to relevant places in vil2 soon
inline float vil2_stream_32bit_read_big_endian_float(vil2_stream* is)
{
  float f;
  is->read((char*)&f,4);
#ifdef VXL_LITTLE_ENDIAN
  swap32((char*)&f,1);
#endif
  return f;
}

// The following function should be moved to relevant places in vil2 soon
// Reads in n shorts, assumed to be two bytes, into data[i]
inline void vil2_stream_16bit_read_big_endian_shorts(vil2_stream* is,
                                                     vxl_uint_16* data, unsigned n)
{
  assert(sizeof(short)==2);
  is->read((char*)data,n*2);
#ifdef VXL_LITTLE_ENDIAN
  swap16((char*)data,n);
#endif
}

//: Read header and image from given stream if possible
bool vil3d_gipl_format::read_stream(vil3d_header_data_sptr& header,
                                    vil3d_image_view_base_sptr& image,
                                    vil2_stream *is)
{
  is->seek(252);
  unsigned magic_number = vil2_stream_32bit_read_big_endian(is);
  if (magic_number!=GIPL_MAGIC) return false;

  // Only read basic stuff from the header, so use base object
  header = new vil3d_header_data;

  // Return to start
  is->seek(0);
  unsigned short dim1 = vil2_stream_16bit_read_big_endian(is);
  unsigned short dim2 = vil2_stream_16bit_read_big_endian(is);
  unsigned short dim3 = vil2_stream_16bit_read_big_endian(is);

  is->seek(8);
  unsigned short gipl_pixel_type = vil2_stream_16bit_read_big_endian(is);

  float vox_width1 = vil2_stream_32bit_read_big_endian_float(is);
  float vox_width2 = vil2_stream_32bit_read_big_endian_float(is);
  float vox_width3 = vil2_stream_32bit_read_big_endian_float(is);
  vcl_cout<<"Voxel widths: "<<vox_width1<<" x "<<vox_width2<<" x "<<vox_width3<<vcl_endl;

  header->set_size(dim1,dim2,dim3);
  header->set_voxel_widths(vox_width1,vox_width2,vox_width3);

  vil3d_image_view<vxl_byte>*  byte_image;
//  vil3d_image_view<vxl_sbyte>* sbyte_image;
  vil3d_image_view<vxl_uint_16>*  uint_16_image;
  switch (gipl_pixel_type)
  {
    case (1): // Binary
      vcl_cout<<"vil3d_gipl_format::read_stream()"
              <<" Binary not yet implemented\n";
      return false;
#if 0
      case (7): // Char
      sbyte_image = new vil3d_image_view<vxl_sbyte>;
      sbyte_image->set_size(dim1,dim2,dim3);
      is->seek(GIPL_HEADERSIZE);
      is->read(sbyte_image->origin_ptr(),sbyte_image->size());
      image = sbyte_image;
      header->set_pixel_format(sbyte_image->pixel_format());
      return true;
#endif
    case (8): // UChar
      byte_image = new vil3d_image_view<vxl_byte>;
      byte_image->set_size(dim1,dim2,dim3);
      is->seek(GIPL_HEADERSIZE);
      is->read(byte_image->origin_ptr(),byte_image->size());
      image = byte_image;
      header->set_pixel_format(byte_image->pixel_format());
      return true;
    case (15): // Short
      uint_16_image = new vil3d_image_view<vxl_uint_16>;
      uint_16_image->set_size(dim1,dim2,dim3);
      is->seek(GIPL_HEADERSIZE);
      vil2_stream_16bit_read_big_endian_shorts(is,uint_16_image->origin_ptr(),
                                                  uint_16_image->size());
      image = uint_16_image;
      header->set_pixel_format(uint_16_image->pixel_format());
      return true;
    case (16): // UShort
    case (31): // U Int
    case (32): // Int
    case (64): // Float
    case (65): // Double
    case (144): // C.Short
    case (160): // C.Int
    case (192): // C.Float
    case (193): // C.Double
    default:
      vcl_cout<<"vil3d_gipl_format::read_stream()";
      vcl_cout<<" Unknown pixel type\n";
      return false;
  }

  return true;
}

//: Write header and image to given stream if possible
bool vil3d_gipl_format::write_stream(const vil3d_header_data_sptr& header,
                                     const vil3d_image_view_base_sptr& image,
                                     vil2_stream *os)
{
  vcl_cerr<<"Writing to GIPL format not yet implemented. Sorry.\n";
  return false;
}

