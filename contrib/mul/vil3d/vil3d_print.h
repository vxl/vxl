// This is mul/vil3d/vil3d_print.h
#ifndef vil3d_print_h_
#define vil3d_print_h_
//:
// \file
// \author Ian Scott, Tim Cootes.

#include <iostream>
#include <vil/vil_print.h>
#include <vil3d/vil3d_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable: 4244 )  // conversion from ptrdiff_t to int, possible loss of data
#endif

//: Print all image data to os in a grid (rounds output to int)
// \relatesalso vil3d_image_view
template <class T>
inline void vil3d_print_all(std::ostream& os,const vil3d_image_view<T>& view)
{
  int width=os.width();
  os<<view.is_a()<<' '<<view.nplanes()
    <<" planes, each "<<view.ni()<<" x "<<view.nj()<<" x "<<view.nk()
    <<" istep: "<<view.istep()<<' '
    <<" jstep: "<<view.jstep()<<' '
    <<" kstep: "<<view.kstep()<<' '
    <<" planestep: "<<view.planestep()<<'\n' << std::flush;
  for (unsigned int p=0;p<view.nplanes();++p)
  {
    if (view.nplanes()>1) os<<"Plane "<<p<<'\n';
    for (unsigned int k=0;k<view.nk();++k)
    {
      if (view.nk()>1) os<<"Slice "<<k<<":\n";
      for (unsigned int j=0;j<view.nj();++j)
      {
        for (unsigned int i=0;i<view.ni();++i)
        {
          os.width(width);
          vil_print_value(os,view(i,j,k,p));
          os<<' ';
        }
        os<<'\n';
      }
    }
  }
}

#ifdef _MSC_VER
#  pragma warning( pop )
#endif


#endif // vil3d_print_h_
