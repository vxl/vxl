#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_pnm.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sprintf

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

char const* vil_pnm_format_tag = "pnm";

static inline bool iseol(int c)
{
  return c == 10 || c == 13;
}

static inline bool isws(int c)
{
  return c == ' ' || c == '\t' || c == 10 || c == 13;
}

vil_image_impl* vil_pnm_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  unsigned char buf[3];
  vs->read(buf, 3);
  bool ok = ((buf[0] == 'P') &&
             iseol(buf[2]) &&
             (buf[1] >= '1' && buf[2] <= '6'));
  if (!ok)
    return 0;

  return new vil_pnm_generic_image(vs);
}

vil_image_impl* vil_pnm_file_format::make_output_image(vil_stream* vs, int planes,
                                                       int width,
                                                       int height,
                                                       int components,
                                                       int bits_per_component,
                                                       vil_component_format format)
{
  return new vil_pnm_generic_image(vs, planes, width, height, components, bits_per_component, format);
}

char const* vil_pnm_file_format::tag() const
{
  return vil_pnm_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_pnm_generic_image::vil_pnm_generic_image(vil_stream* vs):
  vs_(vs)
{
  vs_->ref();
  read_header();
}

char const* vil_pnm_generic_image::file_format() const
{
  return vil_pnm_format_tag;
}

vil_pnm_generic_image::vil_pnm_generic_image(vil_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil_component_format format):
  vs_(vs)
{
  vs_->ref();
  width_ = width;
  height_ = height;

  components_ = components * planes;
  bits_per_component_ = bits_per_component;

  if (components_ == 3) {
    magic_ = 6;
  } else if (components_ == 1) {
    if (bits_per_component_ == 1)
      magic_ = 4;
    else
      magic_ = 5;
  }
  if (bits_per_component_ > 8) magic_ -= 3;

  if (bits_per_component_ <= 8) {
    maxval_ = 0xFF;
    bits_per_component_ = 8;
  } else if (bits_per_component_ <= 16) {
    maxval_ = 0xFFFF;
    bits_per_component_ = 16;
  } else if (bits_per_component_ <= 24) {
    maxval_ = 0xFFFFFF;
    bits_per_component_ = 24;
  } else if (bits_per_component_ <= 32) {
    maxval_ = 0x7FFFFFFF; // not 0xFFFFFFFF as the pnm format does not allow values > MAX_INT
  } else {
    vcl_cerr << "vil_pnm_generic_image: cannot make  " << bits_per_component_ << " bit x " << components_ << " image\n";
  }

  write_header();
}

vil_pnm_generic_image::~vil_pnm_generic_image()
{
  //delete vs_;
  vs_->unref();
}

// Skip over spaces and comments; temp is the current vs character
static void SkipSpaces(vil_stream* vs, signed char& temp)
{
  while (isws(temp) || temp == '#')
  {
    if (temp == '#') // skip this line:
      while ((temp != '\n') && (temp != -1))
        vs->read(&temp,1);
    // skip this `whitespace' byte:
    vs->read(&temp,1);
  }
}

// Get an integer from the vs stream; temp is the current vs character
static int ReadInteger(vil_stream* vs, signed char& temp)
{
  int n = 0;
  while ((temp >= '0') && (temp <= '9'))
  {
    n *= 10; n += (temp - '0');
    vs->read(&temp,1);
  }
  return n;
}

bool vil_pnm_generic_image::read_header()
{
  signed char temp;

//This method assumes the PGM header is in the following format
//P5
//#comments
//#up to any amount of comments (including no comments)
//width height
//maxval

  // Go to start
  vs_->seek(0);

  char buf[3];
  vs_->read(buf, 3);
  if (buf[0] != 'P') return false;
  if (!iseol(buf[2])) return false;
  magic_ = buf[1] - '0';
  if (magic_ < 1 || magic_ > 6) return false;

  // read 1 byte
  vs_->read(&temp, 1);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Width
  width_ = ReadInteger(vs_,temp);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Height
  height_ = ReadInteger(vs_,temp);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Maxval
  maxval_ = ReadInteger(vs_,temp);

  //Skip over final end-of-line, before the data section begins
  if (isws(temp))
    vs_->read(&temp,1);

  start_of_data_ = vs_->tell() - 1;

  components_ = ((magic_ == 3 || magic_ == 6) ? 3 : 1);

  if (magic_ == 1 || magic_ == 4)
    bits_per_component_ = 1;
  else {
    if (maxval_ == 0) assert(!"indentation is everything");
    else if (maxval_ <= 0xFF) bits_per_component_ = 8;
    else if (maxval_ <= 0xFFFF) bits_per_component_ = 16;
    else if (maxval_ <= 0xFFFFFF) bits_per_component_ = 24;
    else if (maxval_ <= 0xFFFFFFFF) bits_per_component_ = 32;
    else assert(!"vil_pnm_generic_image: too big");
  }

  return true;
}

bool vil_pnm_generic_image::write_header()
{
  vs_->seek(0);

  char buf[1024];
  //sprintf(buf, "P%d\n# VIL pnm image\n%u %u\n%lu\n",
  //        magic_, width_, height_, maxval_);
  // The comment does not add any useful information.
  vcl_sprintf(buf, "P%d\n%u %u\n%lu\n",
          magic_, width_, height_, maxval_);
  vs_->write(buf, vcl_strlen(buf));
  start_of_data_ = vs_->tell();
  return true;
}

bool operator>>(vil_stream& vs, int& a)
{
  char c; vs.read(&c,1);
  while (c == '#' || c == ' ' || c == '\t' || c == '\n') {
    if (c == '#') while (c != '\n') { vs.read(&c,1); continue; }
    vs.read(&c,1);
  }
  if (c < '0' || c > '9') return false; // non-digit found
  a = 0;
  while (c >= '0' && c <= '9') { a*=10; a+=(c-'0'); vs.read(&c,1); }
  return true;
}

bool vil_pnm_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (bits_per_component_ < 8) {
    vcl_cerr << "vil_pnm_generic_image: cannot load " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }

  int bytes_per_pixel = components_ * (bits_per_component_/8);

  int byte_start = start_of_data_ + (y0 * width_ + x0) * bytes_per_pixel;
  int byte_width = width_ * bytes_per_pixel;

  int byte_out_width = xs * bytes_per_pixel;

  unsigned char* ib = (unsigned char*) buf;
  unsigned short* jb = (unsigned short*) buf;
  //
  if (magic_ > 3) {
    for(int y = 0; y < ys; ++y) {
      vs_->seek(byte_start + y * byte_width);
      vs_->read(ib + y * byte_out_width, byte_out_width);
    }
  } else {
    vs_->seek(start_of_data_);
    for(int t = 0; t < y0*width_*components_; ++t) { int a; (*vs_) >> a; }
    for(int y = 0; y < ys; ++y) {
      for(int t = 0; t < x0*components_; ++t) { int a; (*vs_) >> a; }
      if (bits_per_component_ <= 8)
        for(int x = 0; x < xs*components_; ++x) { int a; (*vs_) >> a; ib[x0+x]=a; }
      else if (bits_per_component_ <= 16)
        for(int x = 0; x < xs*components_; ++x) { int a; (*vs_) >> a; jb[x0+x]=a; }
      else
        for(int x = 0; x < xs*components_; ++x) {
          int a; (*vs_) >> a;
          ib[3*(x0+x)]=(a>>16); ib[3*(x0+x)+1]=(a>>8); ib[3*(x0+x)+2]=a;
        }
      ib += xs; jb += xs; if (bits_per_component_>16) ib += 2*xs;
    }
    for(int t = 0; t < (width_-x0-xs)*components_; ++t) { int a; (*vs_) >> a; }
  }

  return true;
}

