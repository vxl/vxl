// This is mul/vil2/vil2_transpose.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil2_transpose.h"
#include "vcl_cstring.h"
#include "vil2/vil2_property.h"




vil2_image_resource_sptr vil2_transpose(const vil2_image_resource_sptr &src)
{
  return new vil2_transpose_image_resource(src);
}


vil2_transpose_image_resource::vil2_transpose_image_resource(vil2_image_resource_sptr const& src):
  src_(src)
{
}




//: Return the name of the class;
vcl_string vil2_transpose_image_resource::is_a() const
{
  static const vcl_string class_name_="vil2_transpose_image_resource";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil2_transpose_image_resource::is_class(vcl_string const& s) const
{
  return s==vil2_transpose_image_resource::is_a() || vil2_image_resource::is_class(s);
}

vil2_image_view_base_sptr vil2_transpose_image_resource::get_copy_view(unsigned i0, unsigned ni, 
                                                unsigned j0, unsigned nj) const
{
  vil2_image_view_base_sptr vs = src_->get_copy_view(j0, nj, i0, ni);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil2_image_view<T > (vil2_transpose(static_cast<const vil2_image_view<T >&>(*vs)));
    break;

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
    return 0;
  }
  return vs;
}

vil2_image_view_base_sptr vil2_transpose_image_resource::get_view(unsigned i0, unsigned ni,
                                         unsigned j0, unsigned nj) const
{
  vil2_image_view_base_sptr vs = src_->get_view(j0, nj, i0, ni);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil2_image_view<T > (vil2_transpose(static_cast<const vil2_image_view<T >&>(*vs)));
    break;

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
    return 0;
  }
  return vs;
}

//: Put the data in this view back into the image source.
bool vil2_transpose_image_resource::put_view(const vil2_image_view_base& im, unsigned i0,
                      unsigned j0)
{
  switch (im.pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return src_->put_view(vil2_transpose(static_cast<const vil2_image_view<T >&>(im)), j0, i0); \
    break;

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

