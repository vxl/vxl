// This is mul/vil3d/vil3d_convert.h
#ifndef vil3d_convert_h_
#define vil3d_convert_h_
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
// a vil3d_image_view<T> to a vil3d_image_view<T>,
// the others take an inknown pixel type, using a
// vil3d_image_view_base_sptr. The basic conversion
// operations (e.g. casting, rounding) are available in both types.
// All of the conversions attempt to find shortcuts, so the output
// may be a reconfigured, or shallow copy of the input.
//
// \par vil3d_convert with explicit pixel types
// These are useful when you have two vil3d_image_view objects you want
// to convert between. You can use them in templates where the pixel
// type is one of the template parameters. These functions
// may create a shallow copy of the input if the types match too save
// unnecessary work.
// - vil3d_convert_cast
// - vil3d_convert_round
// - vil3d_convert_stretch_range
//
// \par vil3d_convert with unknown pixel types
// These functions are useful when taking an image from vil3d_load
// of vil3d_image_resource::get_view(), where you may not know the
// pixel type in advance, but want to force the image into a
// particular pixel type.
// - vil3d_convert_cast
// - vil3d_convert_round
// - vil3d_convert_stretch_range
// - vil3d_convert_to_n_planes
// In general these functions expect to take scalar pixel images as
// inputs. Even though many of these functions could return a known
// pixel-typed image, they all return a vil3d_image_view_base_sptr,
// so that the functions can be strung along
//

// Note that these vil3d_convert_..( vil3d_image_view_base_sptr ) functions
// are provided as a convenience for users of vil3d_load and
// vil3d_image_resource::get_view(). Their existence should not suggest
// that it is sensible to use a vil3d_image_view_base_sptr as storage,
// nor that it is a good idea to write a functions that
// take or return a vil3d_image_view_base_sptr. If you need a
// pixel-type-agnostic image container then use a vil3d_image_resource_sptr

// It may be a good idea to provide vil3d_image_resource_sptr based
// vil3d_converts as well.

#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vil/vil_convert.h>
#include <vil3d/vil3d_transform.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_plane.h>
#include <vil3d/vil3d_copy.h>


//: Cast one pixel type to another (with rounding).
// There must be a cast operator from inP to outP
//
// If the two pixel types are the same, the destination may only be a shallow
// copy of the source.
// \relates vil3d_image_view
template <class inP, class outP>
inline void vil3d_convert_cast(const vil3d_image_view<inP >&src,
                               vil3d_image_view<outP >&dest)
{
  if (vil_pixel_format_of(inP()) == vil_pixel_format_of(outP()))
    dest = src;
  else
    vil3d_transform2(src, dest, vil_convert_cast_pixel<inP, outP>());
}


//: Convert one pixel type to another with rounding.
// This should only be used to convert scalar pixel types to other scalar
// pixel types, or RGBs to RGBs. This function only rounds in terms of the
// destination type.
//
// If the two pixel types are the same, the destination may only be a
// shallow copy of the source.
// \relates vil3d_image_view
template <class inP, class outP>
inline void vil3d_convert_round(const vil3d_image_view<inP >&src,
                                vil3d_image_view<outP >&dest)
{
  if (vil_pixel_format_of(inP()) == vil_pixel_format_of(outP()))
    dest = src;
  else
    vil3d_transform2(src, dest, vil_convert_round_pixel<inP, outP>());
}


