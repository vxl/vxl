// This is brl/bbas/vidl2/vidl2_istream_image_resource.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vidl2_istream_image_resource.h"
#include <vidl2/vidl2_istream.h>
#include "vidl2_convert.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_property.h>


vidl2_istream_image_resource::
    vidl2_istream_image_resource(const vidl2_istream_sptr& i_stream, int frame,
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
vidl2_istream_image_resource::find_image_properties()
{
  vidl2_frame_sptr frame = istream_->current_frame();
  if (!frame)
    return false;

  vidl2_pixel_format fmt = frame->pixel_format();
  unsigned int bpp = vidl2_pixel_format_bpp(fmt);
  ni_ = frame->ni();
  nj_ = frame->nj();
  np_ = vidl2_pixel_format_num_channels(fmt);
  if (bpp / np_ < 16)
    format_ = VIL_PIXEL_FORMAT_BYTE;
  else
    format_ = VIL_PIXEL_FORMAT_UINT_16;

  return true;
}


vidl2_istream_image_resource::
    vidl2_istream_image_resource(const vidl2_istream_sptr& i_stream, int frame)
  : istream_(i_stream),
    frame_number_(frame)
{
  assert(istream_);
  bool init = find_image_properties();
  assert(init);
}


vidl2_istream_image_resource::
    vidl2_istream_image_resource(const vidl2_istream_sptr& i_stream)
  : istream_(i_stream),
    frame_number_(0)
{
  assert(istream_);
  frame_number_ = istream_->frame_number();
  bool init = find_image_properties();
  assert(init);
}


vidl2_istream_image_resource::~vidl2_istream_image_resource()
{
}


unsigned
vidl2_istream_image_resource::nplanes() const
{
  return np_;
}


unsigned
vidl2_istream_image_resource::ni() const
{
  return ni_;
}


unsigned
vidl2_istream_image_resource::nj() const
{
  return nj_;
}


enum vil_pixel_format
vidl2_istream_image_resource::pixel_format() const
{
  return format_;
}


bool
vidl2_istream_image_resource::get_property(char const *key, void * value) const
{
  return false;
}


vil_image_view_base_sptr
vidl2_istream_image_resource::get_copy_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const
{
  if (!istream_)
    return NULL;

  int curr_frame = istream_->frame_number();
  vidl2_frame_sptr frame = NULL;
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
    return NULL;

  // try the wrap the frame in an image view
  vil_image_view_base_sptr view = vidl2_convert_wrap_in_view(*frame);

  if (!view) {
    // try to convert the frame data to the expected view
    view = create_empty_view();
    vidl2_convert_to_view(*frame,*view);
  }
  if (!view)
    return NULL;

  if (i0 == 0 && j0 == 0 && ni == view->ni() && nj == view->nj())
    return view;

  if (i0 + ni > view->ni() || j0 + nj > view->nj()) return NULL;

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
   macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
   macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
   default:
    break;
  }

  return NULL;
}


bool
vidl2_istream_image_resource::put_view(const vil_image_view_base &view,
                              unsigned x0, unsigned y0)
{
  vcl_cerr << "vidl2_istream_image_resource::put_view not supported\n";
  return false;
}


//: create an empty image of the appropriate type and size
vil_image_view_base_sptr
vidl2_istream_image_resource::create_empty_view() const
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
   macro(VIL_PIXEL_FORMAT_COMPLEX_FLOAT ,  vcl_complex<float>)
   macro(VIL_PIXEL_FORMAT_COMPLEX_DOUBLE , vcl_complex<double>)
#undef macro
   default:
    break;
  }
  return NULL;
}

