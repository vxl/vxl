// This is core/vil/file_formats/vil_mit.cxx
//
// Author: Joris Schouteden
// Created: 18 Feb 2000
// Converted from vil1 by Peter Vanroose on 17 June 2003.
//
//-----------------------------------------------------------------------------

#include "vil_mit.h"

static char const* vil_mit_format_tag = "mit";

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_stream_read.h>
#include <vil/vil_stream_write.h>

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

vil_image_resource_sptr vil_mit_file_format::make_input_image(vil_stream* is)
{
  is->seek(0L);
  unsigned int type = vil_stream_read_little_endian_uint_16(is);

  if (!(type == MIT_UNSIGNED ||
        type == MIT_RGB      ||
        type == MIT_SIGNED   ||
        type == MIT_FLOAT    ))
    return 0;

  unsigned int bpp = vil_stream_read_little_endian_uint_16(is);
  if (bpp != 1 && bpp != 8 && bpp != 16 && bpp != 32 && bpp != 64)
    return 0;

#ifdef DEBUG
  unsigned int width = vil_stream_read_little_endian_uint_16(is);
  unsigned int height= vil_stream_read_little_endian_uint_16(is);
  vcl_cerr << __FILE__ " : here we go:\n"
           << __FILE__ " : type_ = " << type << vcl_endl
           << __FILE__ " : bits_per_pixel_ = " << bpp << vcl_endl
           << __FILE__ " : width_ = " << width << vcl_endl
           << __FILE__ " : height_ = " << height << vcl_endl;
#endif
  return new vil_mit_image(is);
}

vil_image_resource_sptr vil_mit_file_format::make_output_image(vil_stream* is,
                                                               unsigned int ni, unsigned int nj, unsigned int nplanes,
                                                               vil_pixel_format format)
{
  return new vil_mit_image(is, ni, nj, nplanes, format);
}

