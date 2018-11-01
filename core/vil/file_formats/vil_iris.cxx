// This is core/vil/file_formats/vil_iris.cxx
//:
// \file
// \author Joris Schouteden
// \date 17 Feb 2000
//
// \verbatim
//  Modifications
//   Ported from vil1 by Peter Vanroose, 16 June 2003.
//   12-Oct-2003 - PVr - being more careful with unsigned (bug fix for Alpha)
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include "vil_iris.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

#include <vxl_config.h> // for vxl_byte, vxl_uint_32, ...

static vxl_sint_16 get_short(vil_stream* file, int location = -1); // default -1 means: read at current position
static vxl_uint_16 get_ushort(vil_stream* file, int location = -1);
static char get_char(vil_stream* file, int location = -1);
static vxl_sint_32 get_long(vil_stream* file, int location = -1);
static void send_char(vil_stream* data, char s);
static void send_short(vil_stream* data, vxl_sint_16 s);
static void send_ushort(vil_stream* data, vxl_uint_16 s);
static void send_long(vil_stream* data, vxl_sint_32 s);
static void expandrow(unsigned char *optr, unsigned char *iptr, int z);

char const* vil_iris_format_tag = "iris";

vil_image_resource_sptr vil_iris_file_format::make_input_image(vil_stream* is)
{
  is->seek(0L);
  if (is->file_size() < 84L) return nullptr;
  int colormap_;

  vxl_sint_16 magic_      = get_short(is);
  int storage_            = get_char(is);
  int bytes_per_component = get_char(is);
  int dimension_          = get_ushort(is);
  /*int ni_     =*/ get_ushort(is);
  /*int nj_     =*/ get_ushort(is);
  /*int nplanes_=*/ get_ushort(is);
  /*int pixmin_ =*/ get_long(is);
  /*int pixmax_ =*/ get_long(is);

  is->seek(24L);
  char imagename[81];
  if (is->read(imagename, 80L) != 80) return nullptr;

  colormap_ = get_long(is);

  if (magic_ != 474) return nullptr;
  if (storage_ != 0 && storage_ != 1) return nullptr;
  if (colormap_ == 3) return nullptr;
  if (dimension_ == 3 && colormap_ != 0) return nullptr;
  if (dimension_ > 3 || dimension_ < 1) return nullptr;
  if (bytes_per_component < 1 || bytes_per_component > 2) return nullptr;

  return new vil_iris_generic_image(is,imagename);
}

vil_image_resource_sptr vil_iris_file_format::make_output_image(vil_stream* is,
                                                                unsigned int ni, unsigned int nj, unsigned int nplanes,
                                                                vil_pixel_format format)
{
  return new vil_iris_generic_image(is, ni, nj, nplanes, format);
}

