// This is core/vil/file_formats/vil_pnm.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_pnm.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h> // for sprintf
#include <vcl_vector.h>

#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vxl_config.h> // for VXL_BIG_ENDIAN and vxl_byte

#include <vil/vil_property.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_chunk.h>

#if 0 // see comment below
# include <vil/vil_rgb.h>
#endif

char const* vil_pnm_format_tag = "pnm";

static inline bool iseol(int c)
{
  return c == 10 || c == 13;
}

static inline bool isws(int c)
{
  return c == ' ' || c == '\t' || c == 10 || c == 13;
}

vil_image_resource_sptr vil_pnm_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  unsigned char buf[3];
  vs->read(buf, 3L);
  bool ok = ((buf[0] == 'P') &&
             iseol(buf[2]) &&
             (buf[1] >= '1' && buf[2] <= '6'));
  if (!ok)
    return 0;

  return new vil_pnm_image(vs);
}

vil_image_resource_sptr vil_pnm_file_format::make_output_image(vil_stream* vs,
                                                               unsigned ni,
                                                               unsigned nj,
                                                               unsigned nplanes,
                                                               vil_pixel_format format)
{
  return new vil_pnm_image(vs, ni, nj, nplanes, format);
}

char const* vil_pnm_file_format::tag() const
{
  return vil_pnm_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_pnm_image::vil_pnm_image(vil_stream* vs):
  vs_(vs)
{
  vs_->ref();
  read_header();
}

bool vil_pnm_image::get_property(char const * tag, void * value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = bits_per_component_;
    return true;
  }

  return false;
}

char const* vil_pnm_image::file_format() const
{
  return vil_pnm_format_tag;
}

vil_pnm_image::vil_pnm_image(vil_stream* vs, unsigned ni, unsigned nj,
                             unsigned nplanes, vil_pixel_format format):
  vs_(vs)
{
  vs_->ref();
  ni_ = ni;
  nj_ = nj;

  ncomponents_ = nplanes;
  format_ = vil_pixel_format_component_format(format);
  if (nplanes == 1 &&
      (format==VIL_PIXEL_FORMAT_RGB_BYTE ||
       format==VIL_PIXEL_FORMAT_RGB_SBYTE ||
       format==VIL_PIXEL_FORMAT_RGB_INT_16 ||
       format==VIL_PIXEL_FORMAT_RGB_INT_16 ||
       format==VIL_PIXEL_FORMAT_RGB_INT_32 ||
       format==VIL_PIXEL_FORMAT_RGB_INT_32 ||
       format==VIL_PIXEL_FORMAT_RGB_FLOAT ||
       format==VIL_PIXEL_FORMAT_RGB_DOUBLE
     ))
    ncomponents_ = 3;
  if (format==VIL_PIXEL_FORMAT_BOOL)
    bits_per_component_ = 1;
  else
    bits_per_component_ = 8*vil_pixel_format_sizeof_components(format);

  if (ncomponents_ == 3)
    magic_ = 6;
  else if (ncomponents_ == 1)
  {
    if (bits_per_component_ == 1)
      magic_ = 4;
    else
      magic_ = 5;
  }
  else assert(!"nplanes must be 1 or 3");

  // pnm allows 16 bit samples in rawbits format, stored MSB, but not 32 bit.
  if (bits_per_component_ > 16) magic_ -= 3;

  if (bits_per_component_ < 31)
    maxval_ = (1L<<bits_per_component_)-1;
  else
    maxval_ = 0x7FFFFFFF; // not 0xFFFFFFFF as the pnm format does not allow values > MAX_INT

  write_header();
}

vil_pnm_image::~vil_pnm_image()
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
  unsigned char* ptr = static_cast<unsigned char*>(buf);
  for ( int i=0; i < num_words; ++i )
  {
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
  unsigned char* ptr = static_cast<unsigned char*>(buf);
  for ( int i=0; i < num_words; ++i )
  {
    unsigned char t = *ptr;
    *ptr = *(ptr+1);
    *(ptr+1) = t;
    ptr += 2;
  }
#endif
}