char const* vil_mit_file_format::tag() const
{
  return vil_mit_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil_mit_image::vil_mit_image(vil_stream* is)
  : is_(is)
{
  is_->ref();
  if (!read_header())
  {
    vcl_cerr << "vil_mit: cannot read file header; creating dummy 0x0 image\n";
    ni_ = nj_ = 0; components_ = 1; type_ = 1;
    format_ = VIL_PIXEL_FORMAT_BYTE;
  }
}

char const* vil_mit_image::file_format() const
{
  return vil_mit_format_tag;
}

vil_mit_image::vil_mit_image(vil_stream* is,
                             unsigned int ni, unsigned int nj, unsigned int nplanes,
                             vil_pixel_format format)
  : is_(is), ni_(ni), nj_(nj), components_(nplanes), format_(format)
{
  is_->ref();
  write_header();
}

vil_mit_image::~vil_mit_image()
{
  is_->unref();
}

bool vil_mit_image::get_property(char const * /*tag*/, void * /*prop*/) const
{
  // This is not an in-memory image type, nor is it read-only:
  return false;
}

bool vil_mit_image::read_header()
{
  is_->seek(0L);

  type_ = vil_stream_read_little_endian_uint_16(is_);
  unsigned int bpp = vil_stream_read_little_endian_uint_16(is_);
  ni_ = vil_stream_read_little_endian_uint_16(is_);
  nj_ = vil_stream_read_little_endian_uint_16(is_);

  if (type_ == MIT_UNSIGNED) {  // gray
    components_ = 1;
    format_ = bpp==8  ? VIL_PIXEL_FORMAT_BYTE :
              bpp==16 ? VIL_PIXEL_FORMAT_UINT_16 :
              bpp==32 ? VIL_PIXEL_FORMAT_UINT_32 :
              bpp==1  ? VIL_PIXEL_FORMAT_BOOL :
                        VIL_PIXEL_FORMAT_UNKNOWN;
  }
  else if (type_ == MIT_SIGNED) {
    components_ = 1;
    format_ = bpp==8  ? VIL_PIXEL_FORMAT_SBYTE :
              bpp==16 ? VIL_PIXEL_FORMAT_INT_16 :
              bpp==32 ? VIL_PIXEL_FORMAT_INT_32 :
                        VIL_PIXEL_FORMAT_UNKNOWN;
  }
  else if (type_ == MIT_RGB) {
    components_ = 3;
    format_ = bpp==8  ? VIL_PIXEL_FORMAT_BYTE :
              bpp==16 ? VIL_PIXEL_FORMAT_UINT_16 :
              bpp==32 ? VIL_PIXEL_FORMAT_UINT_32 :
                        VIL_PIXEL_FORMAT_UNKNOWN;
  }
  else if (type_ == MIT_FLOAT) {
    components_ = 1;
    format_ = bpp==32 ? VIL_PIXEL_FORMAT_FLOAT :
              bpp==64 ? VIL_PIXEL_FORMAT_DOUBLE :
                        VIL_PIXEL_FORMAT_UNKNOWN;
  }
  else
    return false;

  return format_ != VIL_PIXEL_FORMAT_UNKNOWN;
}

bool vil_mit_image::write_header()
{
  is_->seek(0L);

  if (format_ == VIL_PIXEL_FORMAT_UINT_32 ||
      format_ == VIL_PIXEL_FORMAT_UINT_16 ||
      format_ == VIL_PIXEL_FORMAT_BYTE ||
      format_ == VIL_PIXEL_FORMAT_BOOL)
  {
    if (components_ == 3) type_ = MIT_RGB;
    else if (components_ == 1) type_ = MIT_UNSIGNED;
    else vcl_cerr << __FILE__ " : Can only write RGB or grayscale MIT images\n"
                  << " (format="<<format_<<", #components="<<components_<<")\n";
  }
  else if (format_ == VIL_PIXEL_FORMAT_INT_32 ||
           format_ == VIL_PIXEL_FORMAT_INT_16 ||
           format_ == VIL_PIXEL_FORMAT_SBYTE)
  {
    if (components_ == 1) type_ = MIT_SIGNED;
    else vcl_cerr << __FILE__ " : Can only write RGB or grayscale MIT images\n"
                  << " (format="<<format_<<", #components="<<components_<<")\n";
  }
  else if (format_ == VIL_PIXEL_FORMAT_RGB_UINT_32 ||
           format_ == VIL_PIXEL_FORMAT_RGB_INT_32 ||
           format_ == VIL_PIXEL_FORMAT_RGB_UINT_16 ||
           format_ == VIL_PIXEL_FORMAT_RGB_INT_16 ||
           format_ == VIL_PIXEL_FORMAT_RGB_BYTE ||
           format_ == VIL_PIXEL_FORMAT_RGB_SBYTE)
  {
    if (components_ == 1) type_ = MIT_RGB;
    else vcl_cerr << __FILE__ " : Can only write RGB or grayscale MIT images\n"
                  << " (format="<<format_<<", #components="<<components_<<")\n";
  }
  else if (format_ == VIL_PIXEL_FORMAT_RGB_FLOAT ||
           format_ == VIL_PIXEL_FORMAT_RGB_DOUBLE)
  {
    if (components_ == 1) type_ = MIT_FLOAT;
    else vcl_cerr << __FILE__ " : Can only write grayscale float-pixel MIT images\n"
                  << " (format="<<format_<<", #components="<<components_<<")\n";
  }
  else
    vcl_cerr << __FILE__ " : Can only write RGB or grayscale MIT images\n"
             << " (format="<<format_<<", #components="<<components_<<")\n";

  vil_stream_write_little_endian_uint_16(is_, type_);
  unsigned int bpp = 8 * bytes_per_pixel();
  if (format_ == VIL_PIXEL_FORMAT_BOOL) bpp = 1;
  vil_stream_write_little_endian_uint_16(is_, bpp);
  vil_stream_write_little_endian_uint_16(is_, ni_);
  vil_stream_write_little_endian_uint_16(is_, nj_);
  return true;
}

static inline void swap(void* p,int length)
{
  char* t = (char*)p;
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    vcl_cerr << "Swapping " << *(vxl_uint_32*)p;
    if (length == sizeof(float)) vcl_cerr << " (or " << *(float*)p << ')';
  }
#endif
  for (int j=0;2*j<length;++j) { char c = t[j]; t[j] = t[length-j-1]; t[length-j-1] = c; }
#ifdef DEBUG
  if (length == sizeof(vxl_uint_32) && *(vxl_uint_32*)p != 0) {
    vcl_cerr << " to " << *(vxl_uint_32*)p;
    if (length == sizeof(float)) vcl_cerr << " (or " << *(float*)p << ')';
    vcl_cerr << '\n';
  }
#endif
}

