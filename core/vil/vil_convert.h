// This is core/vil/vil_convert.h
#ifndef vil_convert_h_
#define vil_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Some standard conversion functions.
// \author Ian Scott.
//
// This file contains a large number of image to image conversion
// functions.
// They are in two basic function types (plus a few helper functions.)
// Some involve only explicit types and convert
// a vil_image_view<T> to a vil_image_view<T>,
// the others take an inknown pixel type, using a
// vil_image_view_base_sptr. The basic conversion
// operations (e.g. casting, rounding) are available in both types.
// All of the conversions attempt to find shortcuts, so the output
// may be a reconfigured, or shallow copy of the input.
//
// \par vil_convert with explicit pixel types
// These are useful when you have two vil_image_view objects you want
// to convert between. You can use them in templates where the pixel
// type is one of the template parameters. These functions
// may create a shallow copy of the input if the types match too save
// unnecessary work.
// - vil_convert_cast
// - vil_convert_round
// - vil_convert_rgb_to_grey
// - vil_convert_planes_to_grey
// - vil_convert_stretch_range
//
// \par vil_convert with unknown pixel types
// These functions are useful when taking an image from vil_load
// of vil_image_resource::get_view(), where you may not know the
// pixel type in advance, but want to force the image into a
// particular pixel type.
// - vil_convert_cast
// - vil_convert_round
// - vil_convert_rgb_to_grey
// - vil_convert_planes_to_grey
// - vil_convert_stretch_range
// - vil_convert_to_component_order
// - vil_convert_to_n_planes
//
// In general these functions expect to take scalar pixel images as
// inputs. Even though many of these functions could return a known
// pixel-typed image, they all return a vil_image_view_base_sptr,
// so that the functions can be strung along
//
// Note that these vil_convert_..( vil_image_view_base_sptr ) functions
// are provided as a convenience for users of vil_load and
// vil_image_resource::get_view(). Their existence should not suggest
// that it is sensible to use a vil_image_view_base_sptr as storage,
// nor that it is a good idea to write a functions that
// take or return a vil_image_view_base_sptr. If you need a
// pixel-type-agnostic image container then use a vil_image_resource_sptr
//
// It may be a good idea to provide vil_image_resource_sptr based
// vil_converts as well.
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endverbatim

#include <vcl_deprecated.h>
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>
#include <vil/vil_plane.h>
#include <vil/vil_copy.h>

//: Performs conversion between different pixel types.
template <class In, class Out>
class vil_convert_cast_pixel
{
 public:
  void operator () (In v, Out &d) const;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// deal with conversions from floating point types to/from some compounds
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in, vil_rgb<out > >::operator () (in v, vil_rgb<out >& d) const \
{ d.r = d.g = d.b = (out)v; } \
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<vil_rgb<out >, in >::operator () (vil_rgb<out > v, in& d) const \
{ d = (in)(0.2125*v.r+0.7154*v.g+0.0721*v.b); } \
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in, vil_rgba<out > >::operator () (in v, vil_rgba<out >& d) const \
{ d.r = d.g = d.b = (out)v; d.a=1; } \
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<vil_rgba<out >, in >::operator () (vil_rgba<out > v, in& d) const \
{ d = (in)(0.2125*v.r+0.7154*v.g+0.0721*v.b); }
macro( vxl_byte , vxl_byte )
macro( vxl_int_16 , vxl_byte )
macro( vxl_uint_16 , vxl_byte )
macro( vxl_int_32 , vxl_byte )
macro( vxl_uint_32 , vxl_byte )
macro( float , vxl_byte )
macro( double , vxl_byte )
macro( vxl_byte , vxl_sbyte )
macro( vxl_int_16 , vxl_sbyte )
macro( vxl_uint_16 , vxl_sbyte )
macro( vxl_int_32 , vxl_sbyte )
macro( vxl_uint_32 , vxl_sbyte )
macro( float , vxl_sbyte )
macro( double , vxl_sbyte )
macro( vxl_int_16 , vxl_int_16 )
macro( float , vxl_int_16 )
macro( double , vxl_int_16 )
macro( vxl_uint_16 , vxl_uint_16 )
macro( float , vxl_uint_16 )
macro( double , vxl_uint_16 )
macro( vxl_int_32 , vxl_int_32 )
macro( float , vxl_int_32 )
macro( double , vxl_int_32 )
macro( vxl_uint_32 , vxl_uint_32 )
macro( float , vxl_uint_32 )
macro( double , vxl_uint_32 )
macro( double , float )
#if VXL_HAS_INT_64
macro( vxl_int_64 , vxl_byte )
macro( vxl_uint_64 , vxl_byte )
macro( vxl_int_64 , vxl_sbyte )
macro( vxl_uint_64 , vxl_sbyte )
macro( vxl_int_64 , vxl_int_64 )
macro( float , vxl_int_64 )
macro( double , vxl_int_64 )
macro( vxl_uint_64 , vxl_uint_64 )
macro( float , vxl_uint_64 )
macro( double , vxl_uint_64 )
#endif
#undef macro
#define macro( inout )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<inout, inout >::operator () ( \
  inout v, inout& d) const { d=v; }
