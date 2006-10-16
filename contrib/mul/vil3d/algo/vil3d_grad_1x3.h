#ifndef vil3d_grad_1x3_h_
#define vil3d_grad_1x3_h_
//:
//  \file
//  \brief Apply 1x3 gradient operator (-0.5 0 0.5) to image data
//  \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vxl_config.h>

//: Compute gradients of an image using (-0.5 0 0.5) Sobel filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
// \relates vil3d_image_view
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_i,
                    vil3d_image_view<destT>& grad_j,
                    vil3d_image_view<destT>& grad_k);

//: Compute gradients of an image using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an image.
//  grad_ijk has three times as many planes as src, with dest plane (3i) being the i-gradient
//  of source plane i and dest plane (3i+1) being the j-gradient etc
//  1 pixel border around grad images is set to zero
// \relates vil3d_image_view
template<class srcT, class destT>
void vil3d_grad_1x3(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_ijk);

//: Compute square gradient magnitude of 3D image
//  Use (-0.5,0,+0.5) filters in i,j,k
template<class srcT, class destT>
void vil3d_grad_1x3_mag_sq(const vil3d_image_view<srcT>& src,
                    vil3d_image_view<destT>& grad_mag2);

//: Compute gradients of single plane of 2D data using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_byte>& src,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k);

//: Compute gradients of single plane of 2D data using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
void vil3d_grad_1x3_1plane(const vil3d_image_view<float>& src,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k);

//: Compute gradients of single plane of 2D data using (-0.5 0 0.5) filters
//  Computes both i,j and k gradients of an ni x nj x nk plane of data
//  1 pixel border around grad images is set to zero
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_int_32>& src,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k);

//: Compute square dgradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<vxl_byte>& src_im,
                    vil3d_image_view<float>& grad_mag2);

//: Compute square gradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<float>& src_im,
                    vil3d_image_view<float>& grad_mag2);

//: Compute square gradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<vxl_int_32>& src_im,
                    vil3d_image_view<float>& grad_mag2);

//: Compute gradient by applying (-0.5 0 0.5) filter along i axis
//  Resulting image has same size. Border pixels (i=0,ni-1) set to zero.
template<class srcT, class destT>
void vil3d_grad_1x3_i(const vil3d_image_view<srcT>& src_im,
                      vil3d_image_view<destT>& grad_im)
{
  const unsigned n_i = src_im.ni(),
                 n_j = src_im.nj(),
                 n_k = src_im.nk(),
                 n_p = src_im.nplanes();
  const unsigned ni1 = n_i-1;
  const vcl_ptrdiff_t s_istep = src_im.istep(),
                    s_jstep = src_im.jstep(),
                    s_kstep = src_im.kstep(),
                    s_pstep = src_im.planestep();

  grad_im.set_size(n_i, n_j, n_k, n_p);

  const vcl_ptrdiff_t d_istep = grad_im.istep(),
                      d_jstep = grad_im.jstep(),
                      d_kstep = grad_im.kstep(),
                      d_pstep = grad_im.planestep();

  // Select first plane
  const srcT*  src_plane = src_im.origin_ptr();
  destT*      dest_plane = grad_im.origin_ptr();
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
        const srcT *s1 = src_row;
        const srcT *s2 = src_row+2*s_istep;
        destT * d = dest_row+d_istep;
        destT * end_d = dest_row + ni1*d_istep;
        *dest_row=0;  // Zero the border
        *end_d = 0;
        for (;d!=end_d;d+=d_istep,s1+=s_istep,s2+=s_istep)
          *d = 0.5*(*s2)-0.5*(*s1);
      }
    }
  }
}

//: Compute gradient by applying (-0.5 0 0.5) filter along j axis
//  Resulting image has same size. Border pixels (j=0,nj-1) set to zero.
template<class srcT, class destT>
void vil3d_grad_1x3_j(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_im)
{
  grad_im.set_size(src_im.ni(),src_im.nj(),src_im.nk(),src_im.nplanes());

  // Generate new views so that j-axis becomes the i-axis
  vil3d_image_view<srcT> src_jik = vil3d_switch_axes_jik(src_im);
  vil3d_image_view<destT> grad_jik = vil3d_switch_axes_jik(grad_im);
  vil3d_grad_1x3_i(src_jik,grad_jik);
}

//: Compute gradient by applying (-0.5 0 0.5) filter along k axis
//  Resulting image has same size. Border pixels (k=0,nk-1) set to zero.
template<class srcT, class destT>
void vil3d_grad_1x3_k(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_im)
{
  grad_im.set_size(src_im.ni(),src_im.nj(),src_im.nk(),src_im.nplanes());

  // Generate new views so that j-axis becomes the i-axis
  vil3d_image_view<srcT> src_kij = vil3d_switch_axes_kij(src_im);
  vil3d_image_view<destT> grad_kij = vil3d_switch_axes_kij(grad_im);
  vil3d_grad_1x3_i(src_kij,grad_kij);
}


#endif // vil3d_grad_1x3_h_