vil_image_view_base_sptr vil_mit_image::get_copy_view(unsigned int x0, unsigned int xs,
                                                      unsigned int y0, unsigned int ys) const
{
  assert(x0+xs<=ni_);
  assert(y0+ys<=nj_);
  unsigned int pix_size = 8*bytes_per_pixel();
  if (format_==VIL_PIXEL_FORMAT_BOOL) pix_size = 1;
  if (format_==VIL_PIXEL_FORMAT_BOOL && x0%8 != 0)
    vcl_cerr << "vil_mit_image::get_copy_view(): Warning: x0 should be a multiple of 8 for this type of image\n";
  pix_size *= components_;

  vxl_uint_32 rowsize = (pix_size*xs+7)/8;
  vil_memory_chunk_sptr buf = new vil_memory_chunk(rowsize*ys,format_);
  vxl_byte* ib = reinterpret_cast<vxl_byte*>(buf->data());
  for (unsigned int y = y0; y < y0+ys; ++y)
  {
    is_->seek(8L + y*((ni_*pix_size+7)/8) + x0*pix_size/8);
    is_->read(ib, rowsize);
    ib += rowsize;
  }
  if (VXL_BIG_ENDIAN && bytes_per_pixel() > 1) { // MIT image data is little-endian
    ib = reinterpret_cast<vxl_byte*>(buf->data());
    for (unsigned int i=0;i<xs*ys*components_;++i)
      swap(ib+i*bytes_per_pixel(),bytes_per_pixel());
  }

#define ARGS(T) buf, reinterpret_cast<T*>(buf->data()), ni_,nj_,components_, components_,ni_*components_,1
  if (format_ == VIL_PIXEL_FORMAT_BOOL)         return new vil_image_view<bool>       (ARGS(bool));
  else if (format_ == VIL_PIXEL_FORMAT_BYTE)    return new vil_image_view<vxl_byte>   (ARGS(vxl_byte));
  else if (format_ == VIL_PIXEL_FORMAT_SBYTE)   return new vil_image_view<vxl_sbyte>  (ARGS(vxl_sbyte));
  else if (format_ == VIL_PIXEL_FORMAT_UINT_16) return new vil_image_view<vxl_uint_16>(ARGS(vxl_uint_16));
  else if (format_ == VIL_PIXEL_FORMAT_INT_16)  return new vil_image_view<vxl_int_16> (ARGS(vxl_int_16));
  else if (format_ == VIL_PIXEL_FORMAT_UINT_32) return new vil_image_view<vxl_uint_32>(ARGS(vxl_uint_32));
  else if (format_ == VIL_PIXEL_FORMAT_INT_32)  return new vil_image_view<vxl_int_32> (ARGS(vxl_int_32));
  else if (format_ == VIL_PIXEL_FORMAT_FLOAT)   return new vil_image_view<float>      (ARGS(float));
  else if (format_ == VIL_PIXEL_FORMAT_DOUBLE)  return new vil_image_view<double>     (ARGS(double));
  else return 0;
#undef ARGS
}

bool vil_mit_image::put_view(vil_image_view_base const& buf, unsigned int x0, unsigned int y0)
{
  assert(buf.pixel_format() == format_); // pixel formats of image and buffer must match
  if (!view_fits(buf, x0, y0) || buf.nplanes() != components_)
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }
  unsigned int ni = buf.ni();
  unsigned int nj = buf.nj();
#ifdef DEBUG
  vcl_cerr<<"vil_mit_image::put_view() : buf="
          <<ni<<'x'<<nj<<'x'<< buf.nplanes()<<'p'
          <<" at ("<<x0<<','<<y0<<")\n";
