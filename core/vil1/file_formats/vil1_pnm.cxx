#ifdef __GNUC__
#pragma implementation "vil_pnm.h"
#endif

#include "vil_pnm.h"

#include <assert.h>
#include <stdio.h> // for sprintf

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_generic_image.h>

char const* vil_pnm_format_tag = "pnm";

vil_generic_image* vil_pnm_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  unsigned char buf[3];
  is->read(buf, 3);
  bool ok = ((buf[0] == 'P') &&
	     (buf[2] == '\n') &&
	     (buf[1] >= '1' && buf[2] <= '6'));
  if (!ok)
    return 0;

  return new vil_pnm_generic_image(is);
}

vil_generic_image* vil_pnm_file_format::make_output_image(vil_stream* is, vil_generic_image const* prototype)
{
  return new vil_pnm_generic_image(is, prototype);
}

char const* vil_pnm_file_format::tag() const
{
  return vil_pnm_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_pnm_generic_image::vil_pnm_generic_image(vil_stream* is):
  is_(is)
{
  read_header();
}

char const* vil_pnm_generic_image::file_format() const
{
  return vil_pnm_format_tag;
}

vil_pnm_generic_image::vil_pnm_generic_image(vil_stream* is, vil_generic_image const* prototype):
  is_(is)
{
  width_ = prototype->width();
  height_ = prototype->height();
  
  components_ = prototype->components() * prototype->planes();
  bits_per_component_ = prototype->bits_per_component();

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
    maxval_ = (1 << 8) - 1;
    bits_per_component_ = 8;
  } else if (bits_per_component_ <= 16) {
    maxval_ = (1 << 16) - 1;
    bits_per_component_ = 16;
  } else if (bits_per_component_ <= 24) {
    maxval_ = (1 << 24) - 1;
    bits_per_component_ = 24;
  } else { 
    cerr << "vil_pnm_generic_image: cannot make  " << bits_per_component_ << " bit x " << components_ << " image\n";
  }

  write_header();
}

// Skip over spaces and comments; temp is the current file character
static void SkipSpaces(vil_stream* file, signed char& temp)
{
  while (temp == ' ' || temp == '\t' || temp == '\n' || temp == '#')
  {
    if (temp == '#') // skip this line:
      while ((temp != '\n') && (temp != -1))
	file->read(&temp,1);
    // skip this `whitespace' byte:
    file->read(&temp,1);
  }
}