//: This method accepts any valid PNM file (first 3 bytes "P1\n" to "P6\n")
bool vil_pnm_image::read_header()
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
  ni_ = ReadInteger(vs_,temp);

  //Skip over spaces and comments
  SkipSpaces(vs_,temp);

  //Read in Height
  nj_ = ReadInteger(vs_,temp);

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

  if (magic_ == 1 || magic_ == 4) bits_per_component_ = 1;
  else if (maxval_ == 0) assert(!"problem reading maxval from PNM file");
  else if (maxval_ <= 0xFF) bits_per_component_ = 8;
  else if (maxval_ <= 0xFFFF) bits_per_component_ = 16;
  else if (maxval_ <= 0xFFFFFF) bits_per_component_ = 24;
  else if (maxval_ <= 0x7FFFFFFF) bits_per_component_ = 32;
  else assert(!"vil_pnm_image: maxval is too big");

  switch (magic_)
  {
  case 1:  // pbm format
  case 4:
    format_ = VIL_PIXEL_FORMAT_BOOL;
    break;
  case 2:  // pgm format
  case 5:
  case 3:  // ppm format
  case 6:
    if (bits_per_component_ <= 8)
      format_ = VIL_PIXEL_FORMAT_BYTE;
    else if (bits_per_component_ <= 16)
      format_ = VIL_PIXEL_FORMAT_UINT_16;
    else
      format_ = VIL_PIXEL_FORMAT_UINT_32;
  }

  return true;
}

bool vil_pnm_image::write_header()
{
  vs_->seek(0L);

  char buf[1024];
  vcl_sprintf(buf, "P%d\n#vil pnm image, #c=%u, bpc=%u\n%u %u\n",
              magic_, ncomponents_, bits_per_component_, ni_, nj_);
  vs_->write(buf, vcl_strlen(buf));
  if (magic_ != 1 && magic_ != 4)
  {
    vcl_sprintf(buf, "%lu\n", maxval_);
    vs_->write(buf, vcl_strlen(buf));
  }
  start_of_data_ = vs_->tell();
  return true;
}

static bool operator>>(vil_stream& vs, int& a)
{
  char c; vs.read(&c,1L);
  SkipSpaces(&vs,c);
  if (c < '0' || c > '9') return false; // non-digit found
  a = ReadInteger(&vs,c);
  return true;
}

