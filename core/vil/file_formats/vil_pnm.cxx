// This is mul/vil2/file_formats/vil2_pnm.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil2_pnm.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sprintf
#include <vcl_vector.h>

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_property.h>
#include <vil2/vil2_image_data.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_memory_chunk.h>

#include <vxl_config.h>

char const* vil2_pnm_format_tag = "pnm";

static inline bool iseol(int c)
{
  return c == 10 || c == 13;
}

static inline bool isws(int c)
{
  return c == ' ' || c == '\t' || c == 10 || c == 13;
}

vil2_image_data_sptr vil2_pnm_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  unsigned char buf[3];
  vs->read(buf, 3L);
  bool ok = ((buf[0] == 'P') &&
             iseol(buf[2]) &&
             (buf[1] >= '1' && buf[2] <= '6'));
  if (!ok)
    return 0;

  return new vil2_pnm_image(vs);
}

vil2_image_data_sptr vil2_pnm_file_format::make_output_image(vil_stream* vs, 
                                                       unsigned nx,
                                                       unsigned ny,
                                                       unsigned nplanes,
                                                       unsigned bits_per_component,
                                                       vil_component_format format)
{
  return new vil2_pnm_image(vs, nx, ny, nplanes, bits_per_component, format);
}

char const* vil2_pnm_file_format::tag() const
{
  return vil2_pnm_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil2_pnm_image::vil2_pnm_image(vil_stream* vs):
  vs_(vs)
{
  vs_->ref();
  read_header();
}

bool vil2_pnm_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

char const* vil2_pnm_image::file_format() const
{
  return vil2_pnm_format_tag;
}

vil2_pnm_image::vil2_pnm_image(vil_stream* vs, unsigned nplanes,
                                             unsigned nx,
                                             unsigned ny,
                                             unsigned bits_per_component,
                                             vil_component_format format):
  vs_(vs)
{
  vs_->ref();
  nx_ = nx;
  ny_ = ny;

  ncomponents_ = nplanes;
  bits_per_component_ = bits_per_component;

  if (ncomponents_ == 3) {
    magic_ = 6;
  } else if (ncomponents_ == 1) {
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

vil2_pnm_image::~vil2_pnm_image()
{
  //delete vs_;
  vs_->unref();
}

// Skip over spaces and comments; temp is the current vs character
static void SkipSpaces(vil_stream* vs, char& temp)
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
static int ReadInteger(vil_stream* vs, char& temp)
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
bool vil2_pnm_image::read_header()
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
  nx_ = ReadInteger(vs_,temp);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Height
  ny_ = ReadInteger(vs_,temp);

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

  ncomponents_ = ((magic_ == 3 || magic_ == 6) ? 3 : 1);

  if (magic_ == 1 || magic_ == 4)
    bits_per_component_ = 1;
  else {
    if (maxval_ == 0) assert(!"indentation is everything");
    else if (maxval_ <= 0xFF) bits_per_component_ = 8;
    else if (maxval_ <= 0xFFFF) bits_per_component_ = 16;
    else if (maxval_ <= 0xFFFFFF) bits_per_component_ = 24;
    else if (maxval_ <= 0x7FFFFFFF) bits_per_component_ = 32;
    else assert(!"vil2_pnm_image: maxval is too big");
  }

  return true;
}

bool vil2_pnm_image::write_header()
{
  vs_->seek(0L);

  char buf[1024];
  vcl_sprintf(buf, "P%d\n#vil2 pnm image, #c=%u, bpc=%u\n%u %u\n",
              magic_, ncomponents_, bits_per_component_, nx_, ny_);
  vs_->write(buf, vcl_strlen(buf));
  if (magic_ != 1 && magic_ != 4)
  {
    vcl_sprintf(buf, "%lu\n", maxval_);
    vs_->write(buf, vcl_strlen(buf));
  }
  start_of_data_ = vs_->tell();
  return true;
}

bool operator>>(vil_stream& vs, int& a)
{
  char c; vs.read(&c,1L);
  SkipSpaces(&vs,c);
  if (c < '0' || c > '9') return false; // non-digit found
  a = ReadInteger(&vs,c);
  return true;
}

vil2_image_view_base* vil2_pnm_image::get_view(
  unsigned x0, unsigned y0, unsigned plane0,
  unsigned nx, unsigned ny, unsigned nplanes) const
{
  bool* bb = 0;
  unsigned char* ib = 0;
  unsigned short* jb = 0;
  unsigned int* kb = 0;

  vil2_memory_chunk_sptr buf;
  
  if (bits_per_component_ == 1)
  {
    buf = new vil2_memory_chunk(nx_ * ny_* ncomponents_ * sizeof(bool));
    bb = (bool *) buf->data();
  }
  else if (bits_per_component_ <= 8)
  {
    buf = new vil2_memory_chunk(nx_ * ny_* ncomponents_ * sizeof(unsigned char));
    ib = (unsigned char *) buf->data();
  }
  else if (bits_per_component_ <= 16)
  {
    buf = new vil2_memory_chunk(nx_ * ny_* ncomponents_ * sizeof(unsigned short));
    jb = (unsigned short *) buf->data();
  }
  else
  {
    buf = new vil2_memory_chunk(nx_ * ny_* ncomponents_ * sizeof(unsigned int));
    kb = (unsigned int *) buf->data();
  }




  if (magic_ == 1) // ascii pbm
  {
    vcl_cerr << __FILE__ << ": ascii bitmap not implemented\n";
    return 0;
  } else if (magic_ > 4) // pgm or ppm raw image
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = ncomponents_ * bytes_per_sample;
    unsigned byte_start = start_of_data_ + (y0 * nx_ + x0) * bytes_per_pixel;
    unsigned byte_width = nx_ * bytes_per_pixel;
    unsigned byte_out_width = nx * bytes_per_pixel;

    for (unsigned y = 0; y < ny; ++y) {
      vs_->seek(byte_start + y * byte_width);
      vs_->read((unsigned char *)buf->data() + y * byte_out_width, byte_out_width);
    }
    if ( bytes_per_sample==2 && VXL_LITTLE_ENDIAN ) {
      ConvertMSBToHost( (unsigned char *)buf->data(), nx*ny*ncomponents_ );
    } else if ( bytes_per_sample > 2 ) {
      vcl_cerr << "ERROR: pnm: reading rawbits format with > 16bit samples\n";
      return 0;
    } 
    if (bits_per_component_ <= 8)
      return new vil2_image_view<unsigned char>(buf, ib, nx, ny, nplanes, nplanes, nx*nplanes, 1);
    else if (bits_per_component_ <= 16)
      return new vil2_image_view<unsigned short>(buf, jb, nx, ny, nplanes, nplanes, nx*nplanes, 1);
    else
      return new vil2_image_view<unsigned int>(buf, kb, nx, ny, nplanes, nplanes, nx*nplanes, 1);

  } else if (magic_ == 4) // pbm (bitmap) raw image
  {
    if (nplanes != 1) return 0;

    unsigned byte_width = (nx_+7)/8;
    unsigned byte_out_width = (nx+7)/8;

    for (unsigned y = 0; y < ny; ++y) {
      vil_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      vs_->seek(byte_start);
      unsigned char a; vs_->read(&a, 1L);
      for (unsigned x = 0; x < nx; ++x)
      {
        bb[y * nx + x] = (a & 0x80) != 0;
        a <<= 1;
        if (x%8 == 7)
          vs_->read(&a, 1L);
      }
    }
    assert (buf->size() == nx*ny*sizeof(bool));
    return new vil2_image_view<bool>(buf, bb, nx, ny, 1, 1, nx, nx*ny);
  }
  else // ascii (non-raw) image data
  {
    vs_->seek(start_of_data_);
    //0. Skip to the starting line
    //
    for (unsigned t = 0; t < y0*nx_*ncomponents_; ++t) { int a; (*vs_) >> a; }
    for (unsigned y = 0; y < ny; ++y) {
      // 1. Skip to column x0
      for (unsigned t = 0; t < x0*ncomponents_; ++t) { int a; (*vs_) >> a; }
      for (unsigned x = 0; x < nx; ++x) {
      // 2. Skip to the starting component
        for (unsigned t = 0; t < plane0; ++t) { int a; (*vs_) >> a; }
        // 3. Read the data
        if (bits_per_component_ <= 8)
          for (unsigned p = 0; p < nplanes; ++p) { int a; (*vs_) >> a; *(ib++)=a; }
        else if (bits_per_component_ <= 16)
          for (unsigned p = 0; p < nplanes; ++p) { int a; (*vs_) >> a; *(jb++)=a; }
        else
          for (unsigned p = 0; p < nplanes; ++p) { int a; (*vs_) >> a; *(kb++)=a; }
      // 4. Skip to the next column
        for (unsigned t = 0; t < (ncomponents_-plane0-nplanes); ++t) { int a; (*vs_) >> a; }
      }
      // 5. Skip to the next line
      for (unsigned t = 0; t < (nx_-x0-nx)*ncomponents_; ++t) { int a; (*vs_) >> a; }
    }
    if (bits_per_component_ <= 8)
      return new vil2_image_view<unsigned char>(buf, (unsigned char*)buf->data(), nx, ny, nplanes, nplanes, nx*nplanes, 1);
    else if (bits_per_component_ <= 16)
      return new vil2_image_view<unsigned short>(buf, (unsigned short*)buf->data(), nx, ny, nplanes, nplanes, nx*nplanes, 1);
    else
      return new vil2_image_view<unsigned int>(buf, (unsigned int*)buf->data(), nx, ny, nplanes, nplanes, nx*nplanes, 1);
  }


}


vil2_image_view_base* vil2_pnm_image::get_copy_view(
  unsigned x0, unsigned y0, unsigned plane0,
  unsigned nx, unsigned ny, unsigned nplanes) const
{
  // The data is on disk, so all views are copy views.
  return get_view(x0, y0, plane0, nx, ny, nplanes);
}


void operator<<(vil_stream& vs, int a) {
  char buf[128]; vcl_sprintf(buf, " %d\n", a); vs.write(buf,vcl_strlen(buf));
}

bool vil2_pnm_image::put_view(const vil2_image_view_base& view,
                                      unsigned x0, unsigned y0, unsigned plane0)
{
  if (!view_fits(view, x0, y0, plane0)) return false;


  if (view.is_a() == "vil2_image_view<int>" && bits_per_component_ < 32 ||
      view.is_a() == "vil2_image_view<short>" && bits_per_component_ < 16 ||
      view.is_a() == "vil2_image_view<uchar>" && bits_per_component_ < 8)
  {
      vcl_cerr << "ERROR: " << __FILE__ << ":\n Can't fit view into pnm component size\n" << vcl_endl;
      return false;
  }
  
  unsigned char const*  ob = 0;
  unsigned short const* pb = 0;
  unsigned int const*   qb = 0;

  if (view.is_a() == "vil2_image_view<uchar>")
    ob = static_cast<const vil2_image_view<unsigned char>& >(view).top_left_ptr();
  else if (view.is_a() == "vil2_image_view<short>")
    pb = static_cast<const vil2_image_view<unsigned short>& >(view).top_left_ptr();
  else if (view.is_a() == "vil2_image_view<int>")
    qb = static_cast<const vil2_image_view<unsigned int>& >(view).top_left_ptr();
  else
  {
      vcl_cerr << "ERROR: " << __FILE__ << ":\n Do not support putting "
        << view.is_a() << " views into pnm image_data objects " << vcl_endl;
      return false;
  }


  if (magic_ == 1) // ascii pbm
  {
    vcl_cerr << __FILE__ << ": ascii bitmap not implemented\n";
    return false;
  } else if (magic_ > 4) // pgm or ppm raw image
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = ncomponents_ * bytes_per_sample;
    vil_streampos byte_start = start_of_data_ + (y0 * nx_ + x0) * bytes_per_pixel;
    unsigned byte_width = nx_ * bytes_per_pixel;
    unsigned byte_out_width = view.nx() * bytes_per_pixel;

    if ( bytes_per_sample==1 || ( bytes_per_sample==2 && VXL_BIG_ENDIAN ) ) {
      for (unsigned y = 0; y < view.ny(); ++y) {
        vs_->seek(byte_start + y * byte_width);
        vs_->write(ob + y * byte_out_width, byte_out_width);
      }
    } else if ( bytes_per_sample==2 ) {
      // Little endian host; must convert words to have MSB first.
      // Can't convert the input buffer, because it's not ours.
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<unsigned char> tempbuf( byte_out_width );
      for (int y = 0; y < view.ny(); ++y) {
        vs_->seek(byte_start + y * byte_width);
        vcl_memcpy( &tempbuf[0], ob + y * byte_out_width, byte_out_width );
        ConvertHostToMSB( &tempbuf[0], view.ny()*ncomponents_ );
        vs_->write(&tempbuf[0], byte_out_width);
      }
    } else {
      vcl_cerr << "ERROR: pnm: writing rawbits format with > 16bit samples\n";
      return false;
    } 
  }
  else if (magic_ == 4) // pbm (bitmap) raw image
  {
    int byte_width = (nx_+7)/8;
    int byte_out_width = (view.nx()+7)/8;

    for (int y = 0; y < view.ny(); ++y) {
      vil_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      vs_->seek(byte_start);
      int s = x0&7; // = x0%8;
      int t = 0;
      unsigned char a = 0, b = ob[y * byte_out_width];
      if (s) {
        vs_->read(&a, 1L);
        vs_->seek(byte_start);
        a &= ((1<<s)-1)<<(8-s); // clear the last 8-s bits of a
      }
      for (int x = 0; x < view.nx(); ++x) {
        if (b&(1<<(7-t))) a |= 1<<(7-s); // single bit; high bit = first
        if (t >= 7) { b = ob[y * byte_out_width + (x+1)/8]; t = 0; }
        else ++t;
        if (s >= 7) { vs_->write(&a, 1L); ++byte_start; s = 0; a = 0; }
        else ++s;
      }
      if (s) {
        if (x0+view.nx() < nx_) {
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
    if (x0 > 0 || y0 > 0 || view.nx() < nx_)
      return false; // can only write the full image in this mode
    vs_->seek(start_of_data_);
    for (int y = 0; y < view.ny(); ++y) {
      if (bits_per_component_ <= 8)
        for (int x = 0; x < view.nx()*ncomponents_; ++x) { (*vs_) << ob[x]; }
      else if (bits_per_component_ <= 16)
        for (int x = 0; x < view.nx()*ncomponents_; ++x) { (*vs_) << pb[x]; }
      else
        for (int x = 0; x < view.nx()*ncomponents_; ++x) { (*vs_) << qb[x]; }
      ob += view.nx(); pb += view.nx(); qb += view.nx();
    }
  }

  return true;
}

