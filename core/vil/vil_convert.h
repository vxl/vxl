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

#include <vcl_cassert.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>


//: Performs conversion between different pixel types.
template <class In, class Out>
class vil_convert_cast_pixel
{
 public:
  void operator () (In v, Out &d) const;
};

// deal with conversions from floating point types to some compounds
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in, out >::operator () (in v, out& d) const \
{ d.r = d.g = d.b = (out::value_type)v; }
macro( vxl_byte , vil_rgb<vxl_byte> )
macro( float , vil_rgb<vxl_byte> )
macro( double , vil_rgb<vxl_byte> )
macro( vxl_sbyte , vil_rgb<vxl_sbyte> )
macro( float , vil_rgb<vxl_sbyte> )
macro( double , vil_rgb<vxl_sbyte> )
macro( vxl_int_16 , vil_rgb<vxl_int_16> )
macro( float , vil_rgb<vxl_int_16> )
macro( double , vil_rgb<vxl_int_16> )
macro( vxl_uint_16 , vil_rgb<vxl_uint_16> )
macro( float , vil_rgb<vxl_uint_16> )
macro( double , vil_rgb<vxl_uint_16> )
macro( vxl_int_32 , vil_rgb<vxl_int_32> )
macro( float , vil_rgb<vxl_int_32> )
macro( double , vil_rgb<vxl_int_32> )
macro( vxl_uint_32 , vil_rgb<vxl_uint_32> )
macro( float , vil_rgb<vxl_uint_32> )
macro( double , vil_rgb<vxl_uint_32> )
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<in, out >::operator () (in v, out& d) const \
{ d.r = d.g = d.b = (out::value_type)v; d.a=1; }
macro( vxl_byte , vil_rgba<vxl_byte> )
macro( float , vil_rgba<vxl_byte> )
macro( double , vil_rgba<vxl_byte> )
macro( vxl_sbyte , vil_rgba<vxl_sbyte> )
macro( float , vil_rgba<vxl_sbyte> )
macro( double , vil_rgba<vxl_sbyte> )
macro( vxl_int_16 , vil_rgba<vxl_int_16> )
macro( float , vil_rgba<vxl_int_16> )
macro( double , vil_rgba<vxl_int_16> )
macro( vxl_uint_16 , vil_rgba<vxl_uint_16> )
macro( float , vil_rgba<vxl_uint_16> )
macro( double , vil_rgba<vxl_uint_16> )
macro( vxl_int_32 , vil_rgba<vxl_int_32> )
macro( float , vil_rgba<vxl_int_32> )
macro( double , vil_rgba<vxl_int_32> )
macro( vxl_uint_32 , vil_rgba<vxl_uint_32> )
macro( float , vil_rgba<vxl_uint_32> )
macro( double , vil_rgba<vxl_uint_32> )
#undef macro
#define macro( inout )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_cast_pixel<inout, inout >::operator () (inout v, inout& d) const { d=v; }
macro( vxl_byte )
macro( vxl_sbyte )
macro( vxl_uint_16 )
macro( vxl_int_16 )
macro( vxl_uint_32 )
macro( vxl_int_32 )
macro( float )
macro( double )
macro( vil_rgb<vxl_byte> )
macro( vil_rgba<vxl_byte> )
#undef macro

// declare general case in case anyone needs something weird.
template <class In, class Out>
inline void vil_convert_cast_pixel<In, Out>::operator () (In v, Out &d) const { d = static_cast<Out>(v); }


//: Cast one pixel type to another (with rounding).
// There must be a cast operator from inP to outP
//
// If the two pixel types are the same, the destination may only be a shallow copy
// of the source.
// \relates vil_image_view
template <class inP, class outP>
inline void vil_convert_cast(const vil_image_view<inP >&src, vil_image_view<outP >&dest)
{
  if (vil_pixel_format_of(inP()) == vil_pixel_format_of(outP()))
    dest = src;
  else
    vil_transform2(src, dest, vil_convert_cast_pixel<inP, outP>());
}


