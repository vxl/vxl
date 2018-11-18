// This is core/vidl/vidl_istream_image_resource.cxx
//:
// \file

#include <iostream>
#include "vidl_istream_image_resource.h"
#include <vidl/vidl_istream.h>
#include "vidl_convert.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view.h>


vidl_istream_image_resource::
    vidl_istream_image_resource(const vidl_istream_sptr& i_stream, int frame,
                                unsigned int ni, unsigned int nj, unsigned int np,
                                vil_pixel_format format)
  : istream_(i_stream),
    frame_number_(frame),
    ni_(ni), nj_(nj), np_(np),
    format_(format)
{
}


//: try to find the image format and size from the current frame
bool
vidl_istream_image_resource::find_image_properties()
{
  vidl_frame_sptr frame = istream_->current_frame();
  if (!frame)
    return false;

  vidl_pixel_format fmt = frame->pixel_format();
  unsigned int bpp = vidl_pixel_format_bpp(fmt);
  ni_ = frame->ni();
  nj_ = frame->nj();
  np_ = vidl_pixel_format_num_channels(fmt);
  if (bpp / np_ < 16)
    format_ = VIL_PIXEL_FORMAT_BYTE;
  else
    format_ = VIL_PIXEL_FORMAT_UINT_16;

  return true;
}


vidl_istream_image_resource::
    vidl_istream_image_resource(const vidl_istream_sptr& i_stream, int frame)
  : istream_(i_stream),
    frame_number_(frame)
{
  assert(istream_);
  bool init = find_image_properties();
  assert(init);
}


vidl_istream_image_resource::
    vidl_istream_image_resource(const vidl_istream_sptr& i_stream)
  : istream_(i_stream),
    frame_number_(0)
{
  assert(istream_);
  frame_number_ = istream_->frame_number();
  bool init = find_image_properties();
  assert(init);
}


vidl_istream_image_resource::~vidl_istream_image_resource()
= default;


unsigned
vidl_istream_image_resource::nplanes() const
{
  return np_;
}


unsigned
vidl_istream_image_resource::ni() const
{
  return ni_;
}


unsigned
vidl_istream_image_resource::nj() const
{
  return nj_;
}


enum vil_pixel_format
vidl_istream_image_resource::pixel_format() const
{
  return format_;
}


bool
vidl_istream_image_resource::get_property(char const * /*key*/, void * /*value*/) const
{
  return false;
}


vil_image_view_base_sptr
vidl_istream_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                           unsigned j0, unsigned nj) const
{
  if (!istream_)
    return nullptr;

  int curr_frame = istream_->frame_number();
  vidl_frame_sptr frame = nullptr;
  if (curr_frame == frame_number_)
    frame = istream_->current_frame();
  if (curr_frame + 1 == frame_number_) {
    if (istream_->advance())
      frame = istream_->current_frame();
  }
  else {
    if (istream_->is_seekable() && istream_->seek_frame(frame_number_))
      frame = istream_->current_frame();
  }

  if (!frame)
    return nullptr;

  // try the wrap the frame in an image view
  vil_image_view_base_sptr view = vidl_convert_wrap_in_view(*frame);

  if (!view) {
    // try to convert the frame data to the expected view
    view = create_empty_view();
    vidl_convert_to_view(*frame,*view);
  }
  if (!view)
    return nullptr;

  if (i0 == 0 && j0 == 0 && ni == view->ni() && nj == view->nj())
    return view;

  if (i0 + ni > view->ni() || j0 + nj > view->nj()) return nullptr;

  switch (view->pixel_format())
  {
#define macro( F , T ) \
   case  F : { \
    const vil_image_view< T > &v = static_cast<const vil_image_view< T > &>(*view); \
    return new vil_image_view< T >(v.memory_chunk(), &v(i0,j0), \
                                   ni, nj, v.nplanes(), \
                                   v.istep(), v.jstep(), v.planestep()); }
   macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_BOOL , bool )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
   macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  std::complex<float>)
   macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , std::complex<double>)
#undef macro
   default:
    break;
  }

  return nullptr;
}


bool
vidl_istream_image_resource::put_view(vil_image_view_base const& /*view*/,
                                      unsigned /*x0*/, unsigned /*y0*/)
{
  std::cerr << "vidl_istream_image_resource::put_view not supported\n";
  return false;
}


//: create an empty image of the appropriate type and size
vil_image_view_base_sptr
vidl_istream_image_resource::create_empty_view() const
{
  switch (format_)
  {
#define macro( F , T ) \
   case  F : { \
    return new vil_image_view< T >(ni_,nj_,np_); }
   macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_BOOL , bool )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
   macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  std::complex<float>)
   macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , std::complex<double>)
#undef macro
   default:
    break;
  }
  return nullptr;
}