vil_image_view_base_sptr vil_pnm_image::get_copy_view(
  unsigned x0, unsigned ni, unsigned y0, unsigned nj) const
{
  bool* bb = 0;
  unsigned char* ib = 0;
  unsigned short* jb = 0;
  unsigned int* kb = 0;

  vil_memory_chunk_sptr buf;

  if (bits_per_component_ == 1)
  {
    buf = new vil_memory_chunk(ni_ * nj_* nplanes() * sizeof(bool),VIL_PIXEL_FORMAT_BOOL);
    bb = reinterpret_cast<bool *>(buf->data());
  }
  else if (bits_per_component_ <= 8)
  {
    buf = new vil_memory_chunk(ni_ * nj_* nplanes() * 1,VIL_PIXEL_FORMAT_BYTE);
    ib = reinterpret_cast<vxl_byte*>(buf->data());
  }
  else if (bits_per_component_ <= 16)
  {
    buf = new vil_memory_chunk(ni_ * nj_* nplanes() * 2,VIL_PIXEL_FORMAT_UINT_16);
    jb = reinterpret_cast<vxl_uint_16*>(buf->data());
  }
  else
  {
    buf = new vil_memory_chunk(ni_ * nj_* nplanes() * 4,VIL_PIXEL_FORMAT_UINT_32);
    kb = reinterpret_cast<vxl_uint_32*>(buf->data());
  }

  if (magic_ > 4) // pgm or ppm raw image
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = nplanes() * bytes_per_sample;
    unsigned byte_start = start_of_data_ + (y0 * ni_ + x0) * bytes_per_pixel;
    unsigned byte_width = ni_ * bytes_per_pixel;
    unsigned byte_out_width = ni * bytes_per_pixel;

    for (unsigned y = 0; y < nj; ++y)
    {
      vs_->seek(byte_start);
      vs_->read((unsigned char *)buf->data() + y * byte_out_width, byte_out_width);
      byte_start += byte_width;
    }
    if ( bytes_per_sample==2 && VXL_LITTLE_ENDIAN )
      ConvertMSBToHost( reinterpret_cast<unsigned char *>(buf->data()), ni*nj*nplanes() );
    else if ( bytes_per_sample > 2 )
    {
      vcl_cerr << "ERROR: pnm: reading rawbits format with > 16bit samples\n";
      return 0;
    }
#if 0 // see comment below
    if (ncomponents_ == 1) {
#endif
      if (bits_per_component_ <= 1)
        return new vil_image_view<bool>(buf, bb, ni, nj, nplanes(), nplanes(), ni*nplanes(), 1);
      if (bits_per_component_ <= 8)
        return new vil_image_view<vxl_byte>(buf, ib, ni, nj, nplanes(), nplanes(), ni*nplanes(), 1);
      else if (bits_per_component_ <= 16)
        return new vil_image_view<vxl_uint_16>(buf, jb, ni, nj, nplanes(), nplanes(), ni*nplanes(), 1);
      else
        return new vil_image_view<vxl_uint_32>(buf, kb, ni, nj, nplanes(), nplanes(), ni*nplanes(), 1);
#if 0 // never return vil_image_view<vil_rgb<T> > : default image representation is planar
    } else if (ncomponents_ == 3) {
      if (bits_per_component_ <= 8)
        return new vil_image_view<vil_rgb<vxl_byte> >(buf, (vil_rgb<vxl_byte>*)ib, ni, nj, 1, 1, ni, 1);
      else if (bits_per_component_ <= 16)
        return new vil_image_view<vil_rgb<vxl_uint_16> >(buf, (vil_rgb<vxl_uint_16>*)jb, ni, nj, 1, 1, ni, 1);
      else
        return new vil_image_view<vil_rgb<vxl_uint_32> >(buf, (vil_rgb<vxl_uint_32>*)kb, ni, nj, 1, 1, ni, 1);
    } else return 0;
#endif // 0
  } else if (magic_ == 4) // pbm (bitmap) raw image
  {
    unsigned byte_width = (ni_+7)/8;

    for (unsigned y = 0; y < nj; ++y)
    {
      vil_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      vs_->seek(byte_start);
      unsigned char a; vs_->read(&a, 1L);
      for (unsigned x = 0; x < ni; ++x)
      {
        bb[y * ni + x] = (a & 0x80) != 0;
        a <<= 1;
        if (x%8 == 7)
          vs_->read(&a, 1L);
      }
    }
    assert (buf->size() == ni*nj*sizeof(bool));
    return new vil_image_view<bool>(buf, bb, ni, nj, 1, 1, ni, ni*nj);
  }
  else // ascii (non-raw) image data
  {
    vs_->seek(start_of_data_);
    //0. Skip to the starting line
    //
    for (unsigned t = 0; t < y0*ni_*nplanes(); ++t) { int a; (*vs_) >> a; }
    for (unsigned y = 0; y < nj; ++y)
    {
      // 1. Skip to column x0
      for (unsigned t = 0; t < x0*nplanes(); ++t) { int a; (*vs_) >> a; }
      for (unsigned x = 0; x < ni; ++x)
      {
        // 3. Read the data
        if (bits_per_component_ <= 1)
          for (unsigned p = 0; p < nplanes(); ++p) { int a; (*vs_) >> a; *(bb++)=(a!=0); }
        else if (bits_per_component_ <= 8)
          for (unsigned p = 0; p < nplanes(); ++p) { int a; (*vs_) >> a; *(ib++)=a; }
        else if (bits_per_component_ <= 16)
          for (unsigned p = 0; p < nplanes(); ++p) { int a; (*vs_) >> a; *(jb++)=a; }
        else
          for (unsigned p = 0; p < nplanes(); ++p) { int a; (*vs_) >> a; *(kb++)=a; }
      }
      // 5. Skip to the next line
      for (unsigned t = 0; t < (ni_-x0-ni)*nplanes(); ++t) { int a; (*vs_) >> a; }
    }
#if 0 // see comment below
    if (ncomponents_ == 1)
    {
#endif
      if (bits_per_component_ <= 1)
        return new vil_image_view<bool>(buf, reinterpret_cast<bool*>(buf->data()), ni, nj, nplanes(), nplanes(), ni*nplanes(), 1);
      if (bits_per_component_ <= 8)
        return new vil_image_view<vxl_byte>(buf,reinterpret_cast<vxl_byte*>(buf->data()),ni,nj,nplanes(),nplanes(),ni*nplanes(),1);
      else if (bits_per_component_ <= 16)
       return new
         vil_image_view<vxl_uint_16>(buf,reinterpret_cast<vxl_uint_16*>(buf->data()),ni,nj,nplanes(),nplanes(),ni*nplanes(),1);
      else
       return new
         vil_image_view<vxl_uint_32>(buf,reinterpret_cast<vxl_uint_32*>(buf->data()),ni,nj,nplanes(),nplanes(),ni*nplanes(),1);
#if 0 // never return vil_image_view<vil_rgb<T> > : default image representation is planar
    }
    else if (ncomponents_ == 3)
    {
      if (bits_per_component_ <= 8)
        return new vil_image_view<vil_rgb<vxl_byte> >(buf, reinterpret_cast<vil_rgb<vxl_byte>*>(buf->data()), ni,nj,1, 1,ni,1);
      else if (bits_per_component_ <= 16)
       return new vil_image_view<vil_rgb<vxl_uint_16> >(buf,reinterpret_cast<vil_rgb<vxl_uint_16>*>(buf->data()),ni,nj,1,1,ni,1);
      else
       return new vil_image_view<vil_rgb<vxl_uint_32> >(buf,reinterpret_cast<vil_rgb<vxl_uint_32>*>(buf->data()),ni,nj,1,1,ni,1);
    }
    else return 0;
#endif // 0
  }
}


