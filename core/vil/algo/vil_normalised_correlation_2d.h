// This is core/vil/algo/vil_normalised_correlation_2d.h
#ifndef vil_normalised_correlation_2d_h_
#define vil_normalised_correlation_2d_h_
//:
// \file
// \brief 2D normalised correlation
// \author Tim Cootes

#include <cmath>
#include <cstddef>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Evaluate dot product between kernel and src_im
// Assumes that the kernel has been normalised to have zero mean
// and unit variance
// \relatesalso vil_image_view
template <class srcT, class kernelT, class accumT>
inline accumT vil_norm_corr_2d_at_pt(const srcT *src_im, std::ptrdiff_t s_istep,
                                     std::ptrdiff_t s_jstep, std::ptrdiff_t s_pstep,
                                     const vil_image_view<kernelT>& kernel,
                                     accumT)
{
  unsigned ni = kernel.ni();
  unsigned nj = kernel.nj();
  unsigned np = kernel.nplanes();

  std::ptrdiff_t k_istep = kernel.istep(), k_jstep = kernel.jstep();

  accumT sum=0;
  accumT mean=0;
  accumT sum_sq=0;
  for (unsigned p = 0; p<np; ++p)
  {
    // Select first row of p-th plane
    const srcT*  src_row  = src_im + p*s_pstep;
    const kernelT* k_row =  kernel.top_left_ptr() + p*kernel.planestep();

    for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,k_row+=k_jstep)
    {
      const srcT* sp = src_row;
      const kernelT* kp = k_row;
      // Sum over j-th row
      for (unsigned int i=0;i<ni;++i, sp += s_istep, kp += k_istep)
      {
        sum += accumT(*sp)*accumT(*kp);
        mean+= accumT(*sp);
        sum_sq += accumT(*sp)*accumT(*sp);
      }
    }
  }

  long n=ni*nj*np;
  mean/=(accumT)n;
  accumT var = sum_sq/(accumT)n - mean*mean;
  return var<=0 ? 0 : sum/std::sqrt(var);
}

//: Normalised cross-correlation of (pre-normalised) kernel with srcT.
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
// On exit dest(x,y) = sum_ij src_im(x+i,y+j)*kernel(i,j)/sd_under_region
//
// Assumes that the kernel has been normalised to have zero mean
// and unit variance
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_normalised_correlation_2d(const vil_image_view<srcT>& src_im,
                                          vil_image_view<destT>& dest_im,
                                          const vil_image_view<kernelT>& kernel,
                                          accumT ac)
{
  unsigned ni = 1+src_im.ni()-kernel.ni(); assert(1+src_im.ni() >= kernel.ni());
  unsigned nj = 1+src_im.nj()-kernel.nj(); assert(1+src_im.nj() >= kernel.nj());
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  std::ptrdiff_t s_pstep = src_im.planestep();

  dest_im.set_size(ni,nj,1);
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  // Select first row of p-th plane
  const srcT*  src_row  = src_im.top_left_ptr();
  destT* dest_row = dest_im.top_left_ptr();

  for (unsigned j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
  {
    const srcT* sp = src_row;
    destT* dp = dest_row;
    for (unsigned i=0;i<ni;++i, sp += s_istep, dp += d_istep)
      *dp =(destT)vil_norm_corr_2d_at_pt(sp,s_istep,s_jstep,s_pstep,kernel,ac);
    // Convolve at src(i,j)
  }
}

#endif // vil_normalised_correlation_2d_h_
