// This is mul/vil2/vil2_image_view_functions.txx
#ifndef vil2_image_view_functions_txx_
#define vil2_image_view_functions_txx_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view_functions.h>
#include <vcl_cassert.h>

//: Return a 3-plane view of an RGB image
template<class T>
vil2_image_view<T> vil2_view_as_planes(const vil2_image_view<vil_rgb<T> >& v)
{
  assert(v.nplanes()==1);

  // Image is RGBRGBRGB so x step = 3*v.xstep(), ystep=3*v.ystep()
  return vil2_image_view<T>(v.memory_chunk(),(T*) v.top_left_ptr(),
                            v.nx(),v.ny(),3,
                            v.xstep()*3,v.ystep()*3,1);
}

//: Return an RGB component view of a 3-plane image
//  Aborts if plane image not in correct format (ie planestep()!=1)
template<class T>
vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& v)
{
  assert(v.nplanes()==3);
  assert(v.planestep()==1);
  assert(v.xstep()==3 || v.ystep()==3);

  return vil2_image_view<vil_rgb<vil_byte> >(v.memory_chunk(),
                                             (vil_rgb<vil_byte>*) v.top_left_ptr(),
                                             v.nx(),v.ny(),1,
                                             v.xstep()/3,v.ystep()/3,1);
}

#define VIL_IMAGE_VIEW_FUNCTIONS_INSTANTIATE(T) \
template vil2_image_view<T > vil2_view_as_planes(const vil2_image_view<vil_rgb<T > >&); \
template vil2_image_view<vil_rgb<T > > vil2_view_as_rgb(const vil2_image_view<T >& plane_view)

#endif