macro( vxl_byte )
macro( vxl_sbyte )
macro( vxl_uint_16 )
macro( vxl_int_16 )
macro( vxl_uint_32 )
macro( vxl_int_32 )
#if VXL_HAS_INT_64
macro( vxl_uint_64 )
macro( vxl_int_64 )
#endif
macro( float )
macro( double )
#undef macro
VCL_DEFINE_SPECIALIZATION
inline void vil_convert_cast_pixel<vil_rgb<vxl_byte>, vil_rgb<vxl_byte> >::operator () (
  vil_rgb<vxl_byte> v, vil_rgb<vxl_byte>& d) const { d.r=v.r, d.g=v.g, d.b=v.b; }
VCL_DEFINE_SPECIALIZATION
inline void vil_convert_cast_pixel<vil_rgba<vxl_byte>, vil_rgba<vxl_byte> >::operator () (
  vil_rgba<vxl_byte> v, vil_rgba<vxl_byte>& d) const { d.r=v.r, d.g=v.g, d.b=v.b, d.a=v.a; }
#define macro( in )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in,vcl_complex<double> >::operator () (in v, vcl_complex<double>& d) const \
{ d = vcl_complex<double>(double(v),0.0); } \
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in,vcl_complex<float> >::operator () (in v, vcl_complex<float>& d) const \
{ d = vcl_complex<float>(float(v),0.0f); }
macro( vxl_byte )
macro( vxl_sbyte )
macro( vxl_int_16 )
macro( vxl_uint_16 )
macro( vxl_int_32 )
macro( vxl_uint_32 )
#if VXL_HAS_INT_64
macro( vxl_int_64 )
macro( vxl_uint_64 )
#endif
#undef macro
#define macro( out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<vcl_complex<double>,out >::operator () (vcl_complex<double> d, out& v) const \
{ v = (out)(d.real()); } \
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<vcl_complex<float>,out >::operator () (vcl_complex<float> d, out& v) const \
{ v = (out)(d.real()); }
macro( vxl_byte )
macro( vxl_sbyte )
macro( vxl_int_16 )
macro( vxl_uint_16 )
macro( vxl_int_32 )
macro( vxl_uint_32 )
#if VXL_HAS_INT_64
macro( vxl_int_64 )
macro( vxl_uint_64 )
#endif
#undef macro
#endif //DOXYGEN_SHOULD_SKIP_THIS
// declare general case in case anyone needs something weird.

#if defined(VCL_VC)
// Visual C++ intentionally warns when a non-bool is assigned or
// cast to a bool. Since the following cast is valid, we suppress the
// warning.
# pragma warning( push )
# pragma warning( disable : 4800 )
#endif

template <class In, class Out>
inline void vil_convert_cast_pixel<In, Out>::operator () (In v, Out &d) const
{
  d = static_cast<Out>(v);
}

#if defined(VCL_VC)
# pragma warning( pop )
#endif


//: Cast one pixel type to another (with rounding).
// There must be a cast operator from inP to outP
//
// If the two pixel types are the same, the destination may only be a shallow
// copy of the source.
// \relates vil_image_view
template <class inP, class outP>
inline void vil_convert_cast(const vil_image_view<inP >&src,
                             vil_image_view<outP >&dest)
{
  if (vil_pixel_format_of(inP()) == vil_pixel_format_of(outP()))
    dest = src;
  else
    vil_transform2(src, dest, vil_convert_cast_pixel<inP, outP>());
}

#if 0 // TODO ?

