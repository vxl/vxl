#ifndef mil_algo_grad_filter_2d_txx_
#define mil_algo_grad_filter_2d_txx_

//:
//  \file
//  \brief Apply sobel gradient filter to a 2D image
//  \author Tim Cootes

#include <mil/algo/mil_algo_grad_filter_2d.h>
#include <mil/algo/mil_algo_grad_3x3.h>
#include <mil/algo/mil_algo_grad_1x3.h>


//: Apply Sobel 3x3 gradient filter to 2D image.
//  dest has twice as many planes as src, with dest plane (2i) being the x-gradient
//  of source plane i and dest plane (2i+1) being the y-gradient.
template<class srcT, class destT>
void mil_algo_grad_filter_2d<srcT,destT>::filter_xy_3x3(mil_image_2d_of<destT>& dest,
                                                        mil_image_2d_of<srcT>& src)
{
  int n_planes = src.n_planes();
  dest.set_n_planes(2*n_planes);
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    mil_algo_grad_3x3(dest.plane(2*i),dest.xstep(),dest.ystep(),
                      dest.plane(2*i+1),dest.xstep(),dest.ystep(),
                      src.plane(i),src.xstep(),src.ystep(),nx,ny);
  }
}

//: Apply Sobel 3x3 gradient filter to 2D image
template<class srcT, class destT>
void mil_algo_grad_filter_2d<srcT,destT>::filter_xy_3x3(mil_image_2d_of<destT>& grad_x,
                                                        mil_image_2d_of<destT>& grad_y,
                                                        mil_image_2d_of<srcT>& src)
{
  int n_planes = src.n_planes();
  grad_x.set_n_planes(n_planes);
  grad_y.set_n_planes(n_planes);
  int nx = src.nx();
  int ny = src.ny();
  grad_x.resize(nx,ny);
  grad_y.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    mil_algo_grad_3x3(grad_x.plane(i),grad_x.xstep(),grad_x.ystep(),
                      grad_y.plane(i),grad_y.xstep(),grad_y.ystep(),
                      src.plane(i),src.xstep(),src.ystep(),nx,ny);
  }
}
//: Apply simple 1x3 gradient filter to 2D image.
//  dest has twice as many planes as src, with dest plane (2i) being the x-gradient
//  of source plane i and dest plane (2i+1) being the y-gradient.
template<class srcT, class destT>
void mil_algo_grad_filter_2d<srcT,destT>::filter_xy_1x3(mil_image_2d_of<destT>& dest,
                                                        mil_image_2d_of<srcT>& src)
{
  int n_planes = src.n_planes();
  dest.set_n_planes(2*n_planes);
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    mil_algo_grad_1x3(dest.plane(2*i),dest.xstep(),dest.ystep(),
                      dest.plane(2*i+1),dest.xstep(),dest.ystep(),
                      src.plane(i),src.xstep(),src.ystep(),nx,ny);
  }
}

//: Apply simple 1x3 gradient filter to 2D image
template<class srcT, class destT>
void mil_algo_grad_filter_2d<srcT,destT>::filter_xy_1x3(mil_image_2d_of<destT>& grad_x,
                                                        mil_image_2d_of<destT>& grad_y,
                                                        mil_image_2d_of<srcT>& src)
{
  int n_planes = src.n_planes();
  grad_x.set_n_planes(n_planes);
  grad_y.set_n_planes(n_planes);
  int nx = src.nx();
  int ny = src.ny();
  grad_x.resize(nx,ny);
  grad_y.resize(nx,ny);
  for (int i=0;i<n_planes;++i)
  {
    mil_algo_grad_1x3(grad_x.plane(i),grad_x.xstep(),grad_x.ystep(),
                      grad_y.plane(i),grad_y.xstep(),grad_y.ystep(),
                      src.plane(i),src.xstep(),src.ystep(),nx,ny);
  }
}

#undef MIL_ALGO_GRAD_FILTER_2D_INSTANTIATE
#define MIL_ALGO_GRAD_FILTER_2D_INSTANTIATE(srcT, destT) \
 template class mil_algo_grad_filter_2d<srcT, destT >

#endif // mil_algo_grad_filter_2d_txx_
