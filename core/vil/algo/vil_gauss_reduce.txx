// This is mul/vil2/algo/vil2_gauss_reduce.txx
#ifndef vil2_gauss_reduce_txx_
#define vil2_gauss_reduce_txx_
//: \file
//  \brief Functions to smooth and sub-sample image in one direction
//  \author Tim Cootes

#include "vil2_gauss_reduce.h"

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil2_gauss_reduce(const vil2_image_view<T>& src_im,
                            vil2_image_view<T>& dest_im,
                            vil2_image_view<T>& work_im)
{
  unsigned int ni = src_im.ni();
  unsigned int nj = src_im.nj();
  unsigned int n_planes = src_im.nplanes();

  // Output image size
  unsigned int ni2 = (ni+1)/2;
  unsigned int nj2 = (nj+1)/2;

  dest_im.resize(ni2,nj2,n_planes);

  if (work_im.ni()<ni2 || work_im.nj()<nj)
    work_im.resize(ni2,nj);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil2_gauss_reduce(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                           src_im.istep(),src_im.jstep(),
                           work_im.top_left_ptr(),
                           work_im.istep(),work_im.jstep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil2_gauss_reduce(work_im.top_left_ptr(),nj,ni2,
                           work_im.jstep(),work_im.istep(),
                           dest_im.top_left_ptr()+i*dest_im.planestep(),
                           dest_im.jstep(),dest_im.istep());
  }
}
//: Smooth and subsample src_im to produce dest_im
//  Applies filter in x and y, then samples every other pixel.
//  work_im provides workspace
template<class T>
void vil2_gauss_reduce_121(const vil2_image_view<T>& src_im,
                                vil2_image_view<T>& dest_im,
                                vil2_image_view<T>& work_im)
{
  unsigned int ni = src_im.ni();
  unsigned int nj = src_im.nj();
  unsigned int n_planes = src_im.nplanes();

  // Output image size
  unsigned int ni2 = (ni+1)/2;
  unsigned int nj2 = (nj+1)/2;

  dest_im.resize(ni2,nj2,n_planes);

  if (work_im.ni()<ni2 || work_im.nj()<nj)
    work_im.resize(ni2,nj);

  // Reduce plane-by-plane
  for (unsigned int i=0;i<n_planes;++i)
  {
    // Smooth and subsample in x, result in work_im
    vil2_gauss_reduce_121(src_im.top_left_ptr()+i*src_im.planestep(),ni,nj,
                               src_im.istep(),src_im.jstep(),
                               work_im.top_left_ptr(),
                               work_im.istep(),work_im.jstep());

    // Smooth and subsample in y (by implicitly transposing work_im)
    vil2_gauss_reduce_121(work_im.top_left_ptr(),nj,ni2,
                               work_im.jstep(),work_im.istep(),
                               dest_im.top_left_ptr()+i*dest_im.planestep(),
                               dest_im.jstep(),dest_im.istep());
  }
}

#undef VIL2_GAUSS_REDUCE_INSTANTIATE
#define VIL2_GAUSS_REDUCE_INSTANTIATE(T) \
template void vil2_gauss_reduce(const vil2_image_view<T >& src, \
                                     vil2_image_view<T >& dest, \
                                     vil2_image_view<T >& work_im); \
template void vil2_gauss_reduce_121(const vil2_image_view<T >& src, \
                                         vil2_image_view<T >& dest, \
                                         vil2_image_view<T >& work_im)

#endif // vil2_gauss_reduce_txx_