// Get an integer from the file stream; temp is the current file character
static int ReadInteger(vil_stream* file, signed char& temp)
{
  int n = 0;
  while ((temp >= '0') && (temp <= '9'))
  {
    n *= 10; n += (temp - '0');
    file->read(&temp,1);
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
  is_->seek(0);

  char buf[3];
  is_->read(buf, 3);
  if (buf[0] != 'P') return false;
  if (buf[2] != '\n') return false;
  magic_ = buf[1] - '0';
  if (magic_ < 1 || magic_ > 6) return false;

  // read 1 byte
  is_->read(&temp, 1);

  //Skip over spaces and comments
  SkipSpaces(is_,temp);

  //Read in Width
  width_ = ReadInteger(is_,temp);

  //Skip over spaces and comments
  SkipSpaces(is_,temp);

  //Read in Height
  height_ = ReadInteger(is_,temp);
  
  //Skip over spaces and comments
  SkipSpaces(is_,temp);

  //Read in Maxval
  maxval_ = ReadInteger(is_,temp);
  
  //Skip over final end-of-line, before the data section begins
  if (temp == ' ' || temp == '\t' || temp == '\n')
    is_->read(&temp,1);

  start_of_data_ = is_->tell() - 1;

  components_ = ((magic_ == 3 || magic_ == 6) ? 3 : 1);

  if (magic_ == 1 || magic_ == 4)
    bits_per_component_ = 1;
  else {
    assert(maxval_ > 0);
    if (maxval_ < (1<<8)) bits_per_component_ = 8;
    else if (maxval_ < (1<<16)) bits_per_component_ = 16;
    else if (maxval_ < (1<<24)) bits_per_component_ = 24;
    else assert(!"vil_pnm_generic_image: too big");
  }
  
  return true;
}

bool vil_pnm_generic_image::write_header()
{
  is_->seek(0);

  char buf[1024];
  sprintf(buf, "P%d\n# VIL pnm image\n%d %d\n%d\n",
          magic_, width_, height_, maxval_);
  is_->write(buf, strlen(buf));
  start_of_data_ = is_->tell();
  return true;
}

bool operator>>(vil_stream& is, int& a) {
  char c; is.read(&c,1);
  while (c == '#' || c == ' ' || c == '\t' || c == '\n') {
    if (c == '#') while (c != '\n') { is.read(&c,1); continue; }
    is.read(&c,1);
  }
  if (c < '0' || c > '9') return false; // non-digit found
  a = 0;
  while (c >= '0' && c <= '9') { a*=10; a+=(c-'0'); is.read(&c,1); }
  return true;
}

bool vil_pnm_generic_image::do_get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (bits_per_component_ < 8) {
    cerr << "vil_pnm_generic_image: cannot load " << bits_per_component_ << " bit x " << components_ << " image\n";
    return false;
  }

  int bytes_per_pixel = components_ * (bits_per_component_/8);

  int byte_start = start_of_data_ + (y0 * width_ + x0) * bytes_per_pixel;
  int byte_width = width_ * bytes_per_pixel;

  int byte_out_width = xs * bytes_per_pixel;
  
  unsigned char* ib = (unsigned char*) buf;
  unsigned short* jb = (unsigned short*) buf;
  if (magic_ > 3)
    for(int y = 0; y < ys; ++y) {
      is_->seek(byte_start + y * byte_width);
      is_->read(ib + y * byte_out_width, byte_out_width);
    }
  else {
    is_->seek(start_of_data_);
    for(int t = 0; t < y0*width_*components_; ++t) { int a; (*is_) >> a; }
    for(int y = 0; y < ys; ++y) {
      for(int t = 0; t < x0*components_; ++t) { int a; (*is_) >> a; }
      if (bits_per_component_ <= 8)
        for(int x = 0; x < xs*components_; ++x) { int a; (*is_) >> a; ib[x0+x]=a; }
      else if (bits_per_component_ <= 16)
        for(int x = 0; x < xs*components_; ++x) { int a; (*is_) >> a; jb[x0+x]=a; }
      else
        for(int x = 0; x < xs*components_; ++x) {
          int a; (*is_) >> a;
          ib[3*(x0+x)]=(a>>16); ib[3*(x0+x)+1]=(a>>8); ib[3*(x0+x)+2]=a;
	}
      ib += xs; jb += xs; if (bits_per_component_>16) ib += 2*xs;
    }
    for(int t = 0; t < (width_-x0-xs)*components_; ++t) { int a; (*is_) >> a; }
  }

  return true;
}

void operator<<(vil_stream& is, int a) {
  char buf[128]; sprintf(buf, " %d\n", a); is.write(buf,strlen(buf));
}

bool vil_pnm_generic_image::do_put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  if (bits_per_component_ < 8) {
    cerr << "vil_pnm_generic_image: cannot save " << bits_per_component_ << " bit x " << components_ << " image\n";
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
      is_->seek(byte_start + y * byte_width);
      is_->write(ob + y * byte_out_width, byte_out_width);
    }
  else {
    if (x0 > 0 || y0 > 0 || xs < width_)
      return false; // can only write the full image in this mode
    is_->seek(start_of_data_);
    for(int y = 0; y < ys; ++y) {
      if (bits_per_component_ <= 8)
        for(int x = 0; x < xs*components_; ++x) { (*is_) << ob[x]; }
      else if (bits_per_component_ <= 16)
        for(int x = 0; x < xs*components_; ++x) { (*is_) << pb[x]; }
      else
        for(int x = 0; x < xs*components_; ++x) {
          int a=(ob[3*(x0+x)]<<16)|(ob[3*(x0+x)+1]<<8)|(ob[3*(x0+x)+2]);
          (*is_) << a;
        }
      ob += xs; pb += xs; if (bits_per_component_>16) ob += 2*xs;
    }
  }

  return true;
}

vil_generic_image* vil_pnm_generic_image::get_plane(int plane) const 
{
  assert(plane == 0);
  return const_cast<vil_pnm_generic_image*>(this);
}