//: Cast the unknown pixel type to the known one, if possible.
//
// Will call the other vil_convert_case to do the actual
// conversion. For template instantiation reasons, this will only
// convert to a scalar type, not an RGB or RGBA type.
// However, the destination image will have a plane step of 1 if the source
// image is RGB or RGBA, i.e., the pixel layout will still be RGBRGB...
//
// \relates vil_image_view
//
template <class outP>
inline void vil_convert_cast(const vil_image_view_base_sptr&src, vil_image_view<outP >&dest)
{
#define docase(T) \
   case T: \
     vil_convert_cast( vil_image_view< typename vil_pixel_format_type_of<T >::component_type >(src), dest );\
     break

  switch ( src->pixel_format() )
  {
    docase( VIL_PIXEL_FORMAT_UINT_32 );
    docase( VIL_PIXEL_FORMAT_INT_32 );
    docase( VIL_PIXEL_FORMAT_UINT_16 );
    docase( VIL_PIXEL_FORMAT_INT_16 );
    docase( VIL_PIXEL_FORMAT_BYTE );
    docase( VIL_PIXEL_FORMAT_SBYTE );
    docase( VIL_PIXEL_FORMAT_FLOAT );
    docase( VIL_PIXEL_FORMAT_DOUBLE );
    docase( VIL_PIXEL_FORMAT_BOOL );

    docase( VIL_PIXEL_FORMAT_RGB_UINT_32 );
    docase( VIL_PIXEL_FORMAT_RGB_INT_32 );
    docase( VIL_PIXEL_FORMAT_RGB_UINT_16 );
    docase( VIL_PIXEL_FORMAT_RGB_INT_16 );
    docase( VIL_PIXEL_FORMAT_RGB_BYTE );
    docase( VIL_PIXEL_FORMAT_RGB_SBYTE );
    docase( VIL_PIXEL_FORMAT_RGB_FLOAT );
    docase( VIL_PIXEL_FORMAT_RGB_DOUBLE );

    docase( VIL_PIXEL_FORMAT_RGBA_UINT_32 );
    docase( VIL_PIXEL_FORMAT_RGBA_INT_32 );
    docase( VIL_PIXEL_FORMAT_RGBA_UINT_16 );
    docase( VIL_PIXEL_FORMAT_RGBA_INT_16 );
    docase( VIL_PIXEL_FORMAT_RGBA_BYTE );
    docase( VIL_PIXEL_FORMAT_RGBA_SBYTE );
    docase( VIL_PIXEL_FORMAT_RGBA_FLOAT );
    docase( VIL_PIXEL_FORMAT_RGBA_DOUBLE );

    docase( VIL_PIXEL_FORMAT_COMPLEX_FLOAT );
    docase( VIL_PIXEL_FORMAT_COMPLEX_DOUBLE );

    default:
     ;
  }
#undef docase
}

#endif // 0

//: Performs rounding between different pixel types.
template <class In, class Out>
class vil_convert_round_pixel
{
 public:
  void operator () (In v, Out &d) const;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// deal with conversions from floating point types to some compounds
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () ( \
  in v, out& d) const { \
  d.r = (out::value_type)(v.r+0.5); \
  d.g = (out::value_type)(v.g+0.5); \
  d.b = (out::value_type)(v.b+0.5); }

macro( vil_rgb<vxl_byte> , vil_rgb<float> )
macro( vil_rgb<vxl_byte> , vil_rgb<double> )
macro( vil_rgb<vxl_sbyte> , vil_rgb<float> )
macro( vil_rgb<vxl_sbyte> , vil_rgb<double> )
macro( vil_rgb<vxl_int_16> , vil_rgb<float> )
macro( vil_rgb<vxl_int_16> , vil_rgb<double> )
macro( vil_rgb<vxl_uint_16> , vil_rgb<float> )
macro( vil_rgb<vxl_uint_16> , vil_rgb<double> )
macro( vil_rgb<vxl_int_32> , vil_rgb<float> )
macro( vil_rgb<vxl_int_32> , vil_rgb<double> )
macro( vil_rgb<vxl_uint_32> , vil_rgb<float> )
macro( vil_rgb<vxl_uint_32> , vil_rgb<double> )
#if VXL_HAS_INT_64
macro( vil_rgb<vxl_int_64> , vil_rgb<float> )
macro( vil_rgb<vxl_int_64> , vil_rgb<double> )
macro( vil_rgb<vxl_uint_64> , vil_rgb<float> )
macro( vil_rgb<vxl_uint_64> , vil_rgb<double> )
#endif
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) \
  const { \
  d.r = (out::value_type)(v.r); \
  d.g = (out::value_type)(v.g); \
  d.b = (out::value_type)(v.b); }
