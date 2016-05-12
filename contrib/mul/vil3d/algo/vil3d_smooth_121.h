// This is mul/vil3d/algo/vil3d_smooth_121.h
#ifndef vil3d_smooth_121_h_
#define vil3d_smooth_121_h_
//:
// \file
// \brief Smooth 3D image with a (1 2 1)/4 filter.
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_switch_axes.h>

//: Smooth src_im by applying (0.25 0.5 0.25) filter along i axis
//  Resulting image has same size. Border pixels (i=0,ni-1) set to zero.
template<class srcT, class destT>
void vil3d_smooth_121_i(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& smooth_im)
{
  const unsigned n_i = src_im.ni(),
                 n_j = src_im.nj(),
                 n_k = src_im.nk(),
                 n_p = src_im.nplanes();
  const unsigned ni1 = n_i-1;
  const std::ptrdiff_t s_istep = src_im.istep(),
                    s_jstep = src_im.jstep(),
                    s_kstep = src_im.kstep(),
                    s_pstep = src_im.planestep();

  smooth_im.set_size(n_i, n_j, n_k, n_p);

  const std::ptrdiff_t d_istep = smooth_im.istep(),
                      d_jstep = smooth_im.jstep(),
                      d_kstep = smooth_im.kstep(),
                      d_pstep = smooth_im.planestep();

  // Select first plane
  const srcT*  src_plane = src_im.origin_ptr();
  destT*      dest_plane = smooth_im.origin_ptr();
  for (unsigned p=0; p<n_p; ++p, src_plane+=s_pstep, dest_plane+=d_pstep)
  {
    // Select first slice of p-th plane
    const srcT* src_slice = src_plane;
    destT*     dest_slice = dest_plane;
    for (unsigned k=0; k<n_k; ++k, src_slice+=s_kstep, dest_slice+=d_kstep)
    {
      // Apply convolution to each row in turn
      // Note: Could check if either istep is 1 for speed optimisation.
      const srcT* src_row = src_slice;
      destT*     dest_row = dest_slice;

      for (unsigned int j=0; j<n_j; ++j, src_row+=s_jstep, dest_row+=d_jstep)
      {
        const srcT *s = src_row+s_istep;
        destT * d = dest_row+d_istep;
        destT * end_d = dest_row + ni1*d_istep;
        *dest_row=0;  // Zero the border
        *end_d = 0;
        for (;d!=end_d;d+=d_istep,s+=s_istep)
          *d = 0.25f*s[-s_istep] + 0.5f*s[0] + 0.25f*s[s_istep];
      }
    }
  }
}

//: Smooth src_im by applying (0.25 0.5 0.25) filter along j axis
//  Resulting image has same size. Border pixels (j=0,nj-1) set to zero.
template<class srcT, class destT>
void vil3d_smooth_121_j(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& smooth_im)
{
  smooth_im.set_size(src_im.ni(),src_im.nj(),src_im.nk(),src_im.nplanes());

  // Generate new views so that j-axis becomes the i-axis
  vil3d_image_view<srcT> src_jik = vil3d_switch_axes_jik(src_im);
  vil3d_image_view<destT> smooth_jik = vil3d_switch_axes_jik(smooth_im);
  vil3d_smooth_121_i(src_jik,smooth_jik);
}

//: Smooth src_im by applying (0.25 0.5 0.25) filter along k axis
//  Resulting image has same size. Border pixels (k=0,nk-1) set to zero.
template<class srcT, class destT>
void vil3d_smooth_121_k(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& smooth_im)
{
  smooth_im.set_size(src_im.ni(),src_im.nj(),src_im.nk(),src_im.nplanes());

  // Generate new views so that j-axis becomes the i-axis
  vil3d_image_view<srcT> src_kij = vil3d_switch_axes_kij(src_im);
  vil3d_image_view<destT> smooth_kij = vil3d_switch_axes_kij(smooth_im);
  vil3d_smooth_121_i(src_kij,smooth_kij);
}

//: Smooth src_im by applying (0.25 0.5 0.25) filter along each axis in turn
//  Resulting image has same size. Border pixels set to zero.
template<class srcT, class destT>
void vil3d_smooth_121(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& smooth_im)
{
  vil3d_image_view<destT> tmp_im;
  vil3d_smooth_121_i(src_im,smooth_im);  // Use smooth_im as temporary store
  vil3d_smooth_121_j(smooth_im,tmp_im);
  vil3d_smooth_121_k(tmp_im,smooth_im);  // Overwrite smooth_im with final result
}

#endif // vil3d_smooth_121_h_
