// This is core/vil1/file_formats/vil1_iris.cxx
//
// Author: Joris Schouteden
// Created: 17 Feb 2000
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include "vil1_iris.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

static short get_short(vil1_stream* file, int location = -1); // default -1 means: read at current position
static unsigned short get_ushort(vil1_stream* file, int location = -1);
static char get_char(vil1_stream* file, int location = -1);
static long get_long(vil1_stream* file, int location = -1);
static void send_char(vil1_stream* data, int s);
static void send_short(vil1_stream* data, int s);
static void send_ushort(vil1_stream* data, unsigned int s);
static void send_long(vil1_stream* data, long s);
static void expandrow(unsigned char *optr, unsigned char *iptr, int z);


char const* vil1_iris_format_tag = "iris";

vil1_image_impl* vil1_iris_file_format::make_input_image(vil1_stream* is)
{
  is->seek(0L);

  int colormap_;

  int magic_     = get_short(is);
  int storage_   = get_char(is);
  int bytes_per_component_ = get_char(is);
  int dimension_ = get_ushort(is);
  /*int width_     =*/ get_ushort(is);
  /*int height_    =*/ get_ushort(is);
  /*int planes_    =*/ get_ushort(is);
  /*int pixmin_    =*/ get_long(is);
  /*int pixmax_    =*/ get_long(is);

  is->seek(24L);
  char imagename[81];
  is->read(imagename, 80L);

  colormap_ = get_long(is);

  if (magic_ != 474) return nullptr;
  if (storage_ != 0 && storage_ != 1) return nullptr;
  if (colormap_ == 3) return nullptr;
  if (dimension_ == 3 && colormap_ != 0) return nullptr;
  if (dimension_ > 3 || dimension_ < 1) return nullptr;
  if (bytes_per_component_ < 1 || bytes_per_component_ > 2) return nullptr;

  return new vil1_iris_generic_image(is,imagename);
}

