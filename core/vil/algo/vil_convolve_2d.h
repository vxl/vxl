// This is mul/vil2/algo/vil2_convolve_2d.h
#ifndef vil2_convolve_2d_h_
#define vil2_convolve_2d_h_
//:
//  \file
//  \brief 2D Convolution
//  \author Tim Cootes

#include <vil2/algo/vil2_convolve_1d.h>
#include <vcl_compiler.h>
#include <vcl_cassert.h>


//: Evaluate dot product between kernel and src_im
// Returns  sum_ijp src_im[i*istep+j*jstep+p*pstep]*kernel(i,j,p)
template <class srcT, class kernelT, class accumT>
inline accumT vil2_convolve_2d_at_pt(const srcT *src_im,
                                     int s_istep, int s_jstep, int s_pstep,
                                     const vil2_image_view<kernelT>& kernel,
                                     accumT)
{
  unsigned ni = kernel.ni();
  unsigned nj = kernel.nj();
  unsigned np = kernel.nplanes();

  int k_istep = kernel.istep(), k_jstep = kernel.jstep();

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

//: Convolve kernel with srcT
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
// On exit dest(x,y) = sum_ij src_im(x+i,y+j)*kernel(i,j)
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_convolve_2d(const vil2_image_view<srcT>& src_im,
                                  vil2_image_view<destT>& dest_im,
                                  const vil2_image_view<kernelT>& kernel,
                                  accumT ac)
{
  int ni = 1+src_im.ni()-kernel.ni(); assert(ni >= 0);
  int nj = 1+src_im.nj()-kernel.nj(); assert(nj >= 0);
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();
  int s_pstep = src_im.planestep();

  dest_im.set_size(ni,nj,1);
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  // Select first row of p-th plane
  const srcT*  src_row  = src_im.top_left_ptr();
  destT* dest_row = dest_im.top_left_ptr();

  for (int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
  {
    const srcT* sp = src_row;
    destT* dp = dest_row;
    for (int i=0;i<ni;++i, sp += s_istep, dp += d_istep)
      *dp = (destT)vil2_convolve_2d_at_pt(sp,s_istep,s_jstep,s_pstep,kernel,ac);
      // Convolve at src(i,j)
  }
}

#endif // vil2_convolve_2d_h_