macro( vil_rgb<float> , vil_rgb<float> )
macro( vil_rgb<double> , vil_rgb<double> )
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) \
  const { \
  d.r = (out::value_type)(v.r+0.5); \
  d.g = (out::value_type)(v.g+0.5); \
  d.b = (out::value_type)(v.b+0.5); \
  d.a = (out::value_type)(v.a+0.5); }
macro( vil_rgba<vxl_byte> , vil_rgba<float> )
macro( vil_rgba<vxl_byte> , vil_rgba<double> )
macro( vil_rgba<vxl_sbyte> , vil_rgba<float> )
macro( vil_rgba<vxl_sbyte> , vil_rgba<double> )
macro( vil_rgba<vxl_int_16> , vil_rgba<float> )
macro( vil_rgba<vxl_int_16> , vil_rgba<double> )
macro( vil_rgba<vxl_uint_16> , vil_rgba<float> )
macro( vil_rgba<vxl_uint_16> , vil_rgba<double> )
macro( vil_rgba<vxl_int_32> , vil_rgba<float> )
macro( vil_rgba<vxl_int_32> , vil_rgba<double> )
macro( vil_rgba<vxl_uint_32> , vil_rgba<float> )
macro( vil_rgba<vxl_uint_32> , vil_rgba<double> )
#if VXL_HAS_INT_64
macro( vil_rgba<vxl_int_64> , vil_rgba<float> )
macro( vil_rgba<vxl_int_64> , vil_rgba<double> )
macro( vil_rgba<vxl_uint_64> , vil_rgba<float> )
macro( vil_rgba<vxl_uint_64> , vil_rgba<double> )
#endif
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) \
  const { \
  d.r = (out::value_type)(v.r); \
  d.g = (out::value_type)(v.g); \
  d.b = (out::value_type)(v.b); \
  d.a = (out::value_type)(v.a); }
macro( vil_rgba<float> , vil_rgba<float> )
macro( vil_rgba<double> , vil_rgba<double> )
#undef macro

#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) \
const { d = (out)(v+0.5); }
macro( vxl_byte , float )
macro( vxl_byte , double )
macro( vxl_sbyte , float )
macro( vxl_sbyte , double )
macro( vxl_int_16 , float )
macro( vxl_int_16 , double )
macro( vxl_uint_16 , float )
macro( vxl_uint_16 , double )
macro( vxl_int_32 , float )
macro( vxl_int_32 , double )
macro( vxl_uint_32 , float )
macro( vxl_uint_32 , double )
#if VXL_HAS_INT_64
macro( vxl_int_64 , float )
macro( vxl_int_64 , double )
macro( vxl_uint_64 , float )
macro( vxl_uint_64 , double )
#endif
#undef macro
#endif //DOXYGEN_SHOULD_SKIP_THIS

// declare general case for scalars
template <class In, class Out>
inline void vil_convert_round_pixel<In, Out>::operator () (In v, Out &d) const
{
  d = (Out)(v);
}


//: Convert one pixel type to another with rounding.
// This should only be used to convert scalar pixel types to other scalar
// pixel types, or RGBs to RGBs. This function only rounds in terms of the
// destination type.
//
// If the two pixel types are the same, the destination may only be a
// shallow copy of the source.
// \relates vil_image_view
template <class inP, class outP>
inline void vil_convert_round(const vil_image_view<inP >&src,
                              vil_image_view<outP >&dest)
{
  if (vil_pixel_format_of(inP()) == vil_pixel_format_of(outP()))
    dest = src;
  else
    vil_transform2(src, dest, vil_convert_round_pixel<inP, outP>());
}


//: Convert various rgb types to greyscale, using given weights
template <class inP, class outP>
class vil_convert_rgb_to_grey_pixel
{
  double rw_, gw_, bw_;
 public:
  vil_convert_rgb_to_grey_pixel(double rw, double gw, double bw):
    rw_(rw), gw_(gw), bw_(bw) {}

  void operator() (vil_rgb<inP> v, outP& d) const {
    vil_convert_round_pixel<double,outP>()(rw_*v.r+gw_*v.g+bw_*v.b, d); }
  void operator() (vil_rgba<inP> v, outP& d) const {
    vil_convert_round_pixel<double,outP>()(rw_*v.r+gw_*v.g+bw_*v.b, d); }
};

