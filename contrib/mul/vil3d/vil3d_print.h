// This is mul/vil3d/vil3d_print.h
#ifndef vil3d_print_h_
#define vil3d_print_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott, Tim Cootes.

#include <vil2/vil2_print.h>
#include <vil3d/vil3d_image_view.h>

//: Print all image data to os in a grid (rounds output to int)
// \relates vil3d_image_view
template<class T>
inline void vil3d_print_all(vcl_ostream& os,const vil3d_image_view<T>& view)
{
  os<<view.is_a()<<" "<<view.nplanes()
    <<" planes, each "<<view.ni()<<" x "<<view.nj()<<" x "<<view.nk();
  os<<" istep: "<<view.istep()<<" ";
  os<<" jstep: "<<view.jstep()<<" ";
  os<<" kstep: "<<view.kstep()<<" ";
  os<<" planestep: "<<view.planestep()<<'\n';
  for (unsigned int p=0;p<view.nplanes();++p)
   for (unsigned int k=0;k<view.nk();++k)
   {
    if (view.nplanes()>1) os<<"Plane "<<p<<", Slice "<<k<<":\n";
    for (unsigned int j=0;j<view.nj();++j)
    {
      for (unsigned int i=0;i<view.ni();++i)
      {
        vil2_print_value(os,view(i,j,k,p));
        os<<" ";
      }
      os<<'\n';
    }
  }
}

#endif // vil3d_print_h_