#endif
  vil_image_view<vxl_byte> const& ibuf = reinterpret_cast<vil_image_view<vxl_byte> const&>(buf);
  bool buf_is_planar = false;
  if (ibuf.istep() == int(components_) && ibuf.jstep() == int(components_*ni) &&
      (ibuf.planestep() == 1 || components_ == 1))
    buf_is_planar = false;
  else if (ibuf.istep() == 1 && ibuf.jstep() == int(ni) &&
           (ibuf.planestep() == int(ni*nj) || components_ == 1))
    buf_is_planar = true;
  else
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n"
             << " view does not fit: istep="<<ibuf.istep()
             << ", jstep="<<ibuf.jstep()
             << ", planestep="<<ibuf.planestep()
             << " instead of "<<components_<<','<<components_*ni<<','<<1
             << " or 1,"<<ni<<','<<ni*nj<<'\n';
    return false;
  }
  const vxl_byte* ob = ibuf.top_left_ptr();
  unsigned int pix_size = 8*bytes_per_pixel();
  if (format_==VIL_PIXEL_FORMAT_BOOL) pix_size = 1;
  if (format_==VIL_PIXEL_FORMAT_BOOL && x0%8 != 0)
    vcl_cerr << "vil_mit_image::put_view(): Warning: x0 should be a multiple of 8 for this type of image\n";
  pix_size *= components_;

  vxl_uint_32 rowsize = (pix_size*ni+7)/8;

  if (VXL_LITTLE_ENDIAN || bytes_per_pixel() == 1) // MIT image data is little-endian
  {
    if (buf_is_planar && components_ > 1) // have to interleave pixels
    {
      vil_streampos sz = (pix_size/components_+7)/8;
      for (unsigned int y = y0; y < y0+nj; ++y)
        for (unsigned int x = x0; x < x0+ni; ++x)
        {
          is_->seek(8L + y*((ni_*pix_size+7)/8) + x*pix_size/8);
          for (unsigned int p=0; p<components_; ++p)
            if (sz != is_->write(ob+p*ni*nj*sz, sz))
              vcl_cerr << "WARNING: " << __FILE__ << ":\n"
                       << " could not write "<<sz<<" bytes to stream; y,x="<<y<<','<<x<<'\n';
#ifdef DEBUG
            else
              vcl_cerr << "written "<<sz<<" bytes to stream; y,x="<<y<<','<<x<<'\n';
#endif
            ob += sz;
        }
    }
    else
      for (unsigned int y = y0; y < y0+nj; ++y)
      {
        is_->seek(8L + y*((ni_*pix_size+7)/8) + x0*pix_size/8);
        if ((vil_streampos)rowsize != is_->write(ob, rowsize))
          vcl_cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" bytes to stream; y="<<y<<'\n';
#ifdef DEBUG
        else
          vcl_cerr << "written "<<rowsize<<" bytes to stream; y="<<y<<'\n';
#endif
        ob += rowsize;
      }
  }
  else // VXL_BIG_ENDIAN ==> must swap bytes
  {
    if (buf_is_planar && components_ > 1) // have to interleave pixels
    {
      vil_streampos sz = bytes_per_pixel();
      vxl_byte* tempbuf = new vxl_byte[components_*sz];
      for (unsigned int y = y0; y < y0+nj; ++y)
        for (unsigned int x = x0; x < x0+ni; ++x)
        {
          for (unsigned int p=0; p<components_; ++p) {
            vcl_memcpy(tempbuf+p*sz, ob+p*ni*nj, sz);
            swap(tempbuf+p*sz,sz);
          }
          is_->seek(8L + pix_size*(y*ni_+x)/8);
          if (vil_streampos(components_*sz) != is_->write(tempbuf, components_*sz))
             vcl_cerr << "WARNING: " << __FILE__ << ":\n"
                      << " could not write "<<components_*sz<<" bytes to stream; y,x="<<y<<','<<x<<'\n';
#ifdef DEBUG
          else
            vcl_cerr << "written "<<components_*sz<<" bytes to stream; y,x="<<y<<','<<x<<'\n';
#endif
          ob += sz;
        }
      delete[] tempbuf;
    }
    else
    {
      vxl_byte* tempbuf = new vxl_byte[rowsize];
      for (unsigned int y = y0; y < y0+nj; ++y)
      {
        vcl_memcpy(tempbuf, ob, rowsize);
        for (vxl_uint_32 i=0; i<rowsize; i+=bytes_per_pixel())
          swap(tempbuf+i,bytes_per_pixel());
        is_->seek(8L + bytes_per_pixel()*(y*ni_+x0));
        if ((vil_streampos)rowsize != is_->write(tempbuf, rowsize))
          vcl_cerr << "WARNING: " << __FILE__ << ":\n"
                   << " could not write "<<rowsize<<" bytes to stream; y="<<y<<'\n';
#ifdef DEBUG
        else
          vcl_cerr << "written "<<rowsize<<" bytes to stream; y="<<y<<'\n';
#endif
        ob += rowsize;
      }
      delete[] tempbuf;
    }
  }
  return true;
}
