#ifdef __GNUC__
#pragma implementation "vil_mit.h"
#endif
//
// Author: Joris Schouteden
// Created: 18 Feb 2000
// Modifications:
//   000218 JS  Initial version, header info from MITImage.C
//
//-----------------------------------------------------------------------------

#include "vil_mit.h"

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_generic_image.h>

static char const* vil_mit_format_tag = "mit";

#ifdef __GNUC__
# warning "byte ordering is machine dependent"
#endif
#define swapShort(a) ((a & 255) << 8) | ((a >> 8) & 255)

vil_generic_image* vil_mit_file_format::make_input_image(vil_stream* is)
{
  is->seek(0);

  unsigned short temp;
  is->read(&temp, sizeof(unsigned short));
  int type = swapShort(temp);

  is->read(&temp, sizeof(unsigned short));
  int bits_per_pixel = swapShort(temp);

  is->read(&temp, sizeof(unsigned short));
  int width = swapShort(temp);
  
  is->read(&temp, sizeof(unsigned short));
  int height = swapShort(temp);

  short t = type;

  cerr << __FILE__ " : here we go:\n";
  cerr << __FILE__ " : type_ = " << type << endl;
  cerr << __FILE__ " : bits_per_pixel_ = " << bits_per_pixel << endl;
  cerr << __FILE__ " : width_ = " << width << endl;
  cerr << __FILE__ " : height_ = " << height << endl;

  if (t > 7 || t < 1)
    return 0;

  return new vil_mit_generic_image(is);
}

vil_generic_image* vil_mit_file_format::make_output_image(vil_stream* is, vil_generic_image const* prototype)
{
  return new vil_mit_generic_image(is, prototype);
}

char const* vil_mit_file_format::tag() const
{
  return vil_mit_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_mit_generic_image::vil_mit_generic_image(vil_stream* is):
  is_(is)
{
  read_header();
}

char const* vil_mit_generic_image::file_format() const
{
  return vil_mit_format_tag;
}

vil_mit_generic_image::vil_mit_generic_image(vil_stream* is, vil_generic_image const* prototype):
  is_(is)
{
  width_ = prototype->width();
  height_ = prototype->height();
  components_ = prototype->components();
  bits_per_component_ = prototype->bits_per_component();
  bits_per_pixel_ = bits_per_component_ * components_;

  if (bits_per_component_ == 8 || bits_per_component_ == 16)
  {
    if (components_ == 3) type_ = 2;
    else if (components_ == 1) type_ = 1;
    else cerr << __FILE__ " : Can only write RGB or grayscale\n";
  }
  else 
  { 
    if (components_ == 1) type_ = 6; 
    else  
      cerr << __FILE__ " : Ah can only write 8 or 16 bit images\n";
  }

  write_header();
}

bool vil_mit_generic_image::read_header()
{
  is_->seek(0);

  unsigned short temp;
  is_->read(&temp, sizeof(unsigned short));
  type_ = swapShort(temp);

  is_->read(&temp, sizeof(unsigned short));
  bits_per_pixel_ = swapShort(temp);

  is_->read(&temp, sizeof(unsigned short));
  width_ = swapShort(temp);

  is_->read(&temp, sizeof(unsigned short));
  height_ = swapShort(temp);

  if (type_ > 7 || type_ < 1)
    return false;

  if (type_ == 1) components_ = 1;   // gray
  else if (type_ == 2) components_ = 3;  // rgb
  else if (type_ == 6) components_ = 1;  // float 

  return true; 
}

bool vil_mit_generic_image::write_header()
{
  is_->seek(0);
  unsigned short temp;
  temp = swapShort(type_);  
  is_->write((void*)&temp, 2);
  temp = swapShort(bits_per_pixel_);
  is_->write((void*)&temp, 2);
  temp = swapShort(width_);
  is_->write((void*)&temp, 2);
  temp = swapShort(height_); 
  is_->write((void*)&temp, 2);

  return true;
}

bool vil_mit_generic_image::do_get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if ( !buf )
  {
    buf = new unsigned char[bytes_per_pixel() * xs * ys];
    if (!buf) return false;
  }

  int offset = 4; 
  
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

bool vil_mit_generic_image::do_put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  assert(buf); 

  int skip = bytes_per_pixel() * (width_ - xs);

  int offset = 8;
  is_->seek(offset + (width_*y0*bytes_per_pixel()) + (x0*bytes_per_pixel()));

  unsigned char* point = (unsigned char*)buf;

  // FIXME: store as BGR
  for (int tely = 0; tely < ys; tely++)
  {
    is_->write(point, xs * bytes_per_pixel());
    is_->seek(is_->tell() + skip);
    point += (xs * bytes_per_pixel());
  }

  return true; 
}

vil_generic_image* vil_mit_generic_image::get_plane(int plane) const 
{
  assert(plane == 0);
  return const_cast<vil_mit_generic_image*>(this);
}
