// This is brl/bbas/bil/bil_convert_to_grey.h
#ifndef bil_convert_to_grey_h_
#define bil_convert_to_grey_h_
//:
// \file
// \brief Convert various resources to grey scale retaining the component pixel type
// \author J.L. Mundy
// a bit too specialized for vil
// also vil uses special weighting for the bands based on the human visual system
// this routine is meant to deal with multi-spectral images where the weighting is equal
//
#include <cassert>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
class bil_convert_resource_to_grey{
 public:
  bil_convert_resource_to_grey()= default;
  void operator ()(vil_image_resource_sptr const& resc, vil_image_resource_sptr& grey_resc, bool apply_mask){
  // handle special tiff RGBA case first (vil prefers to have separate planes not an rgba pixel)
  bool is_tiff = false;
  char const* file_fmt = resc->file_format();
  if(file_fmt){
    std::string fmt(file_fmt);
    is_tiff = (fmt == "tiff");
  }
  if(is_tiff &&
     (resc->pixel_format() == VIL_PIXEL_FORMAT_RGBA_UINT_16 ||
      (resc->pixel_format() == VIL_PIXEL_FORMAT_UINT_16 && resc->nplanes() >= 3)
     )
    ) {
    vil_image_view<vxl_uint_16> view = resc->get_view();
    vil_image_view<vxl_uint_16> out(view.ni(), view.nj());
    for(unsigned j = 0; j<view.nj(); j++)
      for(unsigned i = 0; i<view.ni(); i++){
        double sum = 0.0;
        for(unsigned p = 0; p<3; ++p)
          sum += view(i,j,p);
        if(apply_mask)
          sum = view(i,j,3)>0 ? sum : 0.0;
        sum /= 3.0;
        out(i,j) = sum;
      }
    grey_resc = vil_new_image_resource(view.ni(), view.nj(), 1, VIL_PIXEL_FORMAT_UINT_16);
    grey_resc->put_view(out);
    return;
  }
  switch ( vil_pixel_format_component_format(resc->pixel_format()) ){
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro(F, T)                              \
  case F: this->convert<T>(resc, grey_resc); break
#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_RGB_UINT_64, vxl_uint_64 );
    macro( VIL_PIXEL_FORMAT_RGB_INT_64, vxl_int_64 );
#endif
    macro( VIL_PIXEL_FORMAT_RGB_UINT_32, vxl_uint_32 );
    macro( VIL_PIXEL_FORMAT_RGB_INT_32, vxl_int_32 );
    macro( VIL_PIXEL_FORMAT_RGB_UINT_16, vxl_uint_16 );
    macro( VIL_PIXEL_FORMAT_RGB_INT_16, vxl_int_16 );
    macro( VIL_PIXEL_FORMAT_RGB_BYTE, vxl_byte );
    macro( VIL_PIXEL_FORMAT_RGB_SBYTE, vxl_sbyte );
    macro( VIL_PIXEL_FORMAT_RGB_FLOAT, float );
    macro( VIL_PIXEL_FORMAT_RGB_DOUBLE, double );
#if VXL_HAS_INT_64
    macro( VIL_PIXEL_FORMAT_RGBA_UINT_64, vxl_uint_64 );
    macro( VIL_PIXEL_FORMAT_RGBA_INT_64, vxl_int_64 );
#endif
    macro( VIL_PIXEL_FORMAT_RGBA_UINT_32, vxl_uint_32 );
    macro( VIL_PIXEL_FORMAT_RGBA_INT_32, vxl_int_32 );
    macro( VIL_PIXEL_FORMAT_RGBA_UINT_16, vxl_uint_16 );
    macro( VIL_PIXEL_FORMAT_RGBA_INT_16, vxl_int_16 );
    macro( VIL_PIXEL_FORMAT_RGBA_BYTE, vxl_byte );
    macro( VIL_PIXEL_FORMAT_RGBA_SBYTE, vxl_sbyte );
    macro( VIL_PIXEL_FORMAT_RGBA_FLOAT, float );
    macro( VIL_PIXEL_FORMAT_RGBA_DOUBLE, double );
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
  default:{
    // if the image is already grey scale with a recognized format don't do anything
    vil_pixel_format fmt = resc->pixel_format();
    if(
#if VXL_HAS_INT_64
       fmt == VIL_PIXEL_FORMAT_UINT_64 ||
       fmt == VIL_PIXEL_FORMAT_INT_64 ||
#endif
       fmt == VIL_PIXEL_FORMAT_BYTE ||
       fmt == VIL_PIXEL_FORMAT_SBYTE ||
       fmt == VIL_PIXEL_FORMAT_UINT_16 ||
       fmt == VIL_PIXEL_FORMAT_INT_16 ||
       fmt == VIL_PIXEL_FORMAT_UINT_32||
       fmt == VIL_PIXEL_FORMAT_INT_32||
       fmt == VIL_PIXEL_FORMAT_FLOAT||
       fmt == VIL_PIXEL_FORMAT_DOUBLE){
      grey_resc = resc;
      return;
    }
    std::cout << "conversion failed with type " << resc->pixel_format() << std::endl;
    grey_resc = nullptr;
  }
  }
  }
  template <class T>
  void convert(vil_image_resource_sptr const& resc, vil_image_resource_sptr& grey_resc);
};
template <class T>
void bil_convert_resource_to_grey::convert(vil_image_resource_sptr const& resc, vil_image_resource_sptr& grey_resc){
  vil_image_view<T> view = resc->get_view();
  vil_image_view<T> out(view.ni(), view.nj());
  unsigned np = view.nplanes();
  double w = 1.0/np;
  for(unsigned j = 0; j<view.nj(); j++)
    for(unsigned i = 0; i<view.ni(); i++){
      double sum = 0.0;
      for(unsigned p = 0; p<np; ++p)
        sum += view(i,j,p);
      sum *= w;
      out(i,j) = sum;
    }
  grey_resc = vil_new_image_resource_of_view(out);
}

#endif // bil_convert_to_grey_h_
