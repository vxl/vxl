// This is core/vil/vil_decimate.cxx
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

#include "vil_decimate.h"
#include <vcl_cassert.h>
#include <vil/vil_copy.h>
#include <vil/vil_crop.h>

static const unsigned long large_image_limit = 1024ul * 1024ul * 8ul; //8M Pixels

vil_image_resource_sptr vil_decimate(const vil_image_resource_sptr &src,
                                     unsigned i_factor,
                                     unsigned j_factor)
{
  return new vil_decimate_image_resource(src, i_factor, j_factor);
}


vil_decimate_image_resource::vil_decimate_image_resource(vil_image_resource_sptr const& src,
                                                         unsigned i_factor, unsigned j_factor):
  src_(src),
  i_factor_(i_factor),
  j_factor_(j_factor)
{
  assert (i_factor > 0 && j_factor > 0);
}


vil_image_view_base_sptr vil_decimate_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                    unsigned j0, unsigned nj) const
{
  if ((unsigned long)i_factor_ * (unsigned long)ni *
      (unsigned long)j_factor_ * (unsigned long)nj < large_image_limit)
  {
    vil_image_view_base_sptr vs = src_->get_copy_view(i0*i_factor_, ni*i_factor_,
                                                      j0*j_factor_, nj*j_factor_);
    if (!vs) return 0;

    switch (vs->pixel_format())
    {
  #define macro( F , T ) \
    case F : \
      return new vil_image_view<T >(vil_decimate(static_cast<vil_image_view<T >&>(*vs), \
                                                 i_factor_, j_factor_));

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
  else // do large image case.
  {
    if ((i0+ni)*i_factor_ > src_->ni() || (j0+nj)*j_factor_ > src_->nj())
      return 0;

    switch (src_->pixel_format())
    {
#define macro( F , T ) \
    case F : { \
      vil_image_view<T > view(ni,nj,src_->nplanes()); \
      for (unsigned j = 0; j < nj; ++j) \
        for (unsigned i = 0; i < ni; ++i) { \
          vil_image_view<T > pixel=src_->get_view((i+i0)*i_factor_,1,(j+j0)*j_factor_,1); \
          assert ((bool)pixel); \
          vil_copy_to_window(pixel, view, i, j); } \
      return new vil_image_view<T >(view); }

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
}

vil_image_view_base_sptr vil_decimate_image_resource::get_view(unsigned i0, unsigned ni,
                                                               unsigned j0, unsigned nj) const
{
  if ((unsigned long)i_factor_ * (unsigned long)ni *
      (unsigned long)j_factor_ * (unsigned long)nj < large_image_limit)
  {
    vil_image_view_base_sptr vs = src_->get_view(i0*i_factor_, ni*i_factor_,
                                                 j0*j_factor_, nj*j_factor_);
    if (!vs) return 0;

    switch (vs->pixel_format())
    {
#define macro( F , T ) \
    case F : \
      return new vil_image_view<T >(vil_decimate(static_cast<vil_image_view<T >&>(*vs), \
                                    i_factor_, j_factor_));

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
  else // do large image case.
    return get_copy_view(i0, ni, j0, nj);
}


//: Put the data in this view back into the image source.
bool vil_decimate_image_resource::put_view(const vil_image_view_base& im, unsigned i0,
                                           unsigned j0)
#if 0 // disable put_view, because current implementation
      // does something really stupid.
      // This put_view, should not just modify the pixels
      // selected by the decimation, but all the unselected
      // pixels around it as well.
{
  if ((unsigned long)i_factor_ * (unsigned long)im.ni() *
      (unsigned long)j_factor_ * (unsigned long)im.nj() < large_image_limit)
  {
    vil_image_view_base_sptr vs = src_->get_view(i0*i_factor_, im.ni()*i_factor_,
                                                 j0*j_factor_, im.nj()*j_factor_);
    if (!vs || im.pixel_format() != vs->pixel_format() ||
        im.nplanes() != vs->nplanes())
      return false;

    switch (vs->pixel_format())
    {
  #define macro( F , T ) \
    case F : { \
      const vil_image_view<T > view = static_cast<const vil_image_view<T >&>(im); \
      vil_image_view<T > decimated = \
        vil_decimate(static_cast<vil_image_view<T >&>(*vs), i_factor_, j_factor_); \
      if (view == decimated) return true; /* If we have already modified the data, do nothing */ \
      assert(view.ni() == decimated.ni() && view.nj() == decimated.nj()); \
      vil_copy_reformat(view, decimated); \
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
  else // do large image case.
  {
    if ((i0+im.ni())*i_factor_ > src_->ni() || (j0+im.nj())*j_factor_ > src_->nj())
      return 0;

    switch (src_->pixel_format())
    {
#define macro( F , T )  \
    case F : { \
      const vil_image_view<T > &view = static_cast<const vil_image_view<T > &>(im); \
      for (unsigned j = 0; j < im.nj(); ++j) \
        for (unsigned i = 0; i < im.ni(); ++i) { \
          vil_image_view<T > pixel=vil_crop(view,i,1,j,1); \
          assert ((bool)pixel); \
          if (!src_->put_view(pixel, (i0+i)*i_factor_, (j0+j)*j_factor_)) \
            return false; } \
      return true; }

      macro(VIL_PIXEL_FORMAT_BYTE ,   vxl_byte )
      macro(VIL_PIXEL_FORMAT_SBYTE ,  vxl_sbyte )
#if VXL_HAS_INT_64
      macro(VIL_PIXEL_FORMAT_UINT_64 ,vxl_uint_64 )
      macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
      macro(VIL_PIXEL_FORMAT_UINT_32 ,vxl_uint_32 )
      macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL_PIXEL_FORMAT_UINT_16 ,vxl_uint_16 )
      macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL_PIXEL_FORMAT_FLOAT ,  float )
      macro(VIL_PIXEL_FORMAT_DOUBLE , double )
      macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
      macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
    default:
      return false;
    }
  }
}
#else // 0
{
  vcl_cerr << "ERROR: vil_decimate_image_resource::put_view not implemented\n";
  return false;
}
#endif // 0
