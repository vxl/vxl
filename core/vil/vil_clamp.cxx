// This is core/vil/vil_clamp.cxx
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

#include "vil_clamp.h"
#include <vcl_cstring.h>
#include <vcl_cassert.h>
#include <vil/vil_property.h>


vil_image_resource_sptr vil_clamp(const vil_image_resource_sptr &src, double lo, double hi)
{
  return new vil_clamp_image_resource(src, lo, hi);
}


vil_clamp_image_resource::vil_clamp_image_resource(vil_image_resource_sptr const& src, double lo, double hi):
  src_(src),
  lo_(lo),
  hi_(hi)
{
  assert (lo <= hi);
}

  //: Extra property information
bool vil_clamp_image_resource::get_property(char const* tag, void* property_value) const
{
  if (0==vcl_strcmp(tag, vil_property_read_only))
    return property_value ? (*static_cast<bool*>(property_value)) = true : true;

  return src_->get_property(tag, property_value);
}

vil_image_view_base_sptr vil_clamp_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                                                 unsigned j0, unsigned nj) const
{
  vil_image_view_base_sptr vs = src_->get_copy_view(i0, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    vil_clamp(static_cast<vil_image_view<T >&>(*vs), static_cast<vil_image_view<T >&>(*vs), \
              T (lo_), T (hi_)); \
    break;

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
// How might you clamp a vcl_complex image ?
#undef macro
  default:
    return 0;
  }
  return vs;
}
