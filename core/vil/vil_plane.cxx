// This is core/vil/vil_plane.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim
//
//-----------------------------------------------------------------------------

#include "vil_plane.h"
#include <vcl_cassert.h>


vil_image_resource_sptr vil_plane(const vil_image_resource_sptr &src, unsigned p)
{
  return new vil_plane_image_resource(src, p);
}

vil_plane_image_resource::vil_plane_image_resource(vil_image_resource_sptr const& gi,
                                                   unsigned p):
  src_(gi),
  plane_(p)
{
  assert (p <= src_->nplanes());
}

vil_image_view_base_sptr vil_plane_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                 unsigned j0, unsigned nj) const
{
  vil_image_view_base_sptr vs = src_->get_copy_view(i0, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil_image_view<T > (vil_plane(static_cast<const vil_image_view<T >&>(*vs), plane_));

    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
    macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
    macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
    macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
    macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
    macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
    macro(VIL_PIXEL_FORMAT_FLOAT , float )
    macro(VIL_PIXEL_FORMAT_DOUBLE , double )
    macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
    macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return 0;
  }
}


vil_image_view_base_sptr vil_plane_image_resource::get_view(unsigned i0, unsigned ni,
                                                            unsigned j0, unsigned nj) const
{
  vil_image_view_base_sptr vs = src_->get_view(i0, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil_image_view<T > (vil_plane(static_cast<const vil_image_view<T >&>(*vs), plane_));

    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
    macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
    macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
    macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
    macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
    macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
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
bool vil_plane_image_resource::put_view(const vil_image_view_base& im,
                                        unsigned i0, unsigned j0)
{
  if (im.nplanes() != 1) return false;
  vil_image_view_base_sptr vs = src_->get_view(i0, im.ni(), j0, im.nj());
  if (!vs || im.pixel_format() != vs->pixel_format()) return false;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : { \
    const vil_image_view<T > view = static_cast<const vil_image_view<T >&>(im); \
    vil_image_view<T > plane = vil_plane(static_cast<vil_image_view<T >&>(*vs), plane_); \
    if (view == plane) return true; /* If we have already modified the data, do nothing */ \
    for (unsigned j=0;j<view.nj();++j) \
      for (unsigned i=0;i<view.ni();++i) \
        plane(i,j) = view(i,j); \
    return src_->put_view(*vs, i0, j0); }

    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
    macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
    macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
    macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
    macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
    macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
    macro(VIL_PIXEL_FORMAT_FLOAT , float )
    macro(VIL_PIXEL_FORMAT_DOUBLE , double )
    macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
    macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return false;
  }
}