//: Convert single plane rgb (or rgba) images to greyscale.
// Component types can be different. Rounding will take place if appropriate.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Poynton's Colour FAQ
// http://www.poynton.com/ColorFAQ.html
template <class rgbP, class outP>
inline void vil_convert_rgb_to_grey(const vil_image_view<rgbP >&src,
                                    vil_image_view<outP >&dest,
                                    double rw=0.2125, double gw=0.7154, double bw=0.0721)
{
#if defined(VCL_VC60) || !VCL_HAS_TYPENAME
  vil_convert_rgb_to_grey_pixel<rgbP::value_type, outP> func(rw, gw, bw);
#else
  vil_convert_rgb_to_grey_pixel<typename rgbP::value_type, outP>
    func(rw, gw, bw);
#endif
  assert(src.nplanes() == 1);
  vil_transform2(src, dest, func);
}


//: Convert first three planes of src image to grey, assuming rgb.
// Pixel types can be different. Rounding will take place if appropriate.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.poynton.com/ColorFAQ.html
template <class inP, class outP>
inline void vil_convert_planes_to_grey(const vil_image_view<inP>&src,
                                       vil_image_view<outP>&dest,
                                       double rw=0.2125, double gw=0.7154, double bw=0.0721)
{
  assert(src.nplanes() >= 3);
  assert(vil_pixel_format_num_components(src.pixel_format()) == 1);
  assert(vil_pixel_format_num_components(dest.pixel_format()) == 1);
  dest.set_size(src.ni(), src.nj(), 1);
  for (unsigned j = 0; j < src.nj(); ++j)
    for (unsigned i = 0; i < src.ni(); ++i)
      vil_convert_round_pixel<double,outP>()(
        src(i,j,0)*rw + src(i,j,1)*gw + src(i,j,2)*bw, dest(i,j));
}


