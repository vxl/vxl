#ifndef vil2_algo_gauss_reduce_txx_
#define vil2_algo_gauss_reduce_txx_

//: \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include "vil2_algo_gauss_reduce.h"

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil2_algo_gauss_reduce(vil2_image_view<T>& dest_im,
                            const vil2_image_view<T>& src_im,
														vil2_image_view<T>& work_im)
{
  int nx = src_im.nx();
  int ny = src_im.ny();
  int n_planes = src_im.nplanes();

  // Output image size
  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;

  dest_im.resize(nx2,ny2,n_planes);

  if (work_im.nx()<nx2 || work_im.ny()<ny)
    work_im.resize(nx2,ny);

  // Reduce plane-by-plane
  for (int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil2_algo_gauss_reduce(work_im.top_left_ptr(),
		                          work_im.xstep(),work_im.ystep(),
                              src_im.top_left_ptr()+i*src_im.planestep(),nx,ny,
                              src_im.xstep(),src_im.ystep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil2_algo_gauss_reduce(dest_im.top_left_ptr()+i*dest_im.planestep(),
		                          dest_im.ystep(),dest_im.xstep(),
                              work_im.top_left_ptr(),ny,nx2,
                              work_im.ystep(),work_im.xstep());
  }
}
//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil2_algo_gauss_reduce_121(vil2_image_view<T>& dest_im,
                            const vil2_image_view<T>& src_im,
														vil2_image_view<T>& work_im)
{
  int nx = src_im.nx();
  int ny = src_im.ny();
  int n_planes = src_im.nplanes();

  // Output image size
  int nx2 = (nx+1)/2;
  int ny2 = (ny+1)/2;

  dest_im.resize(nx2,ny2,n_planes);

  if (work_im.nx()<nx2 || work_im.ny()<ny)
    work_im.resize(nx2,ny);

  // Reduce plane-by-plane
  for (int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil2_algo_gauss_reduce_121(work_im.top_left_ptr(),
		                          work_im.xstep(),work_im.ystep(),
                              src_im.top_left_ptr()+i*src_im.planestep(),nx,ny,
                              src_im.xstep(),src_im.ystep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil2_algo_gauss_reduce_121(dest_im.top_left_ptr()+i*dest_im.planestep(),
		                          dest_im.ystep(),dest_im.xstep(),
                              work_im.top_left_ptr(),ny,nx2,
                              work_im.ystep(),work_im.xstep());
  }
}

#undef VIL2_ALGO_GAUSS_REDUCE_INSTANTIATE
#define VIL2_ALGO_GAUSS_REDUCE_INSTANTIATE(T) \
template void vil2_algo_gauss_reduce(vil2_image_view<T>& dest, \
                            const vil2_image_view<T>& src, \
														vil2_image_view<T>& work_im); \
template void vil2_algo_gauss_reduce_121(vil2_image_view<T>& dest, \
                            const vil2_image_view<T>& src, \
														vil2_image_view<T>& work_im)


#endif