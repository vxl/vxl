#ifndef vil2_algo_sobel_3x3_txx_
#define vil2_algo_sobel_3x3_txx_
//:
//  \file
//  \brief Apply sobel gradient filter to an image
//  \author Tim Cootes

#include "vil2_algo_sobel_3x3.h"

//: Apply Sobel 3x3 gradient filter to image.
//  dest has twice as many planes as src, with dest plane (2i) being the x-gradient
//  of source plane i and dest plane (2i+1) being the y-gradient.
template<class srcT, class destT>
void vil2_algo_sobel_3x3(vil2_image_view<destT>& grad_xy,
                         vil2_image_view<srcT>& src)
{
  int np = src.nplanes();
  int nx = src.nx();
  int ny = src.ny();
  grad_xy.resize(nx,ny,2*np);
  for (int i=0;i<np;++i)
  {
    vil2_algo_sobel_3x3_1plane(grad_xy.top_left_ptr()+2*i*grad_xy.planestep(),
	                  grad_xy.xstep(),grad_xy.ystep(),
                      grad_xy.top_left_ptr()+(2*i+1)*grad_xy.planestep(),
					  grad_xy.xstep(),grad_xy.ystep(),
                      src.top_left_ptr()+i*src.planestep(),
					  src.xstep(),src.ystep(),nx,ny);
  }
}

//: Apply Sobel 3x3 gradient filter to 2D image
template<class srcT, class destT>
void vil2_algo_sobel_3x3(vil2_image_view<destT>& grad_x,
                         vil2_image_view<destT>& grad_y,
                         vil2_image_view<srcT>& src)
{
  int np = src.nplanes();
  int nx = src.nx();
  int ny = src.ny();
  grad_x.resize(nx,ny,np);
  grad_y.resize(nx,ny,np);
  for (int i=0;i<np;++i)
  {
    vil2_algo_sobel_3x3_1plane(grad_x.top_left_ptr()+i*grad_x.planestep(),
	                  grad_x.xstep(),grad_x.ystep(),
                      grad_y.top_left_ptr()+i*grad_y.planestep(),
					  grad_y.xstep(),grad_y.ystep(),
                      src.top_left_ptr()+i*src.planestep(),
					  src.xstep(),src.ystep(),nx,ny);
  }
}


#undef VIL2_ALGO_SOBEL_3X3_INSTANTIATE
#define VIL2_ALGO_SOBEL_3X3_INSTANTIATE(srcT, destT) \
template void vil2_algo_sobel_3x3(vil2_image_view<destT >& grad_xy, \
                         vil2_image_view<srcT >& src); \
template void vil2_algo_sobel_3x3(vil2_image_view<destT >& grad_x, \
                                     vil2_image_view<destT >& grad_y, \
                                     vil2_image_view<srcT >& src)

#endif // vil2_algo_sobel_3x3_txx_
