// This is mul/vil3d/algo/vil3d_normalised_correlation_3d.h
#ifndef vil3d_normalised_correlation_3d_h_
#define vil3d_normalised_correlation_3d_h_
//:
// \file
// \brief 3D normalised correlation
// \author Tim Cootes

#include <vcl_compiler.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>  // for vcl_sqrt()
#include <vil3d/vil3d_image_view.h>

//: Evaluate dot product between kernel and src_im
// Assumes that the kernel has been normalised to have zero mean
// and unit variance
// \relates vil3d_image_view
template <class srcT, class kernelT, class accumT>
inline accumT vil_norm_corr_2d_at_pt(const srcT *src_im, vcl_ptrdiff_t s_istep,
                                     vcl_ptrdiff_t s_jstep, vcl_ptrdiff_t s_kstep,
                                     vcl_ptrdiff_t s_pstep,
                                     const vil3d_image_view<kernelT>& kernel,
                                     accumT)
{
  unsigned ni = kernel.ni();
  unsigned nj = kernel.nj();
  unsigned nk = kernel.nk();
  unsigned np = kernel.nplanes();

  vcl_ptrdiff_t k_istep = kernel.istep(),
                k_jstep = kernel.jstep(),
                k_kstep = kernel.kstep();

  accumT sum=0;
  accumT mean=0;
  accumT sum_sq=0;
  for (unsigned p = 0; p<np; ++p)
  {
    // Select first slice of p-th plane
    const srcT*  src_slice  = src_im + p*s_pstep;
    const kernelT* k_slice =  kernel.origin_ptr() + p*kernel.planestep();

    for (unsigned int k=0;k<nk;++k,src_slice+=s_kstep,k_slice+=k_kstep)
    {
      // Select first row of k-th slice on p-th plane
      const srcT*  src_row  = src_slice;
      const kernelT* k_row =  k_slice;

      for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,k_row+=k_jstep)
      {
        const srcT* sp = src_row;
        const kernelT* kp = k_row;
        // Sum over j-th row
        for (unsigned int i=0;i<ni;++i, sp += s_istep, kp += k_istep)
        {
          sum += accumT(*sp) * accumT(*kp);
          mean+= accumT(*sp);
          sum_sq += accumT(*sp) * accumT(*sp);
        }
      }
    }
  }

  long n=ni*nj*nk*np;
  mean/=n;
  accumT var = sum_sq/n - mean*mean;
  return var<=0 ? 0 : sum/vcl_sqrt(var);
}

//: Normalised cross-correlation of (pre-normalised) kernel with srcT.
// Each dimension of dest is resized to (1+src_im.nX()-kernel.nX())
// (a one plane image).
// On exit dest(x,y,z) = sum_ijk src_im(x+i,y+j,y+k)*kernel(i,j,k)/sd_under_region
//
// Assumes that the kernel has been normalised to have zero mean
// and unit variance
// \relates vil3d_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil3d_normalised_correlation_3d(const vil3d_image_view<srcT>& src_im,
                                            vil3d_image_view<destT>& dest_im,
                                            const vil3d_image_view<kernelT>& kernel,
                                            accumT ac)
{
  unsigned ni = 1+src_im.ni()-kernel.ni(); assert(1+src_im.ni() >= kernel.ni());
  unsigned nj = 1+src_im.nj()-kernel.nj(); assert(1+src_im.nj() >= kernel.nj());
  unsigned nk = 1+src_im.nk()-kernel.nk(); assert(1+src_im.nk() >= kernel.nk());
  vcl_ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  vcl_ptrdiff_t s_kstep = src_im.kstep(), s_pstep = src_im.planestep();

  dest_im.set_size(ni,nj,nk,1);
  vcl_ptrdiff_t d_istep = dest_im.istep(),
                d_jstep = dest_im.jstep(),
                d_kstep = dest_im.kstep();

   // Select first slice of p-th plane
  const srcT*  src_slice  = src_im.origin_ptr();
  destT* dest_slice = dest_im.origin_ptr();

  for (unsigned k=0;k<nk;++k,src_slice+=s_kstep,dest_slice+=d_kstep)
  {
    // Select first row of k-th slice on p-th plane
    const srcT* src_row  = src_slice;
    destT* dest_row = dest_slice;

    for (unsigned j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
    {
      const srcT* sp = src_row;
      destT* dp = dest_row;
      for (unsigned i=0;i<ni;++i, sp += s_istep, dp += d_istep)
        *dp =(destT)vil_norm_corr_2d_at_pt(sp,s_istep,s_jstep,s_kstep,
                                           s_pstep,kernel,ac);
      // Convolve at src(i,j,k)
    }
 }
}

#endif // vil3d_normalised_correlation_3d_h_
