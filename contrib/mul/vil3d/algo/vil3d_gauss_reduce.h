// This is mul/vil3d/algo/vil3d_gauss_reduce.h
#ifndef vil3d_gauss_reduce_h_
#define vil3d_gauss_reduce_h_
//:
// \file
// \brief Functions to smooth and sub-sample 3D images in one direction
//
//  These are not templated because
//  - Each type tends to need a slightly different implementation
//  - Let's not have too many templates.
// \author Tim Cootes

#include <vil/algo/vil_gauss_reduce.h>
#include <vil3d/vil3d_image_view.h>


//: Smooth and subsample single plane src_im in i to produce dest_im
//  Applies 1-5-8-5-1 filter in i, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1][0,nk-1] elements of dest
//  Assumes dest_im has sufficient data allocated.
//
//  By applying three times we can obtain a full gaussian smoothed and
//  sub-sampled 3D image
template<class T>
void vil3d_gauss_reduce_i(const T* src_im,
                          unsigned src_ni, unsigned src_nj, unsigned src_nk,
                          vcl_ptrdiff_t s_i_step, vcl_ptrdiff_t s_j_step,
                          vcl_ptrdiff_t s_k_step,
                          T* dest_im,
                          vcl_ptrdiff_t d_i_step,
                          vcl_ptrdiff_t d_j_step, vcl_ptrdiff_t d_k_step);

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in i,j and k directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x (nj+1)/2 x (nk+1)/2. 
//  An image can be reduced in-place, by having src_im and dest_im 
//  pointing to the same image.
template<class T>
void vil3d_gauss_reduce(const vil3d_image_view<T>& src_im,
                        vil3d_image_view<T>&       dest_im,
                        vil3d_image_view<T>&       work_im1,
                        vil3d_image_view<T>&       work_im2);

//: Smooth and subsample src_im along i and j to produce dest_im
//  Applies filter in i,j directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x (nj+1)/2 x nk
template<class T>
void vil3d_gauss_reduce_ij(const vil3d_image_view<T>& src_im,
                           vil3d_image_view<T>&       dest_im,
                           vil3d_image_view<T>&       work_im1);

//: Smooth and subsample src_im along i and k to produce dest_im
//  Applies filter in i,k directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x nj x (nk+1)/2
template<class T>
void vil3d_gauss_reduce_ik(const vil3d_image_view<T>& src_im,
                           vil3d_image_view<T>&       dest_im,
                           vil3d_image_view<T>&       work_im1);

//: Smooth and subsample src_im along j and k to produce dest_im
//  Applies filter in j,k directions, then samples every other pixel.
//  Resulting image is ni x (nj+1)/2 x (nk+1)/2
template<class T>
void vil3d_gauss_reduce_jk(const vil3d_image_view<T>& src_im,
                           vil3d_image_view<T>&       dest_im,
                           vil3d_image_view<T>&       work_im1);

#define VIL3D_GAUSS_REDUCE_INSTANTIATE(T) extern "please include vil3d/vil3d_gauss_reduce.txx instead"

#endif // vil3d_gauss_reduce_h_
