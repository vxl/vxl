// This is mul/vil3d/vil3d_gauss_reduce.h
#ifndef vil3d_gauss_reduce_h_
#define vil3d_gauss_reduce_h_
//: \file
//  \brief Functions to smooth and sub-sample 3D images in one direction
//  \author Tim Cootes
//  These are not templated because
//  a) Each type tends to need a slightly different implementation
//  b) Let's not have too manj templates.

#include <vil2/algo/vil2_gauss_reduce.h>

//: Smooth and subsample single plane src_im in i to produce dest_im
//  Applies 1-5-8-5-1 filter in i, then samples
//  every other pixel.  Fills [0,(ni+1)/2-1][0,nj-1][0,nk-1] elements of dest
//  Assumes dest_im has suffient data allocated.
//
//  By applying three times we can obtain a full gaussian smoothed and
//  sub-sampled 3D image
template<class T>
inline void vil3d_gauss_reduce_i(const T* src_im,
                           int src_ni, int src_nj, int src_nk,
                           int s_i_step, int s_j_step, int s_k_step,
						   T* dest_im,
                           int d_i_step, int d_j_step, int d_k_step)
{
  for (int k=0;k<src_nk;++k)
  {
    vil2_gauss_reduce(src_im, src_ni,src_nj, s_i_step,s_j_step,
	                  dest_im,d_i_step, d_j_step);
    dest_im += d_k_step;
    src_im  += s_k_step;
  }
}

//: Smooth and subsample src_im to produce dest_im
//  Applies filter in i,j and k directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x (nj+1)/2 x (nk+1)/2
template<class T>
inline void vil3d_gauss_reduce(const vil3d_image_view<T>& src_im,
                               vil3d_image_view<T>& dest_im,
							   vil3d_image_view<T>& work_im1,
							   vil3d_image_view<T>& work_im2)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;

  dest_im.resize(ni2,nj2,nk2,n_planes);

  if (work_im1.ni()<ni2 || work_im1.nj()<nj || work_im1.nk()<nk)
    work_im1.resize(ni2,nj,nk);

  if (work_im2.ni()<ni2 || work_im2.nj()<nj2 || work_im2.nk()<nk)
    work_im2.resize(ni2,nj2,nk);

  // Reduce plane-by-plane
  for (int p=0;p<n_planes;++p)
  {
    // Smooth and subsample in i, result in work_im1
    vil3d_gauss_reduce_i(
      src_im.origin_ptr()+p*src_im.planestep(),ni,nj,nk,
      src_im.istep(),src_im.jstep(),src_im.kstep(),
	  work_im1.origin_ptr(),work_im1.istep(),work_im1.jstep(),work_im1.kstep());

    // Smooth and subsample in j (by implicitly transposing), result in work_im2
    vil3d_gauss_reduce_i(
      work_im1.origin_ptr(),nj,ni2,nk,
      work_im1.jstep(),work_im1.istep(),work_im1.kstep(),
	  work_im2.origin_ptr(),work_im2.jstep(),work_im2.istep(),work_im2.kstep());

    // Smooth and subsample in k (by implicitly transposing)
    vil3d_gauss_reduce_i(
      work_im2.origin_ptr(),nk,ni2,nj2,
      work_im2.kstep(),work_im2.istep(),work_im2.jstep(),
	  dest_im.origin_ptr()+p*dest_im.planestep(),
	  dest_im.kstep(),dest_im.istep(),dest_im.jstep());
  }
}

