// This is prip/vdtop/vdtop_replace_quasi_8_minima.h
#ifndef vdtop_replace_quasi_8_minima_h_
#define vdtop_replace_quasi_8_minima_h_

//:
// \file
// \brief.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vdtop_pixel.h"

//: replaces the values of all points naving no 8 lower neighbor by "value".
template <class T>
void vdtop_replace_quasi_8_minima(vil_image_view<T> & arg, vil_image_view<T> &res, T value)
{
  unsigned planestep=arg.planestep(),
    istep=arg.istep(),
    jstep=arg.jstep(),
    ni=arg.ni(),
    nj=arg.nj(),
    nplanes=arg.nplanes() ;
  res.set_size(ni, nj, nplanes) ;
  ni--;
  nj--;
  T* plane = arg.top_left_ptr();
  vdtop_pixel<T> pix(arg,plane) ;
  for (unsigned p=0; p<nplanes; ++p, plane+=planestep)
  {
    T* row = plane+jstep;
    for (unsigned j=1; j<nj; ++j, row+=jstep)
    {
      T* cur = row+istep;
      for (unsigned i=1; i<ni; ++i, cur+=istep)
      {
        pix.set_position(cur) ;
        if (pix.t8mm()!=0)
          res(i,j,p)=*cur ;
        else
          res(i,j,p)=value ;
      }
    }
  }
}

#endif
