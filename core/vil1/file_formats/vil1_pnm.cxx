// This is core/vil1/file_formats/vil1_pnm.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_pnm.h"

#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

#include <vxl_config.h>
#undef sprintf // This works around a bug in libintl.h
#include <vcl_cstdio.h> // for sprintf

char const* vil1_pnm_format_tag = "pnm";

static inline bool iseol(int c)
{
  return c == 10 || c == 13;
}

static inline bool isws(int c)
{
  return c == ' ' || c == '\t' || c == 10 || c == 13;
}

vil1_image_impl* vil1_pnm_file_format::make_input_image(vil1_stream* vs)
{
  // Attempt to read header
  unsigned char buf[3];
  vs->read(buf, 3L);
  bool ok = ((buf[0] == 'P') &&
             iseol(buf[2]) &&
             (buf[1] >= '1' && buf[2] <= '6'));
  if (!ok)
    return 0;

  return new vil1_pnm_generic_image(vs);
}

vil1_image_impl* vil1_pnm_file_format::make_output_image(vil1_stream* vs, int planes,
                                                         int width,
                                                         int height,
                                                         int components,
                                                         int bits_per_component,
                                                         vil1_component_format format)
{
  return new vil1_pnm_generic_image(vs, planes, width, height, components, bits_per_component, format);
}

char const* vil1_pnm_file_format::tag() const
{
  return vil1_pnm_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil1_pnm_generic_image::vil1_pnm_generic_image(vil1_stream* vs):
  vs_(vs)
{
  vs_->ref();
  read_header();
}

bool vil1_pnm_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

char const* vil1_pnm_generic_image::file_format() const
{
  return vil1_pnm_format_tag;
}

vil1_pnm_generic_image::vil1_pnm_generic_image(vil1_stream* vs, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil1_component_format ):
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
  // from August 2000, pnm allows 16 bit samples in rawbits format, stored MSB.
  if (bits_per_component_ > 16) magic_ -= 3;

  if (bits_per_component_ < 31)
    maxval_ = (1L<<bits_per_component_)-1;
  else
    maxval_ = 0x7FFFFFFF; // not 0xFFFFFFFF as the pnm format does not allow values > MAX_INT

  write_header();
}

vil1_pnm_generic_image::~vil1_pnm_generic_image()
{
  //delete vs_;
  vs_->unref();
}

// Skip over spaces and comments; temp is the current vs character
static void SkipSpaces(vil1_stream* vs, char& temp)
{
  while (isws(temp) || temp == '#')
  {
    if (temp == '#') // skip this line:
      while (!iseol(temp))
        if (1L > vs->read(&temp,1L)) return; // at end-of-file?
    // skip this `whitespace' byte by reading the next byte:
    if (1L > vs->read(&temp,1L)) return;
  }
}

// Get a nonnegative integer from the vs stream; temp is the current vs byte
static int ReadInteger(vil1_stream* vs, char& temp)
{
  int n = 0;
  while ((temp >= '0') && (temp <= '9'))
  {
    n *= 10; n += (temp - '0');
    if (1L > vs->read(&temp,1L)) return n; // at end-of-file?
  }
  return n;
}

// Convert the buffer of 16 bit words from MSB to host order
static void ConvertMSBToHost( void* buf, int num_words )
{
#if VXL_LITTLE_ENDIAN
  unsigned char* ptr = (unsigned char*)buf;
  for ( int i=0; i < num_words; ++i ) {
    unsigned char t = *ptr;
    *ptr = *(ptr+1);
    *(ptr+1) = t;
    ptr += 2;
  }
#endif
}

// Convert the buffer of 16 bit words from host order to MSB
static void ConvertHostToMSB( void* buf, int num_words )
{
#if VXL_LITTLE_ENDIAN
  unsigned char* ptr = (unsigned char*)buf;
  for ( int i=0; i < num_words; ++i ) {
    unsigned char t = *ptr;
    *ptr = *(ptr+1);
    *(ptr+1) = t;
    ptr += 2;
  }
#endif
}


