// This is mul/vil2/vil2_convert.h
#ifndef vil2_convert_h_
#define vil2_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Some standard conversion functions.
// \author Ian Scott.

#include <vcl_cassert.h>
#include <vil2/vil2_transform.h>
#include <vil2/vil2_math.h>

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

VCL_DEFINE_SPECIALIZATION
inline vil_rgb<vxl_byte> vil2_convert_cast_pixel<float, vil_rgb<vxl_byte> >::operator () (float v) const
{ return vil_rgb<vxl_byte>((vxl_byte)(v+0.5)); }

VCL_DEFINE_SPECIALIZATION
inline vil_rgb<vxl_byte> vil2_convert_cast_pixel<double, vil_rgb<vxl_byte> >::operator () (double v) const
{ return vil_rgb<vxl_byte>((vxl_byte)(v+0.5)); }


//: Cast one pixel type to another (with rounding).
// There must be a cast operator from inP to outP
// \relates vil2_image_view
template <class inP, class outP>
inline void vil2_convert_cast(const vil2_image_view<inP >&src, vil2_image_view<outP >&dest)
{
  if (vil2_pixel_format_of(inP()) == vil2_pixel_format_of(outP()))
    dest = src;
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

//: Convert first three planes of src image to grey, assuming rgb.
// Pixel types can be different. Rounding will take place if appropriate.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class inP, class outP>
inline void vil2_convert_planes_to_grey(const vil2_image_view<inP>&src,
                                        vil2_image_view<outP>&dest,
                                        double rw=0.2125, double gw=0.7154,
                                        double bw = 0.0721)
{
  assert(src.nplanes() >= 3);
  assert(vil2_pixel_format_num_components(src.pixel_format()) == 1);
  assert(vil2_pixel_format_num_components(dest.pixel_format()) == 1);
  dest.set_size(src.ni(), src.nj(), 1);
  for (unsigned j = 0; j < src.nj(); ++j)
    for (unsigned i = 0; i < src.ni(); ++i)
      dest(i,j) = vil2_convert_cast_pixel<double,outP>()(
        src(i,j,0)*rw + src(i,j,1)*gw + src(i,j,2)*bw);
}

//: Convert src to byte image dest by stretching to range [0,255]
// \relates vil2_image_view
template <class T>
inline void vil2_convert_stretch_range(const vil2_image_view<T>& src,
                                       vil2_image_view<vxl_byte>& dest)
{
  T min_b,max_b;
  vil2_math_value_range(src,min_b,max_b);
  double a = -1.0*double(min_b);
  double b = 0.0;
  if (max_b-min_b >0) b = 255.0/(max_b-min_b);
  dest.set_size(src.ni(), src.nj(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
        dest(i,j,p) = vil2_convert_cast_pixel<double,vxl_byte>()(a+b*src(i,j,p));
}

//: Create a greyscale image of specified pixel type from any image src.
// The output may be a reconfigured view of the input.
// \relates vil2_image_view
template <class outP>
inline vil2_image_view<outP> vil2_convert_to_grey_using_average(
  const vil2_image_view_base_sptr &src, outP /*dummy*/)
{
  // Check output is scalar component image.
  assert (vil2_pixel_format_num_components(vil2_pixel_format_of(outP())) == 1);

  if (!src) return vil2_image_view<outP>();

  // try to do it quickly
  if (vil2_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil2_image_view<outP>(src);

  // create output view
  if (!src) return vil2_image_view<outP>();
  vil2_image_view<outP> dest;

  // convert via vil2_image_view<double>
  switch (vil2_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
  case F: { \
      vil2_image_view<T > src1 = *src; \
      vil2_image_view<double> dest1; \
      vil2_math_mean_over_planes(src1, dest1); \
      vil2_convert_cast(dest1,dest); \
      break; }
macro(VIL2_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    dest.clear();
  }
  return dest;
}


//: Create a greyscale image of specified pixel type from any image src.
// The output may be a reconfigured view of the input.
// \relates vil2_image_view
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class outP>
inline vil2_image_view<outP> vil2_convert_to_grey_using_rgb_weighting(
                          const vil2_image_view_base_sptr &src,
                          outP /*dummy*/,
                          double rw=0.2125,
                          double gw=0.7154,
                          double bw=0.0721)
{
  // Check output is scalar component image.
  assert (vil2_pixel_format_num_components(vil2_pixel_format_of(outP())) == 1);

  if (!src) return vil2_image_view<outP>();

  // try to do it quickly
  if (vil2_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil2_image_view<outP>(src);

  // create output view
  if (!src) return vil2_image_view<outP>();
  vil2_image_view<outP> dest;

  // convert via vil2_image_view<double>
  switch (vil2_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
  case F: { \
      vil2_image_view<T > src1 = src; \
      vil2_image_view<double> dest1; \
      vil2_convert_planes_to_grey(src1, dest1, rw, gw, bw); \
      vil2_convert_cast(dest1,dest); \
      break; }
macro(VIL2_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL2_PIXEL_FORMAT_FLOAT , float )
macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    dest.clear();
  }
  return dest;
}


#endif // vil2_convert_h_
