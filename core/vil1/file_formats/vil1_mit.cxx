#ifdef __GNUC__
#pragma implementation
#endif
//
// Author: Joris Schouteden
// Created: 18 Feb 2000
//
//-----------------------------------------------------------------------------

#include "vil_mit.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_16bit.h>
#include <vil/vil_property.h>

static char const* vil_mit_format_tag = "mit";

#define MIT_UNSIGNED    0x0001
#define MIT_RGB         0x0002
#define MIT_HSB         0x0003
#define MIT_CAP         0x0004
#define MIT_SIGNED      0x0005
#define MIT_FLOAT       0x0006
#define MIT_EDGE        0x0007

#define MIT_UCOMPLEX    0x0101
#define MIT_SCOMPLEX    0x0105
#define MIT_FCOMPLEX    0x0106

#define MIT_UNSIGNED_E  0x0201
#define MIT_SIGNED_E    0x0205
#define MIT_FLOAT_E     0x0206

#define MIT_UCOMPLEX_E  0x0301
#define MIT_SCOMPLEX_E  0x0305
#define MIT_FCOMPLEX_E  0x0306

#define EDGE_HOR 0200           /* Edge direction codes */
#define EDGE_VER 0100

// The mit image format is encoded (in little-endian format) as follows :
//   2 bytes : magic number
//   2 bytes : number of bits per pixel
//   2 bytes : width
//   2 bytes : height
//   raw image data follows.
//
// E.g. :
// 00000000: 01 00 08 00 67 01 5A 01 6D 6D 6D 6D 6D 6D 6D 6D ....g.Z.mmmmmmmm
// 00000010: 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D 6D mmmmmmmmmmmmmmmm

//----------------------------------------------------------------------

vil_image_impl* vil_mit_file_format::make_input_image(vil_stream* is)
{
  is->seek(0L);
  int type = vil_16bit_read_little_endian(is);

  if (!(type == MIT_UNSIGNED ||
        type == MIT_RGB      ||
        type == MIT_HSB      ||
        type == MIT_CAP      ||
        type == MIT_SIGNED   ||
        type == MIT_FLOAT    ||
        type == MIT_EDGE      ))
    return 0;

  int bits_per_pixel = vil_16bit_read_little_endian(is);
  if (bits_per_pixel > 32) {
    vcl_cerr << "vil_mit_file_format:: Thought it was MIT, but bpp = " << bits_per_pixel << vcl_endl;
    return 0;
  }

  /*int width =*/ vil_16bit_read_little_endian(is);
  /*int height=*/ vil_16bit_read_little_endian(is);
#if 0
  vcl_cerr << __FILE__ " : here we go:\n"
           << __FILE__ " : type_ = " << type << vcl_endl
           << __FILE__ " : bits_per_pixel_ = " << bits_per_pixel << vcl_endl
           << __FILE__ " : width_ = " << width << vcl_endl
           << __FILE__ " : height_ = " << height << vcl_endl;
#endif
  return new vil_mit_generic_image(is);
}

vil_image_impl* vil_mit_file_format::make_output_image(vil_stream* is, int planes,
                                                       int width,
                                                       int height,
                                                       int components,
                                                       int bits_per_component,
                                                       vil_component_format format)
{
  return new vil_mit_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil_mit_file_format::tag() const
{
  return vil_mit_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_mit_generic_image::vil_mit_generic_image(vil_stream* is):
  is_(is)
{
  is_->ref();
  read_header();
}

bool vil_mit_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

char const* vil_mit_generic_image::file_format() const
{
  return vil_mit_format_tag;
}

vil_mit_generic_image::vil_mit_generic_image(vil_stream* is, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil_component_format format):
  is_(is)
{
  is_->ref();
  width_ = width;
  height_ = height;
  components_ = components;
  bits_per_component_ = bits_per_component;
  bits_per_pixel_ = bits_per_component_ * components_;

  if (bits_per_component_ == 8 || bits_per_component_ == 16)
  {
    if (components_ == 3) type_ = 2;
    else if (components_ == 1) type_ = 1;
    else vcl_cerr << __FILE__ " : Can only write RGB or grayscale\n";
  }
  else
  {
    if (components_ == 1) type_ = 6;
    else
      vcl_cerr << __FILE__ " : Ah can only write 8 or 16 bit images\n";
  }

  write_header();
}

vil_mit_generic_image::~vil_mit_generic_image()
{
  is_->unref();
}

bool vil_mit_generic_image::read_header()
{
  is_->seek(0L);

  type_ = vil_16bit_read_little_endian(is_);
  bits_per_pixel_ = vil_16bit_read_little_endian(is_);
  width_ = vil_16bit_read_little_endian(is_);
  height_ = vil_16bit_read_little_endian(is_);

  if (type_ > 7 || type_ < 1)
    return false;

  if (type_ == 1) components_ = 1;   // gray
  else if (type_ == 2) components_ = 3;  // rgb
  else if (type_ == 6) components_ = 1;  // float

  return true;
}

bool vil_mit_generic_image::write_header()
{
  is_->seek(0L);
  vil_16bit_write_little_endian(is_, type_);
  vil_16bit_write_little_endian(is_, bits_per_pixel_);
  vil_16bit_write_little_endian(is_, width_);
  vil_16bit_write_little_endian(is_, height_);
  return true;
}

bool vil_mit_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  assert(buf != 0);

  vil_streampos offset = 8; // fsm: was 4

  int skip = bytes_per_pixel() * (width_ - xs);

  unsigned char *point = (unsigned char*)buf;

  is_->seek(offset + (width_*y0*bytes_per_pixel()) + (x0*bytes_per_pixel()));


  // FIXME: BGR
  for (int tely = 0; tely < ys; tely++)
  {
    is_->read(point, xs * bytes_per_pixel());
    is_->seek(is_->tell() + skip);
    point += (xs * bytes_per_pixel());
  }

  return true;
}

bool vil_mit_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  assert(buf != 0);

  int skip = bytes_per_pixel() * (width_ - xs);

  vil_streampos offset = 8;
  is_->seek(offset + (width_*y0*bytes_per_pixel()) + (x0*bytes_per_pixel()));

  const unsigned char* point = (const unsigned char*)buf;

  // FIXME: store as BGR
  for (int tely = 0; tely < ys; tely++)
  {
    is_->write(point, xs * bytes_per_pixel());
    is_->seek(is_->tell() + skip);
    point += (xs * bytes_per_pixel());
  }

  return true;
}

vil_image vil_mit_generic_image::get_plane(int plane) const
{
  assert(plane == 0);
  return const_cast<vil_mit_generic_image*>(this);
}
