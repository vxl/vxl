// This is mul/vil3d/vil3d_gauss_reduce.h
#ifndef vil3d_gauss_reduce_h_
#define vil3d_gauss_reduce_h_
//: \file
//  \brief Functions to smooth and sub-sample 3D images in one direction
//  \author Tim Cootes
//  These are not templated because
//  a) Each type tends to need a slightly different implementation
//  b) Let's not have too many templates.

#include <vil2/algo/vil2_algo_gauss_reduce.h>

//: Smooth and subsample single plane src_im in i to produce dest_im
//  Applies 1-5-8-5-1 filter in i, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1][0,nk-1] elements of dest
//  Assumes dest_im has suffient data allocated.
//
//  By applying three times we can obtain a full gaussian smoothed and
//  sub-sampled 3D image
template<class T>
void vil3d_gauss_reduce(T* dest_im,
                           int d_i_step, int d_j_step, int d_k_step,
                           const T* src_im,
                           int src_ni, int src_nj, int src_nk,
                           int s_i_step, int s_j_step, int s_k_step)
{
  for (int k=0;k<src_nk;++k)
  {
    vil2_algo_gauss_reduce(dest_im,d_i_step, d_j_step,
                           src_im, src_ni,src_nj, s_i_step,s_j_step);
    dest_im += d_k_step;
    src_im  += s_k_step;
  }
}

#endif // vil3d_gauss_reduce_h_
