// This is mul/vil2/vil2_convert.cxx

//:
// \file
// \brief For some reason functions are not getting instantiated on VC60
// \author Ian Scott.

#include <vil2/vil2_convert.h>
#include <vcl_compiler.h>

//: Performs conversion between different pixel types.
// For floating point types to integers it performs rounding.
template <class In, class Out>
inline Out vil2_convert_pixel(In v, Out dummy=0)
{
  return (Out)v;
}

VCL_DEFINE_SPECIALIZATION
inline vxl_byte vil2_convert_pixel(float v, vxl_byte dummy)
{ return (vxl_byte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_byte vil2_convert_pixel(double v, vxl_byte dummy)
{ return (vxl_byte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_sbyte vil2_convert_pixel(float v, vxl_sbyte dummy)
{ return (vxl_sbyte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_sbyte vil2_convert_pixel(double v, vxl_sbyte dummy)
{ return (vxl_sbyte)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_16 vil2_convert_pixel(float v, vxl_uint_16 dummy)
{ return (vxl_uint_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_16 vil2_convert_pixel(double v, vxl_uint_16 dummy)
{ return (vxl_uint_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_16 vil2_convert_pixel(float v, vxl_int_16 dummy)
{ return (vxl_int_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_16 vil2_convert_pixel(double v, vxl_int_16 dummy)
{ return (vxl_int_16)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_32 vil2_convert_pixel(float v, vxl_uint_32 dummy)
{ return (vxl_uint_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_uint_32 vil2_convert_pixel(double v, vxl_uint_32 dummy)
{ return (vxl_uint_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_32 vil2_convert_pixel(float v, vxl_int_32 dummy)
{ return (vxl_int_32)(v+0.5); }

VCL_DEFINE_SPECIALIZATION
inline vxl_int_32 vil2_convert_pixel(double v, vxl_int_32 dummy)
{ return (vxl_int_32)(v+0.5); }

// Apply a unary operation to each pixel in src to get dest.
template <class inP, class outP>
inline void vil2_convert(vil2_image_view<inP >&src, vil2_image_view<outP >&dest)
{
  vil2_transform(src, dest, &vil2_convert_pixel<inP, outP>);
}


#endif // vil2_convert_h_