vil1_image_impl* vil1_iris_file_format::make_output_image(vil1_stream* is, int planes,
                                                          int width,
                                                          int height,
                                                          int components,
                                                          int bits_per_component,
                                                          vil1_component_format format)
{
  return new vil1_iris_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil1_iris_file_format::tag() const
{
  return vil1_iris_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil1_iris_generic_image::vil1_iris_generic_image(vil1_stream* is, char const* imagename):
  starttab_(nullptr), lengthtab_(nullptr), is_(is)
{
  is_->ref();
  read_header();
  std::strncpy(imagename_, imagename, 80);
}

bool vil1_iris_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==std::strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==std::strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

char const* vil1_iris_generic_image::file_format() const
{
  return vil1_iris_format_tag;
}

vil1_iris_generic_image::vil1_iris_generic_image(vil1_stream* is, int planes,
                                                 int width,
                                                 int height,
                                                 int components,
                                                 int bits_per_component,
                                                 vil1_component_format /*format*/):
  starttab_(nullptr), lengthtab_(nullptr), is_(is)
{
  is_->ref();

  if (bits_per_component == 8 ||
      bits_per_component == 16)
  {
    magic_  = 474;
    storage_ = 0;
    bytes_per_component_ = bits_per_component / 8;
    width_  = width;
    height_ = height;
    pixmin_ = 0;
    pixmax_ = (bits_per_component == 8) ? 255 : 65535;
    std::strcpy(imagename_, "vil1 writes an iris image!");
    colormap_ = 0;

    components_ = 1;
    planes_ = components * planes;
    if (planes_ == 1)
      dimension_ = 2;
    else if (planes_ == 3 || planes_ == 4)
      dimension_ = 3;
    else std::cerr << __FILE__ ": Cannot write iris image, they can only do grayscale or RGB(A)\n";
    write_header();
  }
  else std::cerr << __FILE__ ": Cannot write iris image, they want 8 or 16 bits per component\n";
}

vil1_iris_generic_image::~vil1_iris_generic_image()
{
  is_ -> unref();
  delete[] starttab_;
  delete[] lengthtab_;
}

bool vil1_iris_generic_image::read_header()
{
  is_->seek(0L);

  magic_     = get_short(is_, 0);
  storage_   = get_char(is_);
  bytes_per_component_ = get_char(is_);
  dimension_           = get_ushort(is_);
  width_     = get_ushort(is_);
  height_    = get_ushort(is_);
  planes_    = get_ushort(is_);
  pixmin_    = get_long(is_);
  pixmax_    = get_long(is_);
  components_ = 1;

  // DUMMY1 starts at 20
  //  starts at 24

  is_->seek(24L);
  is_->read(imagename_, 80L);

  // COLORMAP starts at 104
  colormap_ = get_long(is_);

  // _DUMMY2 starts at 108, ends at 512

  if (magic_ != 474)
  {
    std::cerr << __FILE__ ": This is not an Iris RGB file: magic number is incorrect: "
             << magic_ << std::endl;
    return false;
  }

  if (storage_ != 0 && storage_ != 1)
  {
    std::cerr << __FILE__ ": This is not an Iris RGB file: storage must be RLE or VERBATIM\n";
    return false;
  }

  if (colormap_ == 3)
  {
    std::cerr << __FILE__ ": This is not an ordinary Iris RGB image but a colormap file\n";
    return false;
  }

  if (dimension_ == 3 && colormap_ != 0)
  {
    std::cerr << __FILE__ ": Cannot handle Iris RGB file with colormap other than NORMAL\n";
    return false;
  }

  if (storage_)    // we got a RLE image
    read_offset_tables();

  return true;
}


bool vil1_iris_generic_image::write_header()
{
#ifdef DEBUG
  std::cerr << __FILE__ ": vil1_iris_generic_image::write_header()\n"
           << "Here we go :\n"
           << "magic_      = " << magic_    << std::endl
           << "storage_    = " << storage_ << std::endl
           << "bytes_per_c = " << bytes_per_component_ << std::endl
           << "dimension_  = " << dimension_ << std::endl
           << "width_      = " << width_ << std::endl
           << "height_     = " << height_ << std::endl
           << "planes_     = " << planes_ << std::endl
           << "pixmin_     = " << pixmin_ << std::endl
           << "pixmax_     = " << pixmax_ << std::endl
           << "colormap_   = " << colormap_ << std::endl
           << "components_ = " << components_ << std::endl
           << "imagename_  = " << imagename_ << std::endl
           << std::endl;
#endif

  char dummy[410];

  send_short(is_, magic_);
  send_char(is_, storage_); // either VERBATIM (0) or RLE (1)
  send_char(is_, bytes_per_component_);  // bytes per pixel per channel
  send_ushort(is_, dimension_); // either 1 (1 scanline), 2 (grey image), or 3 (colour)
  send_ushort(is_, width_);    // width
  send_ushort(is_, height_);    // height
  send_ushort(is_, planes_);    // nr of colour bands; typically 3 (RGB) or 4 (RGBA)
  send_long(is_, pixmin_);   // minimum pixel value; typically 0
  send_long(is_, pixmax_); // maximum pixel value; typically 255 if _PBC is 1
  is_->write(dummy, 4L);
  is_->write(imagename_, 80L); // null-terminated string
  send_long(is_, colormap_); // either NORMAL (0) (RGB), DITHERED (1) (R=3,G=3,B=2 bits),
                  // SCREEN (2) (obsolete) or COLORMAP (3) (hardware-specific).

  start_of_data_ = is_->tell();

  return is_->write(dummy, 404L) == 404L;
}


vil1_image vil1_iris_generic_image::get_plane(unsigned int plane) const
{
  assert((int)plane < planes_);
  std::cerr << __FILE__ ": do something for vil1_iris_generic_image::get_plane\n";
  return nullptr;
}


bool vil1_iris_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  // at the moment I am not dealing with requests for memory
  // outside the image so just abort if you get any
  assert(x0>=0);

  assert((x0+xs)<=width_);
  assert((y0+ys)<=height_);

  if (!buf) return false; // no storage location was given to us

  if (storage_)
    return get_section_rle(buf,x0,height_-y0-ys,xs,ys);
  else
    return get_section_verbatim(buf,x0,height_-y0-ys,xs,ys);
}


bool vil1_iris_generic_image::get_section_verbatim(void* ib, int x0, int y0, int xs, int ys) const
{
  int row_len = xs * bytes_per_component_;

  auto* dp = (unsigned char*)ib;

  for (int channel=0; channel<planes_; ++channel)
  {
    unsigned char* cbi = dp + row_len*ys*channel;
    // skip cbi to point at last row of section
    cbi+=(row_len*(ys-1));

    is_->seek(512L + channel * width_ * height_ + (y0 * width_) + x0);
                                        // actually: times cell size

    int skipbytes_end = width_ - xs; // bytes to skip, across consecutive rows

    // number of rows to read
    for (int row = 0; row < ys; ++row,cbi-=row_len)
    {
      // step to end of row and then to within next row, where next block begins
      if (row > 0) is_->seek(is_->tell() + skipbytes_end);
      is_->read(cbi, row_len);
    }
  }
  return true;
}//GetSectionVERBATIM


bool vil1_iris_generic_image::get_section_rle(void* ib, int x0, int y0, int xs, int ys) const
{
  int row_len = xs * bytes_per_component_;

  auto* dp = (unsigned char*)ib;
  auto* exrow = new unsigned char[width_];

  // for each channel
  for (int channel=0; channel<planes_; ++channel)
  {
    unsigned char* cbi = dp + row_len*ys*channel;
    // skip cbi to point at last row of section
    cbi+=row_len*(ys-1);

    // for each row
    for (int rowno=y0; rowno<y0+ys; ++rowno,cbi-=row_len)
    {
      // find length and start position
      unsigned long rleoffset =  starttab_[rowno+channel*height_];
      unsigned long rlelength = lengthtab_[rowno+channel*height_];

      // read rle row into array
      auto* rlerow = new unsigned char[rlelength];
      is_->seek(rleoffset);
      is_->read((void*)rlerow, rlelength);

      // decode rle row
      expandrow(exrow,rlerow,0);
      delete[] rlerow;

      // write expanded row in store
      std::memcpy(cbi,exrow+x0,xs);
    }
  }
  delete[] exrow;
  return true;
}


bool vil1_iris_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  int ynul = height_ - y0 - ys;

  int row_len = xs * bytes_per_component_;

  // for each channel
  for (int channel=0; channel<planes_; ++channel)
  {
    const unsigned char* cbi = (const unsigned char*)buf + row_len*ys*channel;
    // skip cbi to point at last row of section
    cbi+=row_len*(ys-1);

    // skip to start of section
    is_->seek(512 + channel * width_ * height_
              + ynul*width_*bytes_per_pixel() + x0*bytes_per_pixel());

    int skipbytes_end = width_ - xs;

    // number of rows to write
    for (int row = 0; row < ys; ++row,cbi-=row_len)
    {
      // step to end of row, and then to beginning of block on next row
      if (row > 0) is_->seek(is_->tell() + skipbytes_end);
      is_->write(cbi, row_len);
    }
  }
  return true;
}

