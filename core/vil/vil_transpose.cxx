// This is core/vil/vil_transpose.cxx
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

#include "vil_transpose.h"

vil_image_resource_sptr vil_transpose(const vil_image_resource_sptr &src)
{
  return new vil_transpose_image_resource(src);
}

vil_transpose_image_resource::vil_transpose_image_resource(vil_image_resource_sptr const& src):
  src_(src)
{
}

vil_image_view_base_sptr vil_transpose_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                     unsigned j0, unsigned nj) const
{
  vil_image_view_base_sptr vs = src_->get_copy_view(j0, nj, i0, ni);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_transpose(static_cast<const vil_image_view<T >&>(*vs)));

    macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
    macro(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
    macro(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
    macro(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
    macro(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
    macro(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
    macro(VIL_PIXEL_FORMAT_FLOAT, float)
    macro(VIL_PIXEL_FORMAT_DOUBLE, double)
    macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
    macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return 0;
  }
}

vil_image_view_base_sptr vil_transpose_image_resource::get_view(unsigned i0, unsigned ni,
                                                                unsigned j0, unsigned nj) const
{
  vil_image_view_base_sptr vs = src_->get_view(j0, nj, i0, ni);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_transpose(static_cast<const vil_image_view<T >&>(*vs)));

    macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
    macro(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
    macro(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
    macro(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
    macro(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
    macro(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
    macro(VIL_PIXEL_FORMAT_FLOAT, float)
    macro(VIL_PIXEL_FORMAT_DOUBLE, double)
    macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
    macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return 0;
  }
}

//: Put the data in this view back into the image source.
bool vil_transpose_image_resource::put_view(const vil_image_view_base& im, unsigned i0,
                                            unsigned j0)
{
  switch (im.pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return src_->put_view(vil_transpose(static_cast<const vil_image_view<T >&>(im)), j0, i0);

    macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte)
    macro(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte)
#if VXL_HAS_INT_64
    macro(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64)
    macro(VIL_PIXEL_FORMAT_INT_64, vxl_int_64)
#endif
    macro(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32)
    macro(VIL_PIXEL_FORMAT_INT_32, vxl_int_32)
    macro(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16)
    macro(VIL_PIXEL_FORMAT_INT_16, vxl_int_16)
    macro(VIL_PIXEL_FORMAT_FLOAT, float)
    macro(VIL_PIXEL_FORMAT_DOUBLE, double)
    macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
    macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
  default:
    return false;
  }
}
