// This is mul/vil2/vil2_convert.cxx
#ifndef vil2_convert_h_
#define vil2_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Some standard conversion functions.
// \author Ian Scott.

#include <vil2/vil2_transform.h>

//: Performs conversion between different pixel types.
// For floating point types to integers it performs rounding.
template <class In, class Out>
class vil2_convert_cast_pixel
{
  public:
  Out operator () (In v) const { return (Out)v; }
};



VCL_DEFINE_SPECIALIZATION
inline vxl_byte vil2_convert_cast_pixel<float, vxl_byte>::operator () (float v) const
{ return (vxl_byte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_byte vil2_convert_cast_pixel<double, vxl_byte>::operator () (double v) const
{ return (vxl_byte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_sbyte vil2_convert_cast_pixel<float, vxl_sbyte>::operator () (float v) const
{ return (vxl_sbyte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_sbyte vil2_convert_cast_pixel<double, vxl_sbyte>::operator () (double v) const
{ return (vxl_sbyte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_16 vil2_convert_cast_pixel<float, vxl_uint_16>::operator () (float v) const
{ return (vxl_uint_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_16 vil2_convert_cast_pixel<double, vxl_uint_16>::operator () (double v) const
{ return (vxl_uint_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_16 vil2_convert_cast_pixel<float, vxl_int_16>::operator () (float v) const
{ return (vxl_int_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_16 vil2_convert_cast_pixel<double, vxl_int_16>::operator () (double v) const
{ return (vxl_int_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_32 vil2_convert_cast_pixel<float, vxl_uint_32>::operator () (float v) const
{ return (vxl_uint_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_32 vil2_convert_cast_pixel<double, vxl_uint_32>::operator () (double v) const
{ return (vxl_uint_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_32 vil2_convert_cast_pixel<float, vxl_int_32>::operator () (float v) const
{ return (vxl_int_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_32 vil2_convert_cast_pixel<double, vxl_int_32>::operator () (double v) const
{ return (vxl_int_32)(v+0.5); }



//: Cast one pixel type to another (with rounding).
// There must be a cast operator from inP to outP
// \relates vil2_image_view
template <class inP, class outP>
inline void vil2_convert_cast(vil2_image_view<inP >&src, vil2_image_view<outP >&dest)
{
  vil2_transform(src, dest, vil2_convert_cast_pixel<inP, outP>());
}


#endif // vil2_convert_h_