//: This method accepts any valid PNM file (first 3 bytes "P1\n" to "P6\n")
bool vil1_pnm_generic_image::read_header()
{
  char temp;

  // Go to start of file
  vs_->seek(0L);

  char buf[3];
  if (3L > vs_->read(buf, 3L)) return false; // at end-of-file?
  if (buf[0] != 'P') return false;
  if (!iseol(buf[2])) return false;
  magic_ = buf[1] - '0';
  if (magic_ < 1 || magic_ > 6) return false;

  // read 1 byte
  vs_->read(&temp, 1L);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Width
  width_ = ReadInteger(vs_,temp);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Height
  height_ = ReadInteger(vs_,temp);

  // a pbm (bitmap) image does not have a maxval field
  if (magic_ == 1 || magic_ == 4)
    maxval_ = 1;
  else
  {
    //Skip over spaces and comments
    SkipSpaces(vs_,temp);

    //Read in Maxval
    maxval_ = ReadInteger(vs_,temp);
  }

  start_of_data_ = vs_->tell() - 1L;

  //Final end-of-line or other white space (1 byte) before the data section begins
  if (isws(temp))
    ++start_of_data_;

  components_ = ((magic_ == 3 || magic_ == 6) ? 3 : 1);

  if (magic_ == 1 || magic_ == 4)
    bits_per_component_ = 1;
  else {
    if (maxval_ == 0) assert(!"indentation is everything");
    else if (maxval_ <= 0xFF) bits_per_component_ = 8;
    else if (maxval_ <= 0xFFFF) bits_per_component_ = 16;
    else if (maxval_ <= 0xFFFFFF) bits_per_component_ = 24;
    else if (maxval_ <= 0x7FFFFFFF) bits_per_component_ = 32;
    else assert(!"vil1_pnm_generic_image: maxval is too big");
  }

  return true;
}

bool vil1_pnm_generic_image::write_header()
{
  vs_->seek(0L);

  char buf[1024];
  vcl_sprintf(buf, "P%d\n#vil1 pnm image, #c=%u, bpc=%u\n%u %u\n",
              magic_, components_, bits_per_component_, width_, height_);
  vs_->write(buf, vcl_strlen(buf));
  if (magic_ != 1 && magic_ != 4)
  {
    vcl_sprintf(buf, "%lu\n", maxval_);
    vs_->write(buf, vcl_strlen(buf));
  }
  start_of_data_ = vs_->tell();
  return true;
}

bool operator>>(vil1_stream& vs, int& a)
{
  char c; vs.read(&c,1L);
  SkipSpaces(&vs,c);
  if (c < '0' || c > '9') return false; // non-digit found
  a = ReadInteger(&vs,c);
  return true;
}

