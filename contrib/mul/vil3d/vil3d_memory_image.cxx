// This is mul/vil3d/vil3d_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott

#include "vil3d_memory_image.h"
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vxl_config.h> // for vxl_uint_32 etc.
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_copy.h>
#include <vil2/vil2_pixel_format.h>

class vil3d_image_view_base;

vil3d_memory_image::vil3d_memory_image():
   view_(new vil3d_image_view<vxl_byte>()) {}

vil3d_memory_image::vil3d_memory_image(unsigned ni, unsigned nj, unsigned nk,
                                       unsigned nplanes, vil2_pixel_format format)
{
  // format should be a scalar type
  assert (vil2_pixel_format_num_components(format)==1);

  switch (format)
  {
#define macro( F , T ) \
  case F :     view_ = new vil3d_image_view<T >(ni, nj, nk, nplanes); break;
macro(VIL2_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_BOOL , bool )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    vcl_cerr << "ERROR: vil3d_memory_image::vil3d_memory_image\n"
                "\t unknown format " << format << vcl_endl;
    vcl_abort();
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
      k0 + nk > view_->nk()) return 0;

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
macro(VIL2_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_BOOL , bool )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    return 0;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vil3d_memory_image::get_view(unsigned i0, unsigned ni,
                                                        unsigned j0, unsigned nj,
                                                        unsigned k0, unsigned nk) const
{
  if (i0 + ni > view_->ni() || j0 + nj > view_->nj() ||
      k0 + nk > view_->nk()) return 0;

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
macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_BOOL , bool )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    return 0;
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
        if (&v(i0,j0,k0) != w.origin_ptr()) \
          assert("ERROR: vil3d_memory_image::put_view()\n" \
                 "different window from that used in get_view()"); \
        else return true; /* The user has already modified the data in place. */ \
      } \
      vil3d_copy_to_window(w, v, i0, j0, k0); \
      return true; }

macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_BOOL , bool )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro

  default:
    vcl_cerr << "WARNING: vil3d_memory_image::put_view()\n"
                "\t Unexpected pixel type" << view_->pixel_format() << vcl_endl;
    return 0;
  }
}

