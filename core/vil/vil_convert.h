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
inline void vil2_convert_cast(const vil2_image_view<inP >&src, vil2_image_view<outP >&dest)
{
  vil2_transform(src, dest, vil2_convert_cast_pixel<inP, outP>());
}



//: Convert various rgb types to greyscale, using given weights
template <class inP, class outP>
class vil2_convert_rgb_to_grey_pixel
{
  double rw_, gw_, bw_;
public:
  vil2_convert_rgb_to_grey_pixel(double rw, double gw, double bw):
    rw_(rw), gw_(gw), bw_(bw) {}

  outP operator() (vil_rgb<inP > v) const {
    return vil2_convert_cast_pixel<double,outP>()(rw_*v.r+gw_*v.g+bw_*v.b); } 
  outP operator() (vil_rgba<inP > v) const {
    return vil2_convert_cast_pixel<double,outP>()(rw_*v.r+gw_*v.g+bw_*v.b); } 
};

//: Convert single plane rgb (or rgba) images to greyscale.
// Component types can be different. Rounding will take place if appropriate.
// 
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class rgbP, class outP>
inline void vil2_convert_rgb_to_grey(const vil2_image_view<rgbP >&src, vil2_image_view<outP >&dest,
                                     double rw=0.2125, double gw=0.7154, double bw = 0.0721)
{
#if VCL_VC60 || !VCL_HAS_TYPENAME
  vil2_convert_rgb_to_grey_pixel<rgbP::value_type, outP> func(rw, gw, bw);
#else
  vil2_convert_rgb_to_grey_pixel<typename rgbP::value_type, outP> func(rw, gw, bw);
#endif
  assert(src.nplanes() == 1);
  vil2_transform(src, dest, func);
}  

template <class inP, class outP>
struct vil2_convert_grey_to_rgb_pixel
{
  vil_rgb<outP> operator() (inP v) const {
    return vil_rgb<outP>(vil2_convert_cast_pixel<double,outP>()(v)); } 
};

template <class inP, class outP>
struct vil2_convert_grey_to_rgba_pixel
{
  vil_rgba<outP> operator() (inP v) const {
    return vil_rgba<outP>(vil2_convert_cast_pixel<double,outP>()(v)); } 
};

//: Convert grey images to rgb.
// Component types can be different. Rounding will take place if appropriate.
template <class inP, class outP >
inline void vil2_convert_grey_to_rgb(const vil2_image_view<inP >&src, vil2_image_view<vil_rgb<outP> >&dest)
{
  assert(src.nplanes() == 1);
  vil2_transform(src, dest, vil2_convert_grey_to_rgb_pixel<inP, outP>());
}

//: Convert grey images to rgba.
// Component types can be different. Rounding will take place if appropriate.
template <class inP, class outP>
inline void vil2_convert_grey_to_rgba(const vil2_image_view<inP >&src, vil2_image_view<vil_rgba<outP> >&dest)
{
  assert(src.nplanes() == 1);
  vil2_transform(src, dest, vil2_convert_grey_to_rgba_pixel<inP, outP>());
}


#endif // vil2_convert_h_
