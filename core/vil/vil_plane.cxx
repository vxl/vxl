// This is mul/vil2/vil2_plane.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil2_plane.h"


vil2_image_resource_sptr vil2_plane(const vil2_image_resource_sptr &src, unsigned p)
{
  return new vil2_plane_image_resource(src, p);
}


vil2_plane_image_resource::vil2_plane_image_resource(vil2_image_resource_sptr const& gi,
                                                     unsigned p):
  src_(gi),
  plane_(p)
{
  assert (p <= src_->nplanes());
}


//: Return the name of the class;
vcl_string vil2_plane_image_resource::is_a() const
{
  static const vcl_string class_name_="vil2_plane_image_resource";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil2_plane_image_resource::is_class(vcl_string const& s) const
{
  return s==vil2_plane_image_resource::is_a() || vil2_image_resource::is_class(s);
}

vil2_image_view_base_sptr vil2_plane_image_resource::get_copy_view(unsigned i0, unsigned ni, 
                                                unsigned j0, unsigned nj) const
{
  vil2_image_view_base_sptr vs = src_->get_copy_view(i0, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil2_image_view<T > (vil2_plane(static_cast<const vil2_image_view<T >&>(*vs), plane_)); 

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
}


vil2_image_view_base_sptr vil2_plane_image_resource::get_view(unsigned i0, unsigned ni,
                                           unsigned j0, unsigned nj) const
{
  vil2_image_view_base_sptr vs = src_->get_view(i0, ni, j0, nj);
  if (!vs) return 0;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : \
    return new vil2_image_view<T > (vil2_plane(static_cast<const vil2_image_view<T >&>(*vs), plane_)); 

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
}



//: Put the data in this view back into the image source.
bool vil2_plane_image_resource::put_view(const vil2_image_view_base& im, unsigned i0,
                      unsigned j0)
{
  if (im.nplanes() != 1) return false;
  vil2_image_view_base_sptr vs = src_->get_view(i0, im.ni(), j0, im.nj());
  if (!vs || im.pixel_format() != vs->pixel_format()) return false;

  switch (vs->pixel_format())
  {
#define macro( F , T ) \
  case F : { \
    const vil2_image_view<T > view = static_cast<const vil2_image_view<T >&>(im); \
    vil2_image_view<T > plane = vil2_plane(static_cast<vil2_image_view<T >&>(*vs), plane); \
    for (unsigned j=0;j<view.nj();++j) \
      for (unsigned i=0;i<view.ni();++i) \
        plane(i,j) = view(i,j); \
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