//: Smooth and subsample src_im along i and j to produce dest_im
//  Applies filter in i,j directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x (nj+1)/2 x nk
template<class T>
inline void vil3d_gauss_reduce_ij(const vil3d_image_view<T>& src_im,
                               vil3d_image_view<T>& dest_im,
							   vil3d_image_view<T>& work_im1)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = nk;

  dest_im.resize(ni2,nj2,nk2,n_planes);

  if (work_im1.ni()<ni2 || work_im1.nj()<nj || work_im1.nk()<nk)
    work_im1.resize(ni2,nj,nk);

  // Reduce plane-by-plane
  for (int p=0;p<n_planes;++p)
  {
    // Smooth and subsample in i, result in work_im1
    vil3d_gauss_reduce_i(
      src_im.origin_ptr()+p*src_im.planestep(),ni,nj,nk,
      src_im.istep(),src_im.jstep(),src_im.kstep(),
	  work_im1.origin_ptr(),work_im1.istep(),work_im1.jstep(),work_im1.kstep());

    // Smooth and subsample in j (by implicitly transposing), result in dest_im
    vil3d_gauss_reduce_i(
      work_im1.origin_ptr(),nj,ni2,nk,
      work_im1.jstep(),work_im1.istep(),work_im1.kstep(),
	  dest_im.origin_ptr()+p*dest_im.planestep(),
	  dest_im.jstep(),dest_im.istep(),dest_im.kstep());
  }
}

//: Smooth and subsample src_im along i and k to produce dest_im
//  Applies filter in i,k directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x nj x (nk+1)/2
template<class T>
inline void vil3d_gauss_reduce_ik(const vil3d_image_view<T>& src_im,
                               vil3d_image_view<T>& dest_im,
							   vil3d_image_view<T>& work_im1)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = (ni+1)/2;
  unsigned nj2 = nj;
  unsigned nk2 = (nk+1)/2;

  dest_im.resize(ni2,nj2,nk2,n_planes);

  if (work_im1.ni()<ni2 || work_im1.nj()<nj || work_im1.nk()<nk)
    work_im1.resize(ni2,nj,nk);

  // Reduce plane-by-plane
  for (int p=0;p<n_planes;++p)
  {
    // Smooth and subsample in i, result in work_im1
    vil3d_gauss_reduce_i(
      src_im.origin_ptr()+p*src_im.planestep(),ni,nj,nk,
      src_im.istep(),src_im.jstep(),src_im.kstep(),
	  work_im1.origin_ptr(),work_im1.istep(),work_im1.jstep(),work_im1.kstep());

    // Smooth and subsample in k (by implicitly transposing), result in dest_im
    vil3d_gauss_reduce_i(
      work_im1.origin_ptr(),nk,ni2,nj,
      work_im1.kstep(),work_im1.istep(),work_im1.jstep(),
	  dest_im.origin_ptr()+p*dest_im.planestep(),
	  dest_im.kstep(),dest_im.istep(),dest_im.jstep());
  }
}

//: Smooth and subsample src_im along j and k to produce dest_im
//  Applies filter in j,k directions, then samples every other pixel.
//  Resulting image is ni x (nj+1)/2 x (nk+1)/2
template<class T>
inline void vil3d_gauss_reduce_jk(const vil3d_image_view<T>& src_im,
                               vil3d_image_view<T>& dest_im,
							   vil3d_image_view<T>& work_im1)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned n_planes = src_im.nplanes();

  // Output image size
  unsigned ni2 = ni;
  unsigned nj2 = (nj+1)/2;
  unsigned nk2 = (nk+1)/2;

  dest_im.resize(ni2,nj2,nk2,n_planes);

  if (work_im1.ni()<ni || work_im1.nj()<nj2 || work_im1.nk()<nk)
    work_im1.resize(ni,nj2,nk);

  // Reduce plane-by-plane
  for (int p=0;p<n_planes;++p)
  {
    // Smooth and subsample in j (by implicitly transposing), result in work_im1
    vil3d_gauss_reduce_i(
      src_im.origin_ptr()+p*src_im.planestep(),nj,ni,nk,
      src_im.jstep(),src_im.istep(),src_im.kstep(),
	  work_im1.origin_ptr(),work_im1.jstep(),work_im1.istep(),work_im1.kstep());

    // Smooth and subsample in k (by implicitly transposing), result in dest_im
    vil3d_gauss_reduce_i(
      work_im1.origin_ptr(),nk,ni,nj2,
      work_im1.kstep(),work_im1.istep(),work_im1.jstep(),
	  dest_im.origin_ptr()+p*dest_im.planestep(),
	  dest_im.kstep(),dest_im.istep(),dest_im.jstep());
  }
}

#endif // vil3d_gauss_reduce_h_
