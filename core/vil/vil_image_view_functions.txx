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

//: How to print value in vil2_print_all(image_view)
template<class T>
void vil2_print_value(vcl_ostream& os, const T& value)
{
  int v = int(value);
  if (v<10)  os<<" ";
  if (v<100) os<<" ";
  os<<v;
}


//: print all image data to os in a grid (rounds output to int)
template<class T>
void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view)
{
  os<<view.is_a()<<" "<<view.nplanes()<<" planes, each "<<view.nx()<<" x "<<view.ny();
  os<<" xstep: "<<view.xstep()<<" ";
  os<<" ystep: "<<view.ystep()<<" ";
  os<<" planestep: "<<view.planestep()<<endl;
  for (int i=0;i<view.nplanes();++i)
  {
    if (view.nplanes()>1) os<<"Plane "<<i<<":"<<vcl_endl;
    for (int y=0;y<view.ny();++y)
    {
      for (int x=0;x<view.nx();++x)
      {
	    vil2_print_value(os,view(x,y,i));
		os<<" ";
      }
      os<<vcl_endl;
    }
  }
}

// For things which must not be composites
#define VIL_IMAGE_VIEW_FUNCTIONS_INSTANTIATE_NON_COMP(T) \
template vil2_image_view<T > vil2_view_as_planes(const vil2_image_view<vil_rgb<T > >&); \
template vil2_image_view<vil_rgb<T > > vil2_view_as_rgb(const vil2_image_view<T >& plane_view); \
template void vil2_print_value(vcl_ostream& os, const T& value)

// For everything else
#define VIL_IMAGE_VIEW_FUNCTIONS_INSTANTIATE_2(T) \
template void vil2_print_all(vcl_ostream& os,const vil2_image_view<T >& view)

#endif