bool vil1_iris_generic_image::read_offset_tables()
{
  int tablen;
  tablen = height_ * planes_;

  starttab_  = new unsigned long[tablen];
  lengthtab_ = new unsigned long[tablen];

  int i;
  for (i=0; i<tablen; ++i) {
    starttab_[i] = get_long(is_,512+(i*4));
  }

  int lengthtab_offset =  512 + tablen*4;

  for (i=0; i<tablen; ++i) {
    lengthtab_[i] = get_long(is_,lengthtab_offset+(i*4));
  }

  return true;
}


short get_short(vil1_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  unsigned char buff[2];
  file->read(buff, 2L);
  return static_cast<short>((buff[0]<<8)|(buff[1]));
}


char get_char(vil1_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  unsigned char buff[1];
  file->read((void*)buff, 1L);
  return buff[0];
}

unsigned short get_ushort(vil1_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  unsigned char buff[2];
  file->read((void*)buff, 2L);
  return static_cast<unsigned short>((buff[0]<<8)|(buff[1]));
}

long get_long(vil1_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  unsigned char buff[4];
  file->read((void*)buff, 4L);
  return (buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+(buff[3]<<0);
}


void send_char(vil1_stream* data, int s)
{
  char c = char(s);
  data->write(&c ,1L);
}

void send_short(vil1_stream* data, int s)
{
  unsigned char buff[2];
  buff[0] = static_cast<unsigned char>((s >> 8) & 0xff);
  buff[1] = static_cast<unsigned char>((s >> 0) & 0xff);
  data->write(buff, 2L);
}

void send_ushort(vil1_stream* data, unsigned int s)
{
  unsigned char buff[2];
  buff[0] = static_cast<unsigned char>((s >> 8) & 0xff);
  buff[1] = static_cast<unsigned char>(s & 0xff);
  data->write(buff, 2L);
}

void send_long(vil1_stream* data, long s)
{
  unsigned char buff[4];
  buff[0] = (unsigned char)((s >> 24) & 0xff);
  buff[1] = (unsigned char)((s >> 16) & 0xff);
  buff[2] = (unsigned char)((s >>  8) & 0xff);
  buff[3] = (unsigned char)((s >>  0) & 0xff);
  data->write(buff, 4L);
}

void expandrow(unsigned char *optr, unsigned char *iptr, int z)
{
  unsigned char pixel, count;

  optr += z;
  while (true)
  {
    pixel = *iptr++;
    if ( !(count = static_cast<unsigned char>(pixel & 0x7f)) )
      return;
    if (pixel & 0x80)
    {
      while (count--) { *optr = *iptr++; ++optr; }
    }
    else
    {
      pixel = *iptr++;
      while (count--) { *optr = pixel; ++optr; }
    }
  }
}