void operator<<(vil_stream& vs, int a) {
  char buf[128]; vcl_sprintf(buf, " %d\n", a); vs.write(buf,vcl_strlen(buf));
}

bool vil_pnm_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  if (bits_per_component_ < 8) {
    vcl_cerr << "vil_pnm_generic_image: cannot save " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }

  int bytes_per_pixel = components_ * (bits_per_component_/8);

  int byte_start = start_of_data_ + (y0 * width_ + x0) * bytes_per_pixel;
  int byte_width = width_ * bytes_per_pixel;

  int byte_out_width = xs * bytes_per_pixel;

  unsigned char const* ob = (unsigned char const*) buf;
  unsigned short const* pb = (unsigned short const*) buf;
  if (magic_ > 3)
    for(int y = 0; y < ys; ++y) {
      vs_->seek(byte_start + y * byte_width);
      vs_->write(ob + y * byte_out_width, byte_out_width);
    }
  else {
    if (x0 > 0 || y0 > 0 || xs < width_)
      return false; // can only write the full image in this mode
    vs_->seek(start_of_data_);
    for(int y = 0; y < ys; ++y) {
      if (bits_per_component_ <= 8)
        for(int x = 0; x < xs*components_; ++x) { (*vs_) << ob[x]; }
      else if (bits_per_component_ <= 16)
        for(int x = 0; x < xs*components_; ++x) { (*vs_) << pb[x]; }
      else
        for(int x = 0; x < xs*components_; ++x) {
          int a=(ob[3*(x0+x)]<<16)|(ob[3*(x0+x)+1]<<8)|(ob[3*(x0+x)+2]);
          (*vs_) << a;
        }
      ob += xs; pb += xs; if (bits_per_component_>16) ob += 2*xs;
    }
  }

  return true;
}

vil_image vil_pnm_generic_image::get_plane(int plane) const
{
  assert(plane == 0);
  return const_cast<vil_pnm_generic_image*>(this);
}