//: Convert src to byte image dest by stretching to range [0,255]
// \relates vil_image_view
template <class T>
inline void vil_convert_stretch_range(const vil_image_view<T>& src,
                                      vil_image_view<vxl_byte>& dest)
{
  T min_b,max_b;
  vil_math_value_range(src,min_b,max_b);
  double a = -1.0*double(min_b);
  double b = 0.0;
  if (max_b-min_b >0) b = 255.0/(max_b-min_b);
  dest.set_size(src.ni(), src.nj(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
         dest(i,j,p) = static_cast<vxl_byte>( b*( src(i,j,p)+ a ) );
}


// It doesn't seem sensible to write a general stretch
// conversion function from any type to any type.
// The individual pixel transfer function has to perform
// multiplications which have to be done in double
// to provide both the range and precision. You may as well
// leave the image in double, and convert it again later.

//: Convert src to double image dest by stretching to range [lo,hi]
template <class inP>
inline void vil_convert_stretch_range(const vil_image_view<inP>& src,
                                      vil_image_view<double>& dest,
                                      double lo, double hi)
{
  inP min_b=0, max_b=0;
  vil_math_value_range(src,min_b,max_b);
  double b = 0.0;
  if (max_b-min_b >0)
    b = static_cast<double>(hi-lo)/static_cast<double>(max_b-min_b);
  double a = -1.0*min_b*b + lo;
  dest.set_size(src.ni(), src.nj(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
        dest(i,j,p) =  b*src(i,j,p) + a;
}


//: Cast the unknown pixel type to the known one.
//
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// If you need a
// multi-component view, then call this to get the corresponding
// multi-planar view, and do a second (cheap) conversion.
// The input image's storage arrangement may not be preserved.
template <class outP>
inline vil_image_view_base_sptr vil_convert_cast(outP /*dummy*/,
                                                 const vil_image_view_base_sptr& src)
{
  if (!src) return vil_image_view_base_sptr();

  vil_image_view_base_sptr dest = new vil_image_view<outP>;
  vil_image_view<outP> & dest_ref = static_cast<vil_image_view<outP> &>(*dest);

  switch ( vil_pixel_format_component_format(src->pixel_format()) )
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro(F , T) \
    case F: vil_convert_cast( vil_image_view<T >(*src), dest_ref ); break

#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64 );
    macro( VIL_PIXEL_FORMAT_INT_64, vxl_int_64 );
#endif
    macro( VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 );
    macro( VIL_PIXEL_FORMAT_INT_32, vxl_int_32 );
    macro( VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16 );
    macro( VIL_PIXEL_FORMAT_INT_16, vxl_int_16 );
    macro( VIL_PIXEL_FORMAT_BYTE, vxl_byte );
    macro( VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte );
    macro( VIL_PIXEL_FORMAT_FLOAT, float );
    macro( VIL_PIXEL_FORMAT_DOUBLE, double );
    macro( VIL_PIXEL_FORMAT_BOOL, bool );
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
    default:
      dest = 0;
  }
  return dest;
}


//: Cast the unknown pixel type to the known one, if possible.
//
// Will call the other vil_convert_cast to do the actual
// conversion. For template instantiation reasons, this will only
// convert to a scalar type, not a RGB or RGBA type. If you need a
// multi-component view, then call this to get the corresponding
// multi-planar view, and do a second (cheap) conversion.
//
// \deprecated Use other vil_convert_cast()
// Can be removed after VXL 1.1.1
template <class outP>
inline void vil_convert_cast(const vil_image_view_base_sptr& src,
                             vil_image_view<outP >&dest)
{
  VXL_DEPRECATED( "void vil_convert_cast(const vil_image_view_base_sptr&,"
    " vil_image_view<outP >&" );

  switch ( src->pixel_format() )
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro(F , T) \
    case F: vil_convert_cast( vil_image_view<T >(src), dest ); break;

#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64 )
    macro( VIL_PIXEL_FORMAT_INT_64, vxl_int_64 )
#endif
    macro( VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32 )
    macro( VIL_PIXEL_FORMAT_INT_32, vxl_int_32 )
    macro( VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16 )
    macro( VIL_PIXEL_FORMAT_INT_16, vxl_int_16 )
    macro( VIL_PIXEL_FORMAT_BYTE, vxl_byte )
    macro( VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte )
    macro( VIL_PIXEL_FORMAT_FLOAT, float )
    macro( VIL_PIXEL_FORMAT_DOUBLE, double )
    macro( VIL_PIXEL_FORMAT_BOOL, bool )
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS

    // Skip the RGB type conversions because the vil_convert_cast are
    // not complete. For example, a cast from vxl_uint_16 to
    // vil_rgb<vxl_uint_32> is not defined.
    default:
      dest.clear();
  }
}


//: Convert an image of any pixel type to another with rounding.
// This should only be used to convert to scalar
// pixel types. This function only rounds in terms of the
// destination type.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
//
// If the input image already has outP as its pixel type, the destination
// may only be a shallow copy of the source.
// outP should be a scalar pixel type.
// The input image's storage arrangement may not be preserved.
template <class outP>
inline vil_image_view_base_sptr vil_convert_round(
  outP /*dummy*/, const vil_image_view_base_sptr &src)
{
  assert(vil_pixel_format_num_components(vil_pixel_format_of(outP()))==1);

  if (!src) return vil_image_view_base_sptr();

  if (vil_pixel_format_component_format(src->pixel_format()) ==
      vil_pixel_format_of(outP()))
    return src;

  vil_image_view_base_sptr dest = new vil_image_view<outP >;
  vil_image_view<outP > &dest_ref = static_cast<vil_image_view<outP >&>(*dest);

  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    vil_image_view<T > src1 = src; \
    vil_transform2(src1, dest_ref, vil_convert_round_pixel<T , outP>()); \
    break; }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
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
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
  default: dest=0;
  }
  return dest;
}


//: Force data to be suitable for viewing as multi component view.
// The output data will have values from different planes but the same
// pixel location stored in adjacent memory locations. After using this
// function on an input with 3 planes, an assignment to a
// vil_image_view<vil_rgb<T> > will always work.
// The input image's scalar pixel type will be preserved.
inline vil_image_view_base_sptr vil_convert_to_component_order(
  const vil_image_view_base_sptr& src)
{
  if (!src) return vil_image_view_base_sptr();

  vil_image_view_base_sptr dest;

  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T )\
   case F: { \
    vil_image_view<T > src_ref(src); \
    if (!src_ref) return vil_image_view_base_sptr(); \
    if (src_ref.planestep()==1) return src; \
    const unsigned ni=src->ni(), nj=src->nj(), nplanes=src->nplanes(); \
    vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T), \
                                                       vil_pixel_format_component_format(F)); \
    dest = new vil_image_view<T >(chunk, reinterpret_cast<T*>(chunk->data()), \
                                  ni, nj, nplanes, nplanes, nplanes*ni, 1); \
    vil_image_view<T > & dest_ref = static_cast<vil_image_view<T >&>(*dest); \
    vil_copy_reformat(src_ref, dest_ref); \
    break; }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
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
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS

    default: dest=0;
  }
  return dest;
}


