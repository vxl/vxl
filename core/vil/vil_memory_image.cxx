// This is core/vil/vil_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott

#include "vil_memory_image.h"
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vil/vil_image_view.h>
#include <vil/vil_copy.h>
#include <vil/vil_pixel_format.h>

class vil_image_view_base;

vil_memory_image::vil_memory_image():
   view_(new vil_image_view<vxl_byte>()) {}

vil_memory_image::vil_memory_image(unsigned ni, unsigned nj, unsigned nplanes, vil_pixel_format format)
{
  switch (vil_pixel_format_component_format(format))
  {
#define macro( F , T  ) \
  case F :     view_ = new vil_image_view<T >(ni, nj, nplanes, \
    vil_pixel_format_num_components(format)); break;
macro(VIL_PIXEL_FORMAT_BYTE ,   vxl_byte)
macro(VIL_PIXEL_FORMAT_SBYTE ,  vxl_sbyte)
macro(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
macro(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32)
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16)
macro(VIL_PIXEL_FORMAT_BOOL ,   bool)
macro(VIL_PIXEL_FORMAT_FLOAT ,  float)
macro(VIL_PIXEL_FORMAT_DOUBLE , double)
macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    vcl_cerr << "ERROR: vil_memory_image::vil_memory_image\n"
                "\t unknown format " << format << vcl_endl;
    vcl_abort();
  }
}


//: Create a wrapper around the given image_view
vil_memory_image::vil_memory_image(vil_image_view_base const &view)
{
  switch (vil_pixel_format_component_format(view.pixel_format()))
  {
#define macro( F , T ) \
  case F :  view_ = new vil_image_view<T >(view); break;
macro(VIL_PIXEL_FORMAT_BYTE ,   vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE ,  vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL ,   bool )
macro(VIL_PIXEL_FORMAT_FLOAT ,  float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    vcl_cerr << "ERROR: vil_memory_image::vil_memory_image\n"
                "\t unknown format " << 
      vil_pixel_format_component_format(view.pixel_format()) << vcl_endl;
    vcl_abort();
  }
  assert (view_->ni() == view.ni() && view_->nj() == view.nj());
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil_image_view_base_sptr vil_memory_image::get_copy_view(unsigned i0, unsigned ni,
                                                         unsigned j0, unsigned nj) const
{
  if (i0 + ni > view_->ni() || j0 + nj > view_->nj()) return 0;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      const vil_image_view< T > &v = \
        static_cast<const vil_image_view< T > &>(*view_); \
      vil_image_view< T > w(v.memory_chunk(), &v(i0,j0), \
                            ni, nj, v.nplanes(), \
                            v.istep(), v.jstep(), v.planestep()); \
      return new vil_image_view< T >(vil_copy_deep(w)); }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return 0;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil_image_view_base_sptr vil_memory_image::get_view(unsigned i0, unsigned ni,
                                                    unsigned j0, unsigned nj) const
{
  if (i0 + ni > view_->ni() || j0 + nj > view_->nj()) return 0;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      const vil_image_view< T > &v = \
        static_cast<const vil_image_view< T > &>(*view_); \
      return new vil_image_view< T >(v.memory_chunk(), &v(i0,j0), \
                                     ni, nj, v.nplanes(), \
                                     v.istep(), v.jstep(), v.planestep()); }
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return 0;
  }
}


//: Put the data in this view back into the image source.
// \return true on success.
bool vil_memory_image::put_view(const vil_image_view_base& im,unsigned i0, unsigned j0)
{
  if (view_-> pixel_format() != im.pixel_format()) return false;
  if (!view_fits(im, i0, j0)) return false;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      vil_image_view< T > &v = \
        static_cast<vil_image_view< T > &>(*view_); \
      const vil_image_view< T > &w = \
        static_cast<const vil_image_view< T > &>(im); \
      if (v.memory_chunk() == w.memory_chunk()) \
      { \
        if (&v(i0,j0) != w.top_left_ptr()) { \
          vcl_cerr << "ERROR: vil_memory_image::put_view()\n" \
            "different window from that used in get_view()" << vcl_endl; \
          vcl_abort(); } \
        else return true; /* The user has already modified the data in place. */ \
      } \
      vil_copy_to_window(w, v, i0, j0); \
      return true; }

macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro

  default:
    vcl_cerr << "WARNING: vil_memory_image::put_view()\n"
                "\t Unexpected pixel type" << view_->pixel_format() << vcl_endl;
    return 0;
  }
}

