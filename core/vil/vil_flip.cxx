// This is core/vil/vil_flip.cxx
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

#include "vil_flip.h"

vil_image_resource_sptr vil_flip_lr(const vil_image_resource_sptr &src)
{
  return new vil_flip_lr_image_resource(src);
}

vil_flip_lr_image_resource::vil_flip_lr_image_resource(vil_image_resource_sptr const& src):
  src_(src)
{
}

vil_image_view_base_sptr vil_flip_lr_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                   unsigned j0, unsigned nj) const
{
  if (i0 + ni > src_->ni()) return 0;
  vil_image_view_base_sptr vs = src_->get_copy_view(src_->ni()- i0-ni, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_flip_lr(static_cast<const vil_image_view<T >&>(*vs)));

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

vil_image_view_base_sptr vil_flip_lr_image_resource::get_view(unsigned i0, unsigned ni,
                                                              unsigned j0, unsigned nj) const
{
  if (i0 + ni > src_->ni()) return 0;
  vil_image_view_base_sptr vs = src_->get_view(src_->ni()- i0-ni, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_flip_lr(static_cast<const vil_image_view<T >&>(*vs)));

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
bool vil_flip_lr_image_resource::put_view(const vil_image_view_base& im, unsigned i0,
                                          unsigned j0)
{
  if (i0 + im.ni() > src_->ni()) return false;
  switch (im.pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return src_->put_view(vil_flip_lr(static_cast<const vil_image_view<T >&>(im)), src_->ni()-i0-im.ni(), j0);

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

vil_image_resource_sptr vil_flip_ud(const vil_image_resource_sptr &src)
{
  return new vil_flip_ud_image_resource(src);
}

vil_flip_ud_image_resource::vil_flip_ud_image_resource(vil_image_resource_sptr const& src):
  src_(src)
{
}


vil_image_view_base_sptr vil_flip_ud_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                   unsigned j0, unsigned nj) const
{
  if (j0 + nj > src_->nj()) return 0;
  vil_image_view_base_sptr vs = src_->get_copy_view(i0, ni, src_->nj()- j0-nj, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_flip_ud(static_cast<const vil_image_view<T >&>(*vs)));

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

vil_image_view_base_sptr vil_flip_ud_image_resource::get_view(unsigned i0, unsigned ni,
                                                              unsigned j0, unsigned nj) const
{
  if (i0 + ni > src_->ni()) return 0;
  vil_image_view_base_sptr vs = src_->get_view(i0, ni, src_->nj()-j0-nj, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return new vil_image_view<T > (vil_flip_ud(static_cast<const vil_image_view<T >&>(*vs)));

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
bool vil_flip_ud_image_resource::put_view(const vil_image_view_base& im, unsigned i0,
                                          unsigned j0)
{
  if (i0 + im.ni() > src_->ni()) return false;
  switch (im.pixel_format())
  {
#define macro( F, T ) \
  case F : \
    return src_->put_view(vil_flip_ud(static_cast<const vil_image_view<T >&>(im)), i0, src_->nj()-j0-im.nj()); \

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