char const* vil_iris_file_format::tag() const
{
  return vil_iris_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_iris_generic_image::vil_iris_generic_image(vil_stream* is, char const* imagename):
  starttab_(nullptr), lengthtab_(nullptr), is_(is)
{
  is_->ref();
  read_header();
  std::strncpy(imagename_, imagename, 80);
}

bool vil_iris_generic_image::get_property(char const* /*tag*/, void* /*prop*/) const
{
  // This is not an in-memory image type, nor is it read-only:
  return false;
}

char const* vil_iris_generic_image::file_format() const
{
  return vil_iris_format_tag;
}

vil_iris_generic_image::vil_iris_generic_image(vil_stream* is,
                                               unsigned int ni, unsigned int nj, unsigned int nplanes,
                                               vil_pixel_format format)
  : starttab_(nullptr), lengthtab_(nullptr), is_(is), magic_(474), ni_(ni), nj_(nj),
    nplanes_(nplanes), format_(format), pixmin_(0),
    pixmax_(vil_pixel_format_sizeof_components(format)==1 ? 255 : 65535),
    storage_(0), dimension_(nplanes_==1 ? 2 : 3), colormap_(0)
{
  is_->ref();

  if (vil_pixel_format_sizeof_components(format) <= 2)
  {
    std::strcpy(imagename_, "written by vil_iris_generic_image");

    if (nplanes_ != 1 && nplanes_ != 3 && nplanes_ != 4)
      std::cerr << __FILE__ ": Cannot write iris image, can only do grayscale or RGB(A)\n";
    write_header();
  }
  else std::cerr << __FILE__ ": Cannot write iris image, which needs 8 or 16 bits per component\n";
}

vil_iris_generic_image::~vil_iris_generic_image()
{
  is_->unref();
  delete [] starttab_;
  delete [] lengthtab_;
}

bool vil_iris_generic_image::read_header()
{
  is_->seek(0L);

  magic_ = get_short(is_, 0);
  if (magic_ != 474)
  {
    std::cerr << __FILE__ ": This is not an Iris RGB file: magic number is incorrect: "
             << magic_ << std::endl;
    return false;
  }

  storage_ = get_char(is_);
  if (storage_ != 0 && storage_ != 1)
  {
    std::cerr << __FILE__ ": This is not an Iris RGB file: storage must be RLE or VERBATIM\n";
    return false;
  }

  int bytes_per_component = get_char(is_);
  dimension_ = get_ushort(is_);
  ni_        = get_ushort(is_);
  nj_        = get_ushort(is_);
  nplanes_   = get_ushort(is_);
  pixmin_    = get_long(is_);
  pixmax_    = get_long(is_);
  format_ = bytes_per_component==1 ? VIL_PIXEL_FORMAT_BYTE :
            bytes_per_component==2 ? VIL_PIXEL_FORMAT_UINT_16 :
                                     VIL_PIXEL_FORMAT_UNKNOWN;

  // DUMMY1 starts at 20
  // image name starts at 24

  is_->seek(24L);
  is_->read(imagename_, 80L);

  // COLORMAP starts at 104
  colormap_ = get_long(is_);

  // _DUMMY2 starts at 108, ends at 512

  if (colormap_ == 3)
  {
    std::cerr << __FILE__ ": This is not an ordinary Iris RGB image but a colormap file which I cannot handle\n";
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


bool vil_iris_generic_image::write_header()
{
#ifdef DEBUG
  std::cerr << __FILE__ ": vil_iris_generic_image::write_header()\n"
           << "Here we go :\n"
           << "magic_      = " << magic_    << std::endl
           << "storage_    = " << storage_ << std::endl
           << "format_     = " << format_ << std::endl
           << "dimension_  = " << dimension_ << std::endl
           << "ni_      = " << ni_ << std::endl
           << "nj_     = " << nj_ << std::endl
           << "nplanes_    = " << nplanes_ << std::endl
           << "pixmin_     = " << pixmin_ << std::endl
           << "pixmax_     = " << pixmax_ << std::endl
           << "colormap_   = " << colormap_ << std::endl
           << "imagename_  = " << imagename_ << std::endl
           << std::endl;
#endif

  char dummy[410];

  send_short(is_, static_cast<vxl_sint_16>(magic_));
  send_char(is_, (char)storage_); // either VERBATIM (0) or RLE (1)
  send_char(is_, (char)vil_pixel_format_sizeof_components(format_));  // bytes per pixel per channel
  send_ushort(is_, static_cast<vxl_uint_16>(dimension_)); // either 1 (1 scanline), 2 (grey image), or 3 (colour)
  send_ushort(is_, static_cast<vxl_uint_16>(ni_));     // width
  send_ushort(is_, static_cast<vxl_uint_16>(nj_));     // height
  send_ushort(is_, static_cast<vxl_uint_16>(nplanes_));// nr of colour bands; typically 3 (RGB) or 4 (RGBA)
  send_long(is_, pixmin_);   // minimum pixel value; typically 0
  send_long(is_, pixmax_); // maximum pixel value; typically 255 if _PBC is 1
  is_->write(dummy, 4L);
  is_->write(imagename_, 80L); // null-terminated string
  send_long(is_, colormap_); // either NORMAL (0) (RGB), DITHERED (1) (R=3,G=3,B=2 bits),
                  // SCREEN (2) (obsolete) or COLORMAP (3) (hardware-specific).

  start_of_data_ = is_->tell();

  return is_->write(dummy, 404L) == 404L;
}


static inline void swap(void* p,int length)
{
  char* t = (char*)p;
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    std::cerr << "Swapping " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      std::cerr << " (or " << *(float*)p << ')';
  }
#endif
  for (int j=0;2*j<length;++j) { char c = t[j]; t[j] = t[length-j-1]; t[length-j-1] = c; }
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    std::cerr << " to " << *(vxl_uint_32*)p;
    if (length == sizeof(float))
      std::cerr << " (or " << *(float*)p << ')';
    std::cerr << '\n';
  }
#endif
}

vil_image_view_base_sptr vil_iris_generic_image::get_copy_view( unsigned int x0, unsigned int xs,
                                                                unsigned int y0, unsigned int ys) const
{
  // I am not dealing with requests for memory outside the image
  // so just abort when getting any such request:
  assert(x0+xs<=ni_);
  assert(y0+ys<=nj_);

  if (storage_)
    return get_section_rle(x0,xs,y0,ys);
  else
    return get_section_verbatim(x0,xs,y0,ys);
}


vil_image_view_base_sptr vil_iris_generic_image::get_section_verbatim(unsigned int x0, unsigned int xs,
                                                                      unsigned int y0, unsigned int ys) const
{
  unsigned int pix_size = vil_pixel_format_sizeof_components(format_);
  unsigned int row_len = xs * pix_size;

  vil_memory_chunk_sptr buf = new vil_memory_chunk(row_len*ys*nplanes_,format_);
  auto* ib = reinterpret_cast<vxl_byte*>(buf->data());
  auto* ob = reinterpret_cast<vxl_uint_16*>(buf->data());
  vxl_byte* cbi = ib;

  // for each channel
  for (unsigned int channel=0; channel<nplanes_; ++channel)
  {
    // for each row; storage is bottom row first!
    for (unsigned int row = nj_-y0-ys; row < nj_-y0; ++row,cbi+=row_len)
    {
      is_->seek(512L + (channel * ni_*nj_ + row * ni_ + x0) * pix_size);
      is_->read(cbi, row_len);
    }
  }
  if (VXL_LITTLE_ENDIAN && pix_size > 1) // IRIS image data is big-endian
    for (unsigned int i=0;i<xs*ys*nplanes_;++i)
      swap(ob+i,pix_size);

  // Note that jstep is negative!  Hence data ref pt is not ib but ib+xs*(ys-1)
  if (format_ == VIL_PIXEL_FORMAT_BYTE)
    return new vil_image_view<vxl_byte>(buf,ib+xs*(ys-1),xs,ys,nplanes_,1,-int(xs),xs*ys);
  else if (format_ == VIL_PIXEL_FORMAT_UINT_16)
    return new vil_image_view<vxl_uint_16>(buf,ob+xs*(ys-1),xs,ys,nplanes_,1,-int(xs),xs*ys);
  else
    return nullptr;
}


vil_image_view_base_sptr vil_iris_generic_image::get_section_rle(unsigned int x0, unsigned int xs,
                                                                 unsigned int y0, unsigned int ys) const
{
  unsigned int pix_size = vil_pixel_format_sizeof_components(format_);
  unsigned int row_len = xs * pix_size;

  vil_memory_chunk_sptr buf = new vil_memory_chunk(row_len*ys*nplanes_,format_);
  auto* ib = reinterpret_cast<vxl_byte*>(buf->data());
  auto* ob = reinterpret_cast<vxl_uint_16*>(buf->data());
  vxl_byte* cbi = ib;
  auto* exrow = new unsigned char[ni_];

  // for each channel
  for (unsigned int channel=0; channel<nplanes_; ++channel)
  {
    // for each row
    for (unsigned int rowno=nj_-y0-ys; rowno<nj_-y0; ++rowno,cbi+=row_len)
    {
      // find length and start position
      unsigned long rleoffset =  starttab_[rowno+channel*nj_];
      unsigned long rlelength = lengthtab_[rowno+channel*nj_];

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
  if (format_ == VIL_PIXEL_FORMAT_BYTE)
    return new vil_image_view<vxl_byte>(buf,ib+xs*(ys-1),xs,ys,nplanes_,1,-int(xs),xs*ys);
  else if (format_ == VIL_PIXEL_FORMAT_UINT_16)
    return new vil_image_view<vxl_uint_16>(buf,ob+xs*(ys-1),xs,ys,nplanes_,1,-int(xs),xs*ys);
  else
    return nullptr;
}


bool vil_iris_generic_image::put_view( vil_image_view_base const& buf, unsigned int x0, unsigned int y0)
{
  assert(buf.pixel_format() == format_); // pixel formats of image and buffer must match
  if (!view_fits(buf, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_iris_generic_image::put_view"));
    return false;
  }
#ifdef DEBUG
  std::cerr << "vil_iris_image::put_view() : buf="
           << buf.ni()<<'x'<<buf.nj()<<'x'<< buf.nplanes()<<'p'
           << " at ("<<x0<<','<<y0<<")\n";
#endif
  const auto& buff = static_cast<vil_image_view<unsigned char> const&>(buf);
  const unsigned char* ob = buff.top_left_ptr();
  unsigned int pix_size = vil_pixel_format_sizeof_components(format_);

  std::size_t rowsize = pix_size*buf.ni();
  std::ptrdiff_t rowskip = pix_size*buff.jstep();
  std::size_t planeskip = pix_size*buff.planestep();

  if (VXL_LITTLE_ENDIAN && pix_size > 1) // IRIS image data is big-endian
  {
    // buffer for swapping bytes
    auto* tempbuf = new vxl_byte[rowsize];
    // for each channel
    for (unsigned int channel = 0; channel<nplanes_; ++channel) {
      ob += rowskip*buff.nj();
      // number of rows to write
      for (unsigned int y = nj_-y0-buf.nj(); y < nj_-y0; ++y) {
        ob -= rowskip;
        // skip to start of section
        is_->seek(512L + (channel * ni_*nj_ + y * ni_ + x0) * pix_size);
        // swap bytes before writing
        std::memcpy(tempbuf,ob,rowsize);
        for (unsigned int i=0;i<buf.ni();++i)
          swap(tempbuf+i*pix_size,pix_size);
        // write swapped bytes
        if ((vil_streampos)rowsize != is_->write(tempbuf, rowsize))
          std::cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" bytes to stream;\n"
                   << " channel="<<channel<<", y="<<y<<'\n';
#ifdef DEBUG
        else
          std::cerr << "written "<<rowsize<<" bytes to stream; channel="<<channel<<", y="<<y<<'\n';
#endif
      }
      ob += planeskip;
    }
    delete[] tempbuf;
  }
  else // (VXL_BIG_ENDIAN || pix_size == 1)
  {
    // for each channel
    for (unsigned int channel = 0; channel<nplanes_; ++channel) {
      ob += rowskip*buff.nj();
      // number of rows to write
      for (unsigned int y = nj_-y0-buf.nj(); y < nj_-y0; ++y) {
        ob -= rowskip;
        // skip to start of section
        is_->seek(512L + (channel * ni_*nj_ + y * ni_ + x0) * pix_size);
        if ((vil_streampos)rowsize != is_->write(ob, rowsize))
          std::cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" bytes to stream;\n"
                   << " channel="<<channel<<", y="<<y<<'\n';
#ifdef DEBUG
        else
          std::cerr << "written "<<rowsize<<" bytes to stream; channel="<<channel<<", y="<<y<<'\n';
#endif
      }
      ob += planeskip;
    }
  }
  return true;
}

bool vil_iris_generic_image::read_offset_tables()
{
  unsigned int tablen = nj_ * nplanes_;

  starttab_  = new unsigned long[tablen];
  lengthtab_ = new unsigned long[tablen];

  for (unsigned int i=0; i<tablen; ++i) {
    starttab_[i] = get_long(is_,512+(i*4));
  }

  unsigned int lengthtab_offset =  512 + tablen*4;

  for (unsigned int i=0; i<tablen; ++i) {
    lengthtab_[i] = get_long(is_,lengthtab_offset+(i*4));
  }

  return true;
}


vxl_sint_16 get_short(vil_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  vxl_byte buff[2];
  file->read(buff, 2L);

  // Decode from two's complement to machine format
  auto bits = static_cast<vxl_uint_16>(( buff[0] << 8 ) + buff[1]);

  if ( ( bits & 0x8000 ) != 0 )
    return static_cast<vxl_sint_16>(-( ~bits + 1 ));
  else
    return static_cast<vxl_sint_16>( bits );
}


char get_char(vil_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  char buff[1];
  file->read((void*)buff, 1L);
  return buff[0];
}

vxl_uint_16 get_ushort(vil_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  unsigned char buff[2];
  file->read((void*)buff, 2L);
  return static_cast<vxl_uint_16>((buff[0]<<8)+(buff[1]<<0));
}

vxl_sint_32 get_long(vil_stream* file, int location)
{
  if (location >= 0) file->seek(location);

  vxl_byte buff[4];
  if (file->read((void*)buff, 4L) != 4L) return 0;

  // Decode from two's complement to machine format
  vxl_uint_32 bits = ( vxl_uint_32(buff[0]) << 24 ) |
                     ( vxl_uint_32(buff[1]) << 16 ) |
                     ( vxl_uint_32(buff[2]) <<  8 ) |
                                   buff[3];

  if ( ( bits & 0x80000000L ) != 0 )
    return -vxl_sint_32( ~bits + 1 );
  else
    return vxl_sint_32( bits );
}


void send_char(vil_stream* data, char s)
{
  data->write(&s ,1L);
}

void send_short(vil_stream* data, vxl_sint_16 s)
{
  vxl_uint_16 bits;
  if ( s < 0 ) {
    bits = static_cast<vxl_uint_16>(-s);
    bits = static_cast<vxl_uint_16>(~bits + 1);
  }
  else {
    bits = static_cast<vxl_uint_16>(s);
  }

  vxl_byte buff[2];
  buff[0] = static_cast<vxl_byte>((bits >>  8) & 0xff);
  buff[1] = static_cast<vxl_byte>( bits        & 0xff);
  data->write(buff, 2L);
}

void send_ushort(vil_stream* data, vxl_uint_16 s)
{
  unsigned char buff[2];
  buff[0] = static_cast<unsigned char>((s >> 8) & 0xff);
  buff[1] = static_cast<unsigned char>( s       & 0xff);
  data->write(buff, 2L);
}

void send_long(vil_stream* data, vxl_sint_32 s)
{
  // The write out the value as a two's complement number in MSB order

  vxl_uint_32 bits;
  if ( s < 0 ) {
    bits = -s;
    bits = ~bits + 1;
  }
  else {
    bits = s;
  }

  vxl_byte buff[4];
  buff[0] = static_cast<unsigned char>( (bits >> 24) & 0xff );
  buff[1] = static_cast<unsigned char>( (bits >> 16) & 0xff );
  buff[2] = static_cast<unsigned char>( (bits >>  8) & 0xff );
  buff[3] = static_cast<unsigned char>(  bits        & 0xff );
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
