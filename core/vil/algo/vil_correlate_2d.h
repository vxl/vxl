// This is core/vil/algo/vil_correlate_2d.h
#ifndef vil_correlate_2d_h_
#define vil_correlate_2d_h_
//:
// \file
// \brief 2D Convolution
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_image_view.h>

//: Evaluate dot product between kernel and src_im
// Returns  sum_ijp src_im[i*istep+j*jstep+p*pstep]*kernel(i,j,p)
// \relatesalso vil_image_view
template <class srcT, class kernelT, class accumT>
inline accumT vil_correlate_2d_at_pt(const srcT *src_im, std::ptrdiff_t s_istep,
                                     std::ptrdiff_t s_jstep, std::ptrdiff_t s_pstep,
                                     const vil_image_view<kernelT>& kernel,
                                     accumT)
{
  unsigned ni = kernel.ni();
  unsigned nj = kernel.nj();
  unsigned np = kernel.nplanes();

  std::ptrdiff_t k_istep = kernel.istep(), k_jstep = kernel.jstep();

  accumT sum=0;
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
        sum += accumT(*sp)*accumT(*kp);
    }
  }

  return sum;
}

//: Correlate kernel with srcT
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
// On exit dest(x,y) = sum_ij src_im(x+i,y+j)*kernel(i,j)
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_correlate_2d(const vil_image_view<srcT>& src_im,
                             vil_image_view<destT>& dest_im,
                             const vil_image_view<kernelT>& kernel,
                             accumT ac)
{
  int ni = 1+src_im.ni()-kernel.ni(); assert(ni >= 0);
  int nj = 1+src_im.nj()-kernel.nj(); assert(nj >= 0);
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  std::ptrdiff_t s_pstep = src_im.planestep();

  dest_im.set_size(ni,nj,1);
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  // Select first row of p-th plane
  const srcT*  src_row  = src_im.top_left_ptr();
  destT* dest_row = dest_im.top_left_ptr();

  for (int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
  {
    const srcT* sp = src_row;
    destT* dp = dest_row;
    for (int i=0;i<ni;++i, sp += s_istep, dp += d_istep)
      *dp = (destT)vil_correlate_2d_at_pt(sp,s_istep,s_jstep,s_pstep,kernel,ac);
      // Correlate at src(i,j)
  }
}

#endif // vil_correlate_2d_h_