//: Create a greyscale image from any image src.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance. e.g.
// \verbatim
// vil_image_view<float> input =
//   vil_convert_to_grey_using_average(vil_load(filename), float());
// \endverbatim
// If you have a image_view of known pixel_type then you should use one of
// the other vil_convert functions.
// The output may be a reconfigured view of the input.
// \deprecated Use other vil_convert_to_grey_using_average()
template <class outP>
inline vil_image_view<outP> vil_convert_to_grey_using_average(
  const vil_image_view_base_sptr &src, outP /*dummy*/)
{
  VXL_DEPRECATED( "vil_convert_to_grey_using_average<outP>("
                  "const vil_image_view_base_sptr &, outP)" );

  // Check output is scalar component image.
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src) return vil_image_view<outP>();

  // try to do it quickly
  if (vil_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil_image_view<outP>(src);

  // create output view
  vil_image_view<outP> dest;

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    vil_image_view<T > src1 = *src; \
    vil_math_mean_over_planes(src1, dest, double()); \
    break; }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
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
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    dest.clear();
  }
  return dest;
}


//: Create a greyscale image of specified pixel type from any image src.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance. e.g.
// \verbatim
// vil_image_view<float> input = vil_convert_cast(
//   convert_to_grey_using_average(vil_load(filename)), float());
// \endverbatim
// The output may be a reconfigured view of the input.
// The input image's pixel type and storage arrangement may not be preserved.
inline vil_image_view_base_sptr vil_convert_to_grey_using_average(
  const vil_image_view_base_sptr &src)
{
  if (!src) return vil_image_view_base_sptr();

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    /* try to do it quickly */ \
    if (src->nplanes() == 1 && \
        vil_pixel_format_component_format(src->pixel_format())==1) \
      return src; \
    /* create output view */ \
    vil_image_view<T > dest; \
    vil_image_view<T > src1 = *src; \
    vil_math_mean_over_planes(src1, dest, double()); \
    return vil_image_view_base_sptr(new vil_image_view<T >(dest)); }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64 && !defined(VCL_VC60)
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    return vil_image_view_base_sptr();
  }
}


//: Create a greyscale image from any image src.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// The output may be a reconfigured view of the input.
// The input image's pixel type and storage arrangement may not be preserved.
inline vil_image_view_base_sptr vil_convert_to_grey_using_rgb_weighting(
  double rw, double gw, double bw, const vil_image_view_base_sptr &src)
{
  if (!src)
    return vil_image_view_base_sptr();

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    /* try to do it quickly */ \
    if (src->nplanes() == 1 && \
        vil_pixel_format_num_components(src->pixel_format()) == 1) \
      return vil_image_view_base_sptr(src); \
    vil_image_view<T > src1 = src; \
    vil_image_view<double> dest1; \
    vil_convert_planes_to_grey(src1, dest1, rw, gw, bw); \
    vil_image_view<T > dest; \
    vil_convert_round(dest1,dest); \
    return vil_image_view_base_sptr(new vil_image_view<T >(dest)); }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64 && !defined(VCL_VC60)
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    return vil_image_view_base_sptr();
  }
}

//: Create a greyscale image from any image src using default weights.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// The output may be a reconfigured view of the input.
// The input image's pixel type and storage arrangement may not be preserved.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Poynton's Colour FAQ
// http://www.poynton.com/ColorFAQ.html

inline vil_image_view_base_sptr vil_convert_to_grey_using_rgb_weighting(
  const vil_image_view_base_sptr &src)
{
  return vil_convert_to_grey_using_rgb_weighting(0.2125, 0.7154, 0.0721, src);
}