//: Convert src to byte image dest by stretching to range [0,255]
// \relates vil3d_image_view
template <class T>
inline void vil3d_convert_stretch_range(const vil3d_image_view<T>& src,
                                        vil3d_image_view<vxl_byte>& dest)
{
  T min_b,max_b;
  vil3d_math_value_range(src,min_b,max_b);
  double a = -1.0*double(min_b);
  double b = 0.0;
  if (max_b-min_b >0) b = 255.0/(max_b-min_b);
  dest.set_size(src.ni(), src.nj(), src.nk(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned k = 0; k < src.nk(); ++k)
      for (unsigned j = 0; j < src.nj(); ++j)
        for (unsigned i = 0; i < src.ni(); ++i)
           dest(i,j,k,p) = static_cast<vxl_byte>( b*( src(i,j,k,p)+ a ) );
}


// It doesn't seem sensible to write a general stretch
// conversion function from any type to any type.
// The individual pixel transfer function has to perform
// multiplications which have to be done in double
// to provide both the range and precision. You may as well
// leave the image in double, and convert it again later.

//: Convert src to double image dest by stretching to range [lo,hi]
template <class inP>
inline void vil3d_convert_stretch_range(const vil3d_image_view<inP>& src,
                                        vil3d_image_view<double>& dest,
                                        double lo, double hi)
{
  inP min_b=0, max_b=0;
  vil3d_math_value_range(src,min_b,max_b);
  double b = 0.0;
  if (max_b-min_b >0)
    b = (hi-lo)/static_cast<double>(max_b-min_b);
  double a = -1.0*min_b*b + lo;
  dest.set_size(src.ni(), src.nj(), src.nk(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned k = 0; k < src.nk(); ++k)
      for (unsigned j = 0; j < src.nj(); ++j)
        for (unsigned i = 0; i < src.ni(); ++i)
          dest(i,j,k,p) =  b*src(i,j,k,p) + a;
}


//: Convert src image<inP> to dest image<double> by stretching 
// input range [src_lo, src_hi] to output range [dest_lo, dest_hi].
// Inputs < src_lo are mapped to dest_lo, and inputs > src_hi to dest_hi.
template <class inP>
inline void vil3d_convert_stretch_range_limited(const vil3d_image_view<inP>& src,
                                                vil3d_image_view<double>& dest,
                                                const inP src_lo, 
                                                const inP src_hi,
                                                const double dest_lo, 
                                                const double dest_hi)
{
  double b = 0.0;

  double ddest = dest_hi - dest_lo;
  double dsrc = static_cast<double>(src_hi - src_lo);  
  double dds = ddest / dsrc;
  
  dest.set_size(src.ni(), src.nj(), src.nk(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned k = 0; k < src.nk(); ++k)
      for (unsigned j = 0; j < src.nj(); ++j)
        for (unsigned i = 0; i < src.ni(); ++i)
        {
          inP s = src(i,j,k,p);
          dest(i,j,k,p) = s<=src_lo ? dest_lo :
                          s>=src_hi ? dest_hi :
                                      dest_lo + dds*static_cast<double>(p-src_lo);
        }
}


//: Cast the unknown pixel type to the known one.
//
// This function is designed to be used with vil3d_load or
// vil3d_image_resource::get_view()
// where you do not know the pixel type in advance.
// If you need a
// multi-component view, then call this to get the corresponding
// multi-planar view, and do a second (cheap) conversion.
// The input image's storage arrangement may not be preserved.
template <class outP>
inline vil3d_image_view_base_sptr vil3d_convert_cast(outP /*dummy*/,
                                                     const vil3d_image_view_base_sptr& src)
{
  if (!src) return vil3d_image_view_base_sptr();

  vil3d_image_view_base_sptr dest = new vil3d_image_view<outP>;
  vil3d_image_view<outP> & dest_ref = static_cast<vil3d_image_view<outP> &>(*dest);

  switch( vil_pixel_format_component_format(src->pixel_format()) )
  {
#define macro(F , T) \
   case F: \
     vil3d_convert_cast( vil3d_image_view<T >( src ), dest_ref );\
     break;

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
    default:
      dest = 0;
  }
  return dest;
}


//: Convert an image of any pixel type to another with rounding.
// This should only be used to convert to scalar
// pixel types. This function only rounds in terms of the
// destination type.
// This function is designed to be used with vil3d_load or
// vil3d_image_resource::get_view()
// where you do not know the pixel type in advance.
//
// If the input image already has outP as its pixel type, the destination
// may only be a shallow copy of the source.
// outP should be a scalar pixel type.
// The input image's storage arrangement may not be preserved.
template <class outP>
inline vil3d_image_view_base_sptr vil3d_convert_round(
  outP /*dummy*/, const vil3d_image_view_base_sptr &src)
{
  assert(vil_pixel_format_num_components(vil_pixel_format_of(outP()))==1);

  if (!src) return vil3d_image_view_base_sptr();

  if (vil_pixel_format_component_format(src->pixel_format()) ==
      vil_pixel_format_of(outP()))
    return src;

  vil3d_image_view_base_sptr dest = new vil3d_image_view<outP >;
  vil3d_image_view<outP > &dest_ref = static_cast<vil3d_image_view<outP >&>(*dest);

  switch(vil_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
   case F: { \
    vil3d_image_view<T > src1 = src; \
    vil3d_transform2(src1, dest_ref, vil_convert_round_pixel<T , outP>()); \
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
   default: dest=0;
  }
  return dest;
}


//: Create a greyscale image of specified pixel type from any image src.
// This function is designed to be used with vil3d_load or
// vil3d_image_resource::get_view()
// where you do not know the pixel type in advance. e.g.
// \verbatim
// vil3d_image_view<float> input = vil3d_convert_cast(
//   convert_to_grey_using_average(vil3d_load(filename)), float());
// \endverbatim
// The output may be a reconfigured view of the input.
// The input image's pixel type and storage arrangement may not be preserved.
inline vil3d_image_view_base_sptr vil3d_convert_to_grey_using_average(
  const vil3d_image_view_base_sptr &src)
{
  if (!src) return vil3d_image_view_base_sptr();

  // convert via vil3d_image_view<double>
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
   case F: { \
    /* try to do it quickly */ \
    if (src->nplanes() == 1 && \
        vil_pixel_format_component_format(src->pixel_format())==1) \
      return src; \
    /* create output view */ \
    vil3d_image_view<T > dest; \
    vil3d_image_view<T > src1 = *src; \
    vil3d_math_mean_over_planes(src1, dest, double()); \
    return vil3d_image_view_base_sptr(new vil3d_image_view<T >(dest)); }
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
    return vil3d_image_view_base_sptr();
  }
}


//: Create an n plane image from any image src.
// This function is designed to be used with vil3d_load or
// vil3d_image_resource::get_view()
// where you do not know the pixel type or number of planes in advance.
// If the input images have too many planes, the higher planes will be
// truncated. If the input image has too few planes, the new planes will be
// copies of the first plane.
//
// The output may be a shallow copy of the input.
// The input image's storage arrangement may not be preserved.
// \endverbatim
inline vil3d_image_view_base_sptr vil3d_convert_to_n_planes(
  unsigned n_planes, const vil3d_image_view_base_sptr &src)
{
  if (!src || n_planes == 0)
    return vil3d_image_view_base_sptr();


  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
 #define macro( F, T ) \
   case F: { \
    vil3d_image_view<T > src_ref = src; \
    if (!src_ref) return vil3d_image_view_base_sptr(); \
    /* try to do it quickly 1 */ \
    if (src_ref.nplanes() >= n_planes)  /* reduce number of planes */ \
      return vil3d_image_view_base_sptr( new vil3d_image_view<T >( \
          vil3d_planes(vil3d_image_view<T > (src),0,1,n_planes) )); \
    else { /* expand number of planes with copying */ \
      vil3d_image_view_base_sptr dest = new vil3d_image_view<T >( \
        src_ref.ni(), src_ref.nj(), src_ref.nk(), n_planes); \
      vil3d_image_view<T > & dest_ref = \
        static_cast<vil3d_image_view<T > &>(*dest); \
      vil3d_image_view<T > dest_slices = \
        vil3d_planes(dest_ref, 0, 1, src_ref.nplanes()); \
      vil3d_copy_reformat(src_ref, dest_slices); \
      vil3d_image_view<T > src_slice(vil3d_plane(src_ref, 0)); \
      for (unsigned i=src_ref.nplanes(); i<n_planes; ++i) { \
        dest_slices = vil3d_plane(dest_ref, i); \
        vil3d_copy_reformat(src_slice,  dest_slices); } \
      return dest;  } } \

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
    return vil3d_image_view_base_sptr();
  }
}


//: Create an image of the desired type by stretching the range to fit.
// This function is designed to be used with vil3d_load or
// vil3d_image_resource::get_view()
// where you do not know the pixel type in advance.
// In the case of floating point output pixels the range is set to [0,1]
// The input image's storage arrangement may not be preserved.
//
// This function works on scalar pixel types only. You can convert the image
// to rgb using a cheap assignment afterwards.
template <class outP>
inline vil3d_image_view_base_sptr vil3d_convert_stretch_range(
  outP /*dummy*/, const vil3d_image_view_base_sptr &src)
{
  // Check that input isn't trying to produce multi-component pixels
  assert (vil_pixel_format_num_components(vil_pixel_format_of(outP())) == 1);

  if (!src)
    return vil3d_image_view_base_sptr();

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

  vil3d_image_view_base_sptr dest = new vil3d_image_view<outP>;
  vil3d_image_view<outP> & dest_ref = static_cast<vil3d_image_view<outP> &>(*dest);
  vil3d_image_view<double> inter;
  switch (vil_pixel_format_component_format(src->pixel_format()))
  {
#define macro( F , T ) \
   case F: { \
    vil3d_image_view<T> src_ref = src; \
    if (!src_ref) return vil3d_image_view_base_sptr(); \
    vil3d_convert_stretch_range(src_ref, inter, lo, hi); \
    vil3d_convert_cast(inter, dest_ref); \
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
    dest_ref.clear();
  }
  return dest;
}


#endif // vil3d_convert_h_
