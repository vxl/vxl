// This is mul/vil2/vil2_image_view_functions.txx
#ifndef vil2_image_view_functions_txx_
#define vil2_image_view_functions_txx_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include "vil2_image_view_functions.h"
#include <vil/vil_rgb.h>
#include <vcl_cassert.h>


//: Compute minimum and maximum values over view
template<class T>
void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T>& view)
{
  if (view.size()==0)
  {
    min_value = 0;
    max_value = 0;
    return;
  }

  min_value = *(view.top_left_ptr());
  max_value = min_value;

  int ni = view.ni(), istep=view.istep();
  int nj = view.nj(), jstep=view.jstep();
  int np = view.nplanes(), pstep = view.planestep();

  const T* plane = view.top_left_ptr();
  for (int p=0;p<np;++p,plane += pstep)
  {
    const T* row = plane;
    for (int j=0;j<nj;++j,row += jstep)
    {
      const T* pixel = row;
      for (int i=0;i<ni;++i,pixel+=istep)
      {
        if (*pixel<min_value) min_value=*pixel;
        else if (*pixel>max_value) max_value=*pixel;
      }
    }
  }
}

//: Fill view with given value
template<class T>
void vil2_fill(vil2_image_view<T>& view, T value)
{
  int ni = view.ni(), istep=view.istep();
  int nj = view.nj(), jstep=view.jstep();
  int np = view.nplanes(), pstep = view.planestep();

  T* plane = view.top_left_ptr();
  for (int p=0;p<np;++p,plane += pstep)
  {
    T* row = plane;
    for (int j=0;j<nj;++j,row += jstep)
    {
      T* pixel = row;
      for (int i=0;i<ni;++i,pixel+=istep) *pixel=value;
    }
  }
}

//: Fill data[i*step] (i=0..n-1) with given value
template<class T>
void vil2_fill_line(T* data, unsigned n, int step, T value)
{
  T* end_data = data + n*step;
  while (data!=end_data) { *data=value; data+=step; }
}

//: Fill row j in view with given value
template<class T>
void vil2_fill_row(vil2_image_view<T>& view, unsigned j, T value)
{
  int ni = view.ni(), istep=view.istep();
  int nj = view.nj(), jstep=view.jstep();
  int np = view.nplanes(), pstep = view.planestep();

  assert(j<nj);

  T* row = view.top_left_ptr() + j*jstep;
  for (int p=0;p<np;++p,row += pstep)
    vil2_fill_line(row,ni,istep,value);
}

//: Fill column i in view with given value
template<class T>
void vil2_fill_col(vil2_image_view<T>& view, unsigned i, T value)
{
  int ni = view.ni(), istep=view.istep();
  int nj = view.nj(), jstep=view.jstep();
  int np = view.nplanes(), pstep = view.planestep();

  assert(i<ni);
  T* col_top = view.top_left_ptr() + i*istep;
  for (int p=0;p<np;++p,col_top += pstep)
    vil2_fill_line(col_top,nj,jstep,value);
}



// For things which must not be composites
#define VIL2_IMAGE_VIEW_FUNCTIONS_INSTANTIATE_FOR_SCALARS(T) \
template void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T >& view)

// For everything else
#define VIL2_IMAGE_VIEW_FUNCTIONS_INSTANTIATE(T) \
template void vil2_fill(vil2_image_view<T >& view, T value); \
template void vil2_fill_line(T * data, unsigned n, int step, T value); \
template void vil2_fill_row(vil2_image_view<T >& view, unsigned j, T value); \
template void vil2_fill_col(vil2_image_view<T >& view, unsigned i, T value)

#endif // vil2_image_view_functions_txx_