//: Cast the unknown pixel type to the known one, if possible.
//
// Will call the other vil_convert_case to do the actual
// conversion. For template instantiation reasons, this will only
// convert to a scalar type, not a RGB or RGBA type. If you need a
// multi-component view, then call this to get the corresponding
// multi-planar view, and do a second (cheap) conversion.
//
// \relates vil_image_view_base
//
template <class outP>
inline void vil_convert_cast(const vil_image_view_base_sptr&src, vil_image_view<outP >&dest)
{
#define docase(T) \
   case T: \
     vil_convert_cast( vil_image_view< vil_pixel_format_type_of< T >::type >( src ), dest );\
     break \

  switch( src->pixel_format() ) {
    docase( VIL_PIXEL_FORMAT_UINT_32 );
    docase( VIL_PIXEL_FORMAT_INT_32 );
    docase( VIL_PIXEL_FORMAT_UINT_16 );
    docase( VIL_PIXEL_FORMAT_INT_16 );
    docase( VIL_PIXEL_FORMAT_BYTE );
    docase( VIL_PIXEL_FORMAT_SBYTE );
    docase( VIL_PIXEL_FORMAT_FLOAT );
    docase( VIL_PIXEL_FORMAT_DOUBLE );

    docase( VIL_PIXEL_FORMAT_BOOL );

    // Skip the RGB type conversions because the vil_convert_cast are
    // not complete. For example, a cast from vxl_uint_16 to
    // vil_rgb<vxl_uint_32> is not defined.

    //docase( VIL_PIXEL_FORMAT_RGB_UINT_32 );
    //docase( VIL_PIXEL_FORMAT_RGB_INT_32 );
    //docase( VIL_PIXEL_FORMAT_RGB_UINT_16 );
    //docase( VIL_PIXEL_FORMAT_RGB_INT_16 );
    //docase( VIL_PIXEL_FORMAT_RGB_BYTE );
    //docase( VIL_PIXEL_FORMAT_RGB_SBYTE );
    //docase( VIL_PIXEL_FORMAT_RGB_FLOAT );
    //docase( VIL_PIXEL_FORMAT_RGB_DOUBLE );

    //docase( VIL_PIXEL_FORMAT_RGBA_UINT_32 );
    //docase( VIL_PIXEL_FORMAT_RGBA_INT_32 );
    //docase( VIL_PIXEL_FORMAT_RGBA_UINT_16 );
    //docase( VIL_PIXEL_FORMAT_RGBA_INT_16 );
    //docase( VIL_PIXEL_FORMAT_RGBA_BYTE );
    //docase( VIL_PIXEL_FORMAT_RGBA_SBYTE );
    //docase( VIL_PIXEL_FORMAT_RGBA_FLOAT );
    //docase( VIL_PIXEL_FORMAT_RGBA_DOUBLE );
    default:
      ;
  }
#undef docase
}


//: Performs rounding between different pixel types.
template <class In, class Out>
class vil_convert_round_pixel
{
 public:
  void operator () (In v, Out &d) const;
};

// deal with conversions from floating point types to some compounds
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) const { \
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
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) const { \
  d.r = (out::value_type)(v.r); \
  d.g = (out::value_type)(v.g); \
  d.b = (out::value_type)(v.b); }
macro( vil_rgb<float> , vil_rgb<float> )
macro( vil_rgb<double> , vil_rgb<double> )
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) const { \
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
#undef macro
#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) const { \
  d.r = (out::value_type)(v.r); \
  d.g = (out::value_type)(v.g); \
  d.b = (out::value_type)(v.b); \
  d.a = (out::value_type)(v.a); }
macro( vil_rgba<float> , vil_rgba<float> )
macro( vil_rgba<double> , vil_rgba<double> )
#undef macro