static void operator<<(vil_stream& vs, int a)
{
  char buf[128]; vcl_sprintf(buf, " %d\n", a); vs.write(buf,vcl_strlen(buf));
}

bool vil_pnm_image::put_view(const vil_image_view_base& view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  if ((view.pixel_format() == VIL_PIXEL_FORMAT_UINT_32 && bits_per_component_ < 32) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_INT_32  && bits_per_component_ < 32) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_16 && bits_per_component_ < 16) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_INT_16  && bits_per_component_ < 16) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_BYTE    && bits_per_component_ <  8) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_SBYTE   && bits_per_component_ <  8) ||
      (view.pixel_format() == VIL_PIXEL_FORMAT_BOOL    && bits_per_component_ <  1) ||
       view.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE ||
       view.pixel_format() == VIL_PIXEL_FORMAT_FLOAT )
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Can't fit view into pnm component size\n";
    return false;
  }

  const vil_image_view<bool>*  bb=0;
  const vil_image_view<vxl_byte>*  ob = 0;
  const vil_image_view<vxl_uint_16>* pb = 0;
  const vil_image_view<vxl_uint_32>*   qb = 0;

  if (view.pixel_format() == VIL_PIXEL_FORMAT_BOOL)
    bb = &static_cast<const vil_image_view<bool>& >(view);
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    ob = &static_cast<const vil_image_view<vxl_byte>& >(view);
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
    pb = &static_cast<const vil_image_view<vxl_uint_16>& >(view);
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
    qb = &static_cast<const vil_image_view<vxl_uint_32>& >(view);
  else
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Do not support putting "
             << view.is_a() << " views into pnm image_resource objects\n";
    return false;
  }

  if (magic_ == 5) // pgm raw image ==> nplanes() == 1
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = bytes_per_sample;
    vil_streampos byte_start = start_of_data_ + (y0 * ni_ + x0) * bytes_per_pixel;
    unsigned byte_width = ni_ * bytes_per_pixel;
    unsigned byte_out_width = view.ni() * bytes_per_pixel;

    if ( bytes_per_sample==1 )
    {
      assert(ob!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vs_->write(ob->top_left_ptr() + y * view.ni(), byte_out_width);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 && VXL_BIG_ENDIAN )
    {
      assert(pb!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vs_->write(pb->top_left_ptr() + y * view.ni(), byte_out_width);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 )
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(pb!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        vcl_memcpy(&tempbuf[0], pb->top_left_ptr() + y * view.ni(), byte_out_width);
        ConvertHostToMSB(&tempbuf[0], view.ni());
        vs_->write(&tempbuf[0], byte_out_width);
        byte_start += byte_width;
      }
    } else { // This should never occur...
      vcl_cerr << "ERROR: pgm: writing rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 6) // ppm raw image; cannot be written as efficiently as pgm
  {
    unsigned bytes_per_sample = (bits_per_component_+7)/8;
    unsigned bytes_per_pixel = nplanes() * bytes_per_sample;
    vil_streampos byte_start = start_of_data_ + (y0 * ni_ + x0) * bytes_per_pixel;
    unsigned byte_width = ni_ * bytes_per_pixel;

    if ( bytes_per_sample==1 )
    {
      assert(ob!=0);
      for (unsigned y = 0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        for (unsigned x = 0; x < view.ni(); ++x)
          for (unsigned p = 0; p < ncomponents_; ++p)
            vs_->write(&(*ob)(x,y,p), 1);
        byte_start += byte_width;
      }
    } else if ( bytes_per_sample==2 )
    {
      assert(pb!=0);
      for (unsigned y = y0; y < view.nj(); ++y)
      {
        vs_->seek(byte_start);
        for (unsigned x = x0; x < view.ni(); ++x)
        {
          vxl_uint_16 tempbuf[3];
          for (unsigned p = 0; p < ncomponents_; ++p)
            tempbuf[p] = (*pb)(x,y,p);
          ConvertHostToMSB(tempbuf, ncomponents_);
          vs_->write(tempbuf, bytes_per_pixel);
        }
        byte_start += byte_width;
      }
    } else { // This should never occur...
      vcl_cerr << "ERROR: pgm: writing rawbits format with > 16bit samples\n";
      return false;
    }
  }
  else if (magic_ == 4) // pbm (bitmap) raw image
  {
    int byte_width = (ni_+7)/8;

    assert(bb!=0);
    for (unsigned y = 0; y < view.nj(); ++y)
    {
      vil_streampos byte_start = start_of_data_ + (y0+y) * byte_width + x0/8;
      int s = x0&7; // = x0%8;
      unsigned char a = 0;
      if (s)
      {
        vs_->read(&a, 1L);
        vs_->seek(byte_start);
        a &= ((1<<s)-1)<<(8-s); // clear the last 8-s bits of a
      }
      for (unsigned x = 0; x < view.ni(); ++x)
      {
        if ((*bb)(x,y)) a |= 1<<(7-s); // single bit; high bit = first
        if (s >= 7) { vs_->write(&a, 1L); ++byte_start; s = 0; a = 0; }
        else ++s;
      }
      if (s)
      {
        if (x0+view.ni() < ni_)
        {
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
    if (x0 > 0 || y0 > 0 || view.ni() < ni_ || view.nj() < nj_)
      return false; // can only write the full image in this mode
    vs_->seek(start_of_data_);
    for (unsigned y = 0; y < view.nj(); ++y)
      for (unsigned x = 0; x < view.ni(); ++x)
        for (unsigned p = 0; p < ncomponents_; ++p)
        {
          if (bits_per_component_ <= 1)       (*vs_) << (*bb)(x,y,p);
          else if (bits_per_component_ <= 8)  (*vs_) << (*ob)(x,y,p);
          else if (bits_per_component_ <= 16) (*vs_) << (*pb)(x,y,p);
          else                                (*vs_) << (*qb)(x,y,p);
        }
  }

  return true;
}
