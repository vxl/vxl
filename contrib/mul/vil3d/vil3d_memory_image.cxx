// This is mul/vil3d/vil3d_memory_image.cxx
//:
// \file
// \author Ian Scott

#include <iostream>
#include <cstdlib>
#include "vil3d_memory_image.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_copy.h>
#include <vil/vil_pixel_format.h>

class vil3d_image_view_base;

vil3d_memory_image::vil3d_memory_image():
   view_(new vil3d_image_view<vxl_byte>()) {}

vil3d_memory_image::vil3d_memory_image(unsigned ni, unsigned nj, unsigned nk,
                                       unsigned nplanes, vil_pixel_format format)
{
  // format should be a scalar type
  assert (vil_pixel_format_num_components(format)==1);

  switch (format)
  {
#define macro( F , T ) \
  case F :     view_ = new vil3d_image_view<T >(ni, nj, nk, nplanes); break;
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    std::cerr << "ERROR: vil3d_memory_image::vil3d_memory_image\n"
                "\t unknown format " << format << std::endl;
    std::abort();
  }
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vil3d_memory_image::get_copy_view(unsigned i0, unsigned ni,
                                                             unsigned j0, unsigned nj,
                                                             unsigned k0, unsigned nk) const
{
  if (i0 + ni > view_->ni() || j0 + nj > view_->nj() ||
      k0 + nk > view_->nk()) return nullptr;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      const vil3d_image_view< T > &v = \
        static_cast<const vil3d_image_view< T > &>(*view_); \
      vil3d_image_view< T > w(v.memory_chunk(), &v(i0,j0,k0), \
                             ni, nj, nk, v.nplanes(), \
                             v.istep(), v.jstep(), v.kstep(), v.planestep()); \
      return new vil3d_image_view< T >(vil3d_copy_deep(w)); }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    return nullptr;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vil3d_memory_image::get_view(unsigned i0, unsigned ni,
                                                        unsigned j0, unsigned nj,
                                                        unsigned k0, unsigned nk) const
{
  if (i0 + ni > view_->ni() || j0 + nj > view_->nj() ||
      k0 + nk > view_->nk()) return nullptr;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      const vil3d_image_view< T > &v = \
        static_cast<const vil3d_image_view< T > &>(*view_); \
      return new vil3d_image_view< T >(v.memory_chunk(), &v(i0,j0,k0), \
                                      ni, nj, nk, v.nplanes(), \
                                      v.istep(), v.jstep(), v.kstep(), \
                                      v.planestep()); }
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    return nullptr;
  }
}


//: Put the data in this view back into the image source.
// Currently not yet implemented.
// \return true on success.
bool vil3d_memory_image::put_view(const vil3d_image_view_base& im,
       unsigned i0, unsigned j0, unsigned k0)
{
  if (view_-> pixel_format() != im.pixel_format()) return false;
  if (!view_fits(im, i0, j0, k0)) return false;

  switch(view_->pixel_format())
  {
#define macro( F , T ) \
  case  F : { \
      vil3d_image_view< T > &v = \
        static_cast<vil3d_image_view< T > &>(*view_); \
      const vil3d_image_view< T > &w = \
        static_cast<const vil3d_image_view< T > &>(im); \
      if (v.memory_chunk() == w.memory_chunk()) \
      { \
        if (&v(i0,j0,k0) != w.origin_ptr()) { \
          std::cerr << "ERROR: vil_memory_image::put_view()\n" \
            "different window from that used in get_view()" << std::endl; \
          std::abort(); } \
        else return true; /* The user has already modified the data in place. */ \
      } \
      vil3d_copy_to_window(w, v, i0, j0, k0); \
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
#undef macro

  default:
    std::cerr << "WARNING: vil3d_memory_image::put_view()\n"
                "\t Unexpected pixel type" << view_->pixel_format() << std::endl;
    return false;
  }
}
