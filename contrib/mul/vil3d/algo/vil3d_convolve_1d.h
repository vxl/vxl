// This is mul/vil3d/algo/vil_convolve_1d.h
#ifndef vil3d_algo_convolve_1d_h_
#define vil3d_algo_convolve_1d_h_
//:
// \file
// \brief 1D Convolution with cunning boundary options
// \author Ian Scott
//
// Note. The convolution operation is defined by
//    $(f*g)(x) = \int f(x-y) g(y) dy$
// i.e. the kernel g is reflected before the integration is performed.
// If you don't want this to happen, the behaviour you want is not
// called "convolution".

#include <vcl_cassert.h>
#include <vil/algo/vil_convolve_1d.h>



//: Convolve kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i-x,j)*kernel(x)  (x=k_lo..k_hi)
// \note  This function reverses the kernel. If you don't want the
// kernel reversed, use vil_correlate_1d instead. The kernel must
// not be larger than src_im.ni()
// \param kernel should point to tap 0.
// \param dest_im will be resized to size of src_im.
//
// If you want to convolve in all three directions, use the following approach:
// verbatim
  vil3d_convolve_1d(                             src, smoothed1, ... );

  vil3d_convolve_1d(vil3d_switch_axes_jki(smoothed1), smoothed2, ... );
  smoothed2_im = vil3d_switch_axes_kij(smoothed2);
  
  vil3d_convolve_1d(vil3d_switch_axes_kij(smoothed2), smoothed3, ... );
  smoothed3_im = vil3d_switch_axes_jki(smoothed3);

  // \endverbatim
// \relates vil3d_image_view

template <class srcT, class destT, class kernelT, class accumT>
inline void vil3d_convolve_1d(const vil3d_image_view<srcT>& src_im,
                              vil3d_image_view<destT>& dest_im,
                              const kernelT* kernel,
                              vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
                              accumT ac,
                              enum vil_convolve_boundary_option start_option,
                              enum vil_convolve_boundary_option end_option)
{
  const unsigned n_i = src_im.ni(),
                 n_j = src_im.nj(),
                 n_k = src_im.nk(),
                 n_p = src_im.nplanes();
  assert(k_hi - k_lo +1 <= (int) n_i);
  const vcl_ptrdiff_t s_istep = src_im.istep(),
                    s_jstep = src_im.jstep(),
                    s_kstep = src_im.kstep(),
                    s_pstep = src_im.planestep();

  dest_im.set_size(n_i, n_j, n_k, n_p);
  
  const vcl_ptrdiff_t d_istep = dest_im.istep(),
                      d_jstep = dest_im.jstep(),
                      d_kstep = dest_im.kstep(),
                      d_pstep = dest_im.planestep();

  // Select first plane
  const srcT*  src_plane = src_im.origin_ptr();
  destT*     dest_plane = dest_im.origin_ptr();
  for (unsigned p=0; p<n_p; ++p, src_plane+=s_pstep, dest_plane+=d_pstep)
  {
    // Select first slice of p-th plane
    const srcT* src_slice = src_plane;
    destT*     dest_slice = dest_plane;
    for (unsigned k=0; k<n_k; ++k, src_slice+=s_kstep, dest_slice+=d_kstep)
    {
  
      // Apply convolution to each row in turn
      // First check if either istep is 1 for speed optimisation.
      const srcT* src_row = src_slice;
      destT*     dest_row = dest_slice;
  
      if (s_istep == 1)
      {
        if (d_istep == 1)
          for (unsigned int j=0; j<n_j; ++j, src_row+=s_jstep, dest_row+=d_jstep)
            vil_convolve_1d(src_row, n_i, 1, dest_row, 1,
                            kernel, k_lo, k_hi, ac, start_option, end_option);
        else
          for (unsigned int j=0; j<n_j; ++j, src_row+=s_jstep, dest_row+=d_jstep)
            vil_convolve_1d(src_row, n_i, 1, dest_row, d_istep,
                            kernel, k_lo, k_hi, ac, start_option, end_option);
      }
      else
      {
        if (d_istep == 1)
          for (unsigned int j=0; j<n_j; ++j, src_row+=s_jstep, dest_row+=d_jstep)
            vil_convolve_1d(src_row, n_i, s_istep, dest_row, 1,
                            kernel, k_lo, k_hi, ac, start_option, end_option);
        else
          for (unsigned int j=0; j<n_j; ++j, src_row+=s_jstep, dest_row+=d_jstep)
            vil_convolve_1d(src_row, n_i, s_istep, dest_row, d_istep,
                            kernel, k_lo, k_hi, ac, start_option, end_option);
      }
    }
  }
}


#endif // vil3d_algo_convolve_1d_h_

