// This is mul/mil/algo/mil_algo_exp_filter_2d.txx
#ifndef mil_algo_exp_filter_2d_txx_
#define mil_algo_exp_filter_2d_txx_
//: \file
//  \brief Apply exponential filter to a 2D image
//  \author Tim Cootes

#include "mil_algo_exp_filter_2d.h"
#include <mil/algo/mil_algo_exp_filter_1d.h>


//: Apply exponential filter along x to 2D image
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
template<class srcT, class destT>
void mil_algo_exp_filter_2d<srcT,destT>::filter_x(mil_image_2d_of<destT>& dest,
                                                  mil_image_2d_of<srcT>& src, double k)
{
  int n_planes = src.n_planes();
  dest.set_n_planes(n_planes);
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    filter_x(dest.plane(i),src.plane(i),nx,ny,dest.xstep(),dest.ystep(),
             src.xstep(),src.ystep(),k);
  }
}

//: Apply exponential filter along y to 2D image
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
template<class srcT, class destT>
void mil_algo_exp_filter_2d<srcT,destT>::filter_y(mil_image_2d_of<destT>& dest,
                                                  mil_image_2d_of<srcT>& src, double k)
{
  int n_planes = src.n_planes();
  dest.set_n_planes(n_planes);
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
     // Apply filter in x to transpose (nx<->ny, xstep<->ystep)
    filter_x(dest.plane(i),src.plane(i),ny,nx,dest.ystep(),dest.xstep(),
             src.ystep(),src.xstep(),k);
  }
}

template<class srcT, class destT>
void mil_algo_exp_filter_2d<srcT,destT>::filter_xy(mil_image_2d_of<destT>& dest, mil_image_2d_of<srcT>& src, double k)
{
  filter_x(x_filtered_,src,k);

  // Now filter in y.
  // Note that source and dest are now the same type, so re-implement y filtering appropriately
  // to avoid compilation problems.
  int n_planes = src.n_planes();
  dest.set_n_planes(n_planes);
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    destT* dest_data = dest.plane(i);
    destT* fx_data   = x_filtered_.plane(i);
    for (int x=0;x<nx;++x)
    {
      mil_algo_exp_filter_1d(dest_data,dest.ystep(), fx_data,x_filtered_.ystep(),ny,k);
      dest_data += dest.xstep();
      fx_data   += x_filtered_.xstep();
    }
  }
}

//: Apply exponential filter along x to single plane
template<class srcT, class destT>
void mil_algo_exp_filter_2d<srcT,destT>::filter_x(destT* dest, const srcT* src,
                                                  int nx, int ny,
                                                  int d_x_step, int d_y_step,
                                                  int s_x_step, int s_y_step, double k)
{
  for (int y=0;y<ny;++y)
  {
    mil_algo_exp_filter_1d(dest,d_x_step,src,s_x_step,nx,k);
    dest += d_y_step;
    src  += s_y_step;
  }
}

#undef MIL_ALGO_EXP_FILTER_2D_INSTANTIATE
#define MIL_ALGO_EXP_FILTER_2D_INSTANTIATE(srcT, destT) \
 template class mil_algo_exp_filter_2d<srcT, destT >

#endif // mil_algo_exp_filter_2d_txx_