bool vil1_pnm_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  unsigned char* ib = (unsigned char*) buf;
  unsigned short* jb = (unsigned short*) buf;
  unsigned int* kb = (unsigned int*) buf;
  //
  if (magic_ > 4) // pgm or ppm raw image
  {
    int bytes_per_sample = (bits_per_component_+7)/8;
    int bytes_per_pixel = components_ * bytes_per_sample;
    int byte_start = start_of_data_ + (y0 * width_ + x0) * bytes_per_pixel;
    int byte_width = width_ * bytes_per_pixel;
    int byte_out_width = xs * bytes_per_pixel;

    for (int y = 0; y < ys; ++y) {
      vs_->seek(byte_start + y * byte_width);
      vs_->read(ib + y * byte_out_width, byte_out_width);
    }
    if ( bytes_per_sample==2 && VXL_LITTLE_ENDIAN ) {
      ConvertMSBToHost( buf, xs*ys*components_ );
    } else if ( bytes_per_sample > 2 ) {
      vcl_cerr << "ERROR: pnm: reading rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 4) // pbm (bitmap) raw image
  {
    int byte_width = (width_+7)/8;
    int byte_out_width = (xs+7)/8;

    for (int y = 0; y < ys; ++y) {
      vil1_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      vs_->seek(byte_start);
      unsigned char a; vs_->read(&a, 1L);
      int s = x0&7; // = x0%8;
      unsigned char b = 0; // output
      int t = 0;
      for (int x = 0; x < xs; ++x) {
        b |= ((a>>(7-s))&1)<<(7-t); // single bit; high bit = first
        if (s >= 7) { vs_->read(&a, 1L); s = 0; }
        else ++s;
        if (t >= 7) { ib[y * byte_out_width + x/8] = b; b = 0; t = 0; }
        else ++t;
      }
      if (t) ib[y * byte_out_width + (xs-1)/8] = b;
    }
  }
  else // ascii (non-raw) image data
  {
    vs_->seek(start_of_data_);
    //0. Skip to the starting line
    //
    for (int t = 0; t < y0*width_*components_; ++t) { int a; (*vs_) >> a; }
    for (int y = 0; y < ys; ++y) {
      // 1. Skip to column x0
      //
      for (int t = 0; t < x0*components_; ++t) { int a; (*vs_) >> a; }
      // 2. Read the data
      //
      if (bits_per_component_ <= 1) {
        --ib; // to compensate for first ++ib
        for (int x=0,t=0; x<xs*components_; ++x,++t) {
          if ((t&=7)==0) *++ib=0; int a; (*vs_) >> a; if (a) *ib|=(1<<(7-t)); }
        ++ib;
      }
      else if (bits_per_component_ <= 8)
        for (int x = 0; x < xs*components_; ++x) { int a; (*vs_) >> a; *(ib++)=a; }
      else if (bits_per_component_ <= 16)
        for (int x = 0; x < xs*components_; ++x) { int a; (*vs_) >> a; *(jb++)=a; }
      else
        for (int x = 0; x < xs*components_; ++x) { int a; (*vs_) >> a; *(kb++)=a; }
      // 3. Skip to the next line
      //
      for (int t = 0; t < (width_-x0-xs)*components_; ++t) { int a; (*vs_) >> a; }
    }
  }

  return true;
}

void operator<<(vil1_stream& vs, int a) {
  char buf[128]; vcl_sprintf(buf, " %d\n", a); vs.write(buf,vcl_strlen(buf));
}

bool vil1_pnm_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  unsigned char const* ob = (unsigned char const*) buf;
  unsigned short const* pb = (unsigned short const*) buf;
  unsigned int const* qb = (unsigned int const*) buf;

  if (magic_ > 4) // pgm or ppm raw image
  {
    int bytes_per_sample = (bits_per_component_+7)/8;
    int bytes_per_pixel = components_ * bytes_per_sample;
    vil1_streampos byte_start = start_of_data_ + (y0 * width_ + x0) * bytes_per_pixel;
    int byte_width = width_ * bytes_per_pixel;
    int byte_out_width = xs * bytes_per_pixel;

    if ( bytes_per_sample==1 || ( bytes_per_sample==2 && VXL_BIG_ENDIAN ) ) {
      for (int y = 0; y < ys; ++y) {
        vs_->seek(byte_start + y * byte_width);
        vs_->write(ob + y * byte_out_width, byte_out_width);
      }
    } else if ( bytes_per_sample==2 ) {
      // Little endian host; must convert words to have MSB first.
      // Can't convert the input buffer, because it's not ours.
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<unsigned char> tempbuf( byte_out_width );
      for (int y = 0; y < ys; ++y) {
        vs_->seek(byte_start + y * byte_width);
        vcl_memcpy( &tempbuf[0], ob + y * byte_out_width, byte_out_width );
        ConvertHostToMSB( &tempbuf[0], xs*components_ );
        vs_->write(&tempbuf[0], byte_out_width);
      }
    } else {
      vcl_cerr << "ERROR: pnm: writing rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 4) // pbm (bitmap) raw image
  {
    int byte_width = (width_+7)/8;
    int byte_out_width = (xs+7)/8;

    for (int y = 0; y < ys; ++y) {
      vil1_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      vs_->seek(byte_start);
      int s = x0&7; // = x0%8;
      int t = 0;
      unsigned char a = 0, b = ob[y * byte_out_width];
      if (s) {
        vs_->read(&a, 1L);
        vs_->seek(byte_start);
        a &= ((1<<s)-1)<<(8-s); // clear the last 8-s bits of a
      }
      for (int x = 0; x < xs; ++x) {
        if (b&(1<<(7-t))) a |= 1<<(7-s); // single bit; high bit = first
        if (t >= 7) { b = ob[y * byte_out_width + (x+1)/8]; t = 0; }
        else ++t;
        if (s >= 7) { vs_->write(&a, 1L); ++byte_start; s = 0; a = 0; }
        else ++s;
      }
      if (s) {
        if (x0+xs < width_) {
          vs_->seek(byte_start);
          unsigned char c; vs_->read(&c, 1L);
          vs_->seek(byte_start);
          c &= ((1<<(8-s))-1); // clear the first s bits of c
          a |= c;
        }
        vs_->write(&a, 1L);
      }
    }
  }
  else // ascii (non-raw) image data
  {
    if (x0 > 0 || y0 > 0 || xs < width_)
      return false; // can only write the full image in this mode
    vs_->seek(start_of_data_);
    for (int y = 0; y < ys; ++y) {
      if (bits_per_component_ <= 1)
        for (int x = 0; x < xs*components_; ++x) { (*vs_) << ((ob[x/8]>>(7-x&7))&1); }
      else if (bits_per_component_ <= 8)
        for (int x = 0; x < xs*components_; ++x) { (*vs_) << ob[x]; }
      else if (bits_per_component_ <= 16)
        for (int x = 0; x < xs*components_; ++x) { (*vs_) << pb[x]; }
      else
        for (int x = 0; x < xs*components_; ++x) { (*vs_) << qb[x]; }
      ob += xs; pb += xs; qb += xs;
    }
  }

  return true;
}

vil1_image vil1_pnm_generic_image::get_plane(unsigned int plane) const
{
  assert(plane == 0);
  return const_cast<vil1_pnm_generic_image*>(this);
}