//: Create a greyscale image of specified pixel type from any image src.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// The output may be a reconfigured view of the input.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Poynton's Colour FAQ
// http://www.poynton.com/ColorFAQ.html
// \deprecated Use other version of vil_convert_to_grey_using_rgb_weighting
template <class outP>
inline vil_image_view<outP> vil_convert_to_grey_using_rgb_weighting(
                          const vil_image_view_base_sptr &src,
                          outP /*dummy*/,
                          double rw=0.2125,
                          double gw=0.7154,
                          double bw=0.0721)
{
  VXL_DEPRECATED( "vil_convert_to_grey_using_rgb_weighting<outP>("
                  "const vil_image_view_base_sptr &, outP)" );

  // Check output is scalar component image.
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src) return vil_image_view<outP>();

  // try to do it quickly
  if (vil_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil_image_view<outP>(src);

  // create output view
  vil_image_view<outP> dest;

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    vil_image_view<T > src1 = src; \
    vil_image_view<double> dest1; \
    vil_convert_planes_to_grey(src1, dest1, rw, gw, bw); \
    vil_convert_round(dest1,dest); \
    break; }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
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
   // Don't even want to think about rgb<complex<float> >
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    dest.clear();
  }
  return dest;
}

//: Create an n plane image from any image src.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type or number of planes in advance.
// If the input images have too many planes, the higher planes will be
// truncated. If the input image has too few planes, the new planes will be
// copies of the first plane.
//
// The output may be a shallow copy of the input.
// The input image's storage arrangement may not be preserved.
//
// This function works on scalar pixel types only, however it can be used to
// produce an rgb image as in the following example
// \verbatim
// vil_image_view<vil_rgb<float> > =
//   vil_convert_cast(
//     vil_convert_to_component_order(
//       vil_convert_to_n_planes(
//         vil_load(filename),
//       3),
//     ),
//   float());
// \endverbatim
inline vil_image_view_base_sptr vil_convert_to_n_planes(
  unsigned n_planes, const vil_image_view_base_sptr &src)
{
  if (!src || n_planes == 0)
    return vil_image_view_base_sptr();


  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
 #define macro( F, T ) \
   case F: { \
    vil_image_view<T > src_ref = src; \
    if (!src_ref) return vil_image_view_base_sptr(); \
    /* try to do it quickly 1 */ \
    if (src_ref.nplanes() >= n_planes) /* reduce number of planes */ \
      return vil_image_view_base_sptr( new vil_image_view<T >( \
          vil_planes(vil_image_view<T > (src),0,1,n_planes) )); \
    else { /* expand number of planes with copying */ \
      vil_image_view_base_sptr dest = new vil_image_view<T >(src_ref.ni(), src_ref.nj(), n_planes); \
      vil_image_view<T > & dest_ref = static_cast<vil_image_view<T >&>(*dest); \
      vil_image_view<T > dest_slices = vil_planes(dest_ref, 0, 1, src_ref.nplanes()); \
      vil_copy_reformat(src_ref, dest_slices); \
      vil_image_view<T > src_slice(vil_plane(src_ref, 0)); \
      for (unsigned i=src_ref.nplanes(); i<n_planes; ++i) { \
        dest_slices = vil_plane(dest_ref, i); \
        vil_copy_reformat(src_slice,  dest_slices); } \
      return dest; } }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
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
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    return vil_image_view_base_sptr();
  }
}


//: Create an image of the desired type by stretching the range to fit.
// This function is designed to be used with vil_load or
// vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// In the case of floating point output pixels the range is set to [0,1]
// The input image's storage arrangement may not be preserved.
//
// This function works on scalar pixel types only. You can convert the image
// to rgb using a cheap assignment afterwards.
template <class outP>
inline vil_image_view_base_sptr vil_convert_stretch_range(
  outP /*dummy*/, const vil_image_view_base_sptr &src)
{
  // Check that input isn't trying to produce multi-component pixels
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src)
    return vil_image_view_base_sptr();

  double hi,lo;

  if (vcl_numeric_limits<outP>::is_integer)
  {
    hi = vcl_numeric_limits<outP>::max()+0.999;
    lo = vcl_numeric_limits<outP>::min();
  }
  else
  {
    hi=1.0;
    lo=0.0;
  }

  vil_image_view_base_sptr dest = new vil_image_view<outP>;
  vil_image_view<outP> & dest_ref = static_cast<vil_image_view<outP> &>(*dest);
  vil_image_view<double> inter;
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
    vil_image_view<T> src_ref = src; \
    if (!src_ref) return vil_image_view_base_sptr(); \
    vil_convert_stretch_range(src_ref, inter, lo, hi); \
    vil_convert_cast(inter, dest_ref); \
    break; }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64 && !defined(VCL_VC60)
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default:
    dest_ref.clear();
  }
  return dest;
}


#endif // vil_convert_h_
