// This is mul/vil2/vil2_print.h
#ifndef vil2_print_h_
#define vil2_print_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott, Tim Cootes.

#include <vil2/vil2_image_view.h>

//: How to print value in vil2_print_all(image_view)
// \relates vil2_image_view
template<class T>
void vil2_print_value(vcl_ostream& s, const T& value);

//: Print all image data to os in a grid (rounds output to int)
// \relates vil2_image_view
template<class T>
inline void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view)
{
  os<<view.is_a()<<" "<<view.nplanes()<<" planes, each "<<view.ni()<<" x "<<view.nj()
    <<" istep: "<<view.istep()<<' '
    <<" jstep: "<<view.jstep()<<' '
    <<" planestep: "<<view.planestep()<<'\n' << vcl_flush;
  for (unsigned int p=0;p<view.nplanes();++p)
  {
    if (view.nplanes()>1) os<<"Plane "<<p<<":\n" << vcl_flush;
    for (unsigned int j=0;j<view.nj();++j)
    {
      for (unsigned int i=0;i<view.ni();++i)
      {
        os<<' ';
        vil2_print_value(os,view(i,j,p));
      }
      os<<'\n'<<vcl_flush;
    }
  }
}

#endif // vil2_print_h_
