// This is mul/vil2/vil2_decimate.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil2_decimate.h"
#include <vcl_cstring.h>
#include <vcl_cassert.h>
#include <vil2/vil2_property.h>
#include <vil2/vil2_copy.h>
#include <vil2/vil2_crop.h>

static const unsigned long large_image_limit = 1024ul * 1024ul * 8ul; //8M Pixels

vil2_image_resource_sptr vil2_decimate(const vil2_image_resource_sptr &src,
                                       unsigned i_factor,
                                       unsigned j_factor)
{
  return new vil2_decimate_image_resource(src, i_factor, j_factor);
}


vil2_decimate_image_resource::vil2_decimate_image_resource(vil2_image_resource_sptr const& src,
                                                           unsigned i_factor, unsigned j_factor):
  src_(src),
  i_factor_(i_factor),
  j_factor_(j_factor)
{
  assert (i_factor > 0 && j_factor > 0);
}


vil2_image_view_base_sptr vil2_decimate_image_resource::get_copy_view(unsigned i0, unsigned ni, 
                                                unsigned j0, unsigned nj) const
{
  if ((unsigned long)i_factor_ * (unsigned long)ni *
    (unsigned long)j_factor_ * (unsigned long)nj < large_image_limit)
  {
   vil2_image_view_base_sptr vs = src_->get_copy_view(i0*i_factor_, ni*i_factor_,
      j0 * j_factor_, nj * j_factor_);
    if (!vs) return 0;

    switch (vs->pixel_format())
    {
  #define macro( F , T ) \
    case F : \
      return new vil2_image_view<T >(vil2_decimate(static_cast<vil2_image_view<T >&>(*vs), \
        i_factor_, j_factor_)); 

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
    return vs;
  }
  else // do large image case.
  {
    if ((i0+ni)*i_factor_ > src_->ni() || (j0+nj)*j_factor_ > src_->nj())
      return false;

    switch (src_->pixel_format())
    {
#define macro( F , T ) \
    case F : { \
        vil2_image_view<T > view(ni,nj,src_->nplanes()); \
        for (unsigned j = 0; j < nj; ++j) \
          for (unsigned i = 0; i < ni; ++i) { \
            vil2_image_view<T > pixel=src_->get_view((i+i0)*i_factor_,1,(j+j0)*j_factor_,1); \
            assert ((bool)pixel); \
            vil2_copy_to_window(pixel, view, i, j); } \
        return new vil2_image_view<T >(view); }

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

    default: return 0;
    }
    
  }

}

vil2_image_view_base_sptr vil2_decimate_image_resource::get_view(unsigned i0, unsigned ni, 
                                                unsigned j0, unsigned nj) const
{
  if ((unsigned long)i_factor_ * (unsigned long)ni *
    (unsigned long)j_factor_ * (unsigned long)nj < large_image_limit)
  {

    vil2_image_view_base_sptr vs = src_->get_view(i0*i_factor_, ni*i_factor_,
      j0 * j_factor_, nj * j_factor_);
    if (!vs) return 0;

    switch (vs->pixel_format())
    {
#define macro( F , T ) \
    case F : \
      return new vil2_image_view<T >(vil2_decimate(static_cast<vil2_image_view<T >&>(*vs), \
        i_factor_, j_factor_)); 

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
    return vs;
  }
  else // do large image case.
    return get_copy_view(i0, ni, j0, nj);
}

//: Put the data in this view back into the image source.
bool vil2_decimate_image_resource::put_view(const vil2_image_view_base& im, unsigned i0,
                      unsigned j0)
{
  if ((unsigned long)i_factor_ * (unsigned long)im.ni() *
    (unsigned long)j_factor_ * (unsigned long)im.nj() < large_image_limit)
  {
    vil2_image_view_base_sptr vs = src_->get_view(i0*i_factor_, im.ni()*i_factor_,
      j0 * j_factor_, im.nj() * j_factor_);
    if (!vs || im.pixel_format() != vs->pixel_format() ||
      im.nplanes() != vs->nplanes()) return false;


    switch (vs->pixel_format())
    {
  #define macro( F , T ) \
    case F : { \
      const vil2_image_view<T > view = static_cast<const vil2_image_view<T >&>(im); \
      vil2_image_view<T > decimated = \
        vil2_decimate(static_cast<vil2_image_view<T >&>(*vs), i_factor_, j_factor_); \
      if (view == decimated) return true; /* If we have already modified the data, do nothing */ \
      assert(view.ni() == decimated.ni() && view.nj() == decimated.nj()); \
      vil2_copy_reformat(view, decimated); \
      return src_->put_view(*vs, i0, j0); }

        macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
        macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
        macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
        macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
        macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
        macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
        macro(VIL2_PIXEL_FORMAT_FLOAT , float )
        macro(VIL2_PIXEL_FORMAT_DOUBLE , double )

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
//#define macro( F , T ) 
    case VIL2_PIXEL_FORMAT_FLOAT : 
      {
        const vil2_image_view<float > &view = static_cast<const vil2_image_view<float > &>(im);
        for (unsigned j = 0; j < im.nj(); ++j)
          for (unsigned i = 0; i < im.ni(); ++i)
          {
            vil2_image_view<float > pixel=vil2_crop(view,i,1,j,1);
            assert ((bool)pixel);
            if (!src_->put_view(pixel, (i0+i)*i_factor_, (j0+j)*j_factor_))
              return false;
          }
        return true;
      }
    default: return false;
    }
    
  }
}