#define macro( in , out )\
VCL_DEFINE_SPECIALIZATION \
inline void vil_convert_round_pixel<in, out >::operator () (in v, out& d) const { \
  d = (out)(v+0.5); }
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
#undef macro


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
// If the two pixel types are the same, the destination may only be a shallow copy
// of the source.
// \relates vil_image_view
template <class inP, class outP>
inline void vil_convert_round(const vil_image_view<inP >&src, vil_image_view<outP >&dest)
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
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class rgbP, class outP>
inline void vil_convert_rgb_to_grey(const vil_image_view<rgbP >&src, vil_image_view<outP >&dest,
                                    double rw=0.2125, double gw=0.7154, double bw = 0.0721)
{
#if VCL_VC60 || !VCL_HAS_TYPENAME
  vil_convert_rgb_to_grey_pixel<rgbP::value_type, outP> func(rw, gw, bw);
#else
  vil_convert_rgb_to_grey_pixel<typename rgbP::value_type, outP> func(rw, gw, bw);
#endif
  assert(src.nplanes() == 1);
  vil_transform2(src, dest, func);
}


//: Convert first three planes of src image to grey, assuming rgb.
// Pixel types can be different. Rounding will take place if appropriate.
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class inP, class outP>
inline void vil_convert_planes_to_grey(const vil_image_view<inP>&src,
                                       vil_image_view<outP>&dest,
                                       double rw=0.2125, double gw=0.7154,
                                       double bw = 0.0721)
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
         dest(i,j,p) = (vxl_byte)( b*( src(i,j,p)+ a ) );
}

//: Create a greyscale image of specified pixel type from any image src.
// This function is designed to be used with vil_load or vil_image_resource::get_view()
// where you do not know the pixel type in advance. e.g.
// \verbatim
// vil_image_view<float> input = vil_convert_to_grey_using_average(vil_load(filename), float());
// \endverbatim
// If you have a image_view of known pixel_type then you should use one of the other vil_convert functions.
// The output may be a reconfigured view of the input.
template <class outP>
inline vil_image_view<outP> vil_convert_to_grey_using_average(
  const vil_image_view_base_sptr &src, outP /*dummy*/)
{
  // Check output is scalar component image.
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src) return vil_image_view<outP>();

  // try to do it quickly
  if (vil_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil_image_view<outP>(src);

  // create output view
  if (!src) return vil_image_view<outP>();
  vil_image_view<outP> dest;

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
  case F: { \
      vil_image_view<T > src1 = *src; \
      vil_image_view<double> dest1; \
      vil_math_mean_over_planes(src1, dest1); \
      vil_convert_round(dest1,dest); \
      break; }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    dest.clear();
  }
  return dest;
}


//: Create a greyscale image of specified pixel type from any image src.
// This function is designed to be used with vil_load or vil_image_resource::get_view()
// where you do not know the pixel type in advance.
// The output may be a reconfigured view of the input.This
//
// Default weights convert from linear RGB to CIE luminance assuming a
// modern monitor.  See Charles Pontyon's Colour FAQ
// http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html
template <class outP>
inline vil_image_view<outP> vil_convert_to_grey_using_rgb_weighting(
                          const vil_image_view_base_sptr &src,
                          outP /*dummy*/,
                          double rw=0.2125,
                          double gw=0.7154,
                          double bw=0.0721)
{
  // Check output is scalar component image.
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src) return vil_image_view<outP>();

  // try to do it quickly
  if (vil_pixel_format_of(outP()) == src->pixel_format() && src->nplanes() == 1)
    return vil_image_view<outP>(src);

  // create output view
  if (!src) return vil_image_view<outP>();
  vil_image_view<outP> dest;

  // convert via vil_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
  case F: { \
      vil_image_view<T > src1 = src; \
      vil_image_view<double> dest1; \
      vil_convert_planes_to_grey(src1, dest1, rw, gw, bw); \
      vil_convert_round(dest1,dest); \
      break; }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
    dest.clear();
  }
  return dest;
}


#endif // vil_convert_h_
