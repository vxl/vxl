// This is mul/vil2/algo/vil2_algo_convolve_1d.h
#ifndef vil2_algo_convolve_1d_h_
#define vil2_algo_convolve_1d_h_
//:
//  \file
//  \brief 1D Convolution with cunning boundary options
//  \author Tim Cootes (based on work by fsm)

#include <vcl_compiler.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vil2/vil2_image_view.h>

//: Available options for boundary behavior
// When convolving a finite signal the boundaries may be
// treated in various ways which can often be expressed in terms
// of ways to extend the signal outside its original range.
enum vil2_convolve_boundary_option {
  // Do not fill destination edges at all
  // ie leave them unchanged.
  vil2_convolve_ignore_edge,
  // Do not to extend the signal, but pad with zeros.
  //     |                               |
  // K                       ----*-------
  // in   ... ---------------------------
  // out  ... --------------------0000000
  vil2_convolve_no_extend,

  // Zero-extend the input signal beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   ... ---------------------------000000000000...
  // out  ... ---------------------------
  vil2_convolve_zero_extend,

  // Extend the signal to be constant beyond the boundary
  //     |                               |
  // K                              ----*--------
  // in   ... --------------------------aaaaaaaaaaaaa...
  // out  ... ---------------------------
  vil2_convolve_constant_extend,

  // Extend the signal periodically beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   abc...-------------------------abc...------..
  // out  ... ---------------------------
  vil2_convolve_periodic_extend,

  // Extend the signal by reflection about the boundary.
  //     |                               |
  // K                               ----*--------
  // in   ... -------------------...edcbabcde...
  // out  ... ---------------------------
  vil2_convolve_reflect_extend,

  // This one is slightly different. The input signal is not
  // extended in any way, but the kernel is trimmed to allow
  // convolution to proceed up to the boundary and reweighted
  // to keep the total area the same.
  // *** may not work with kernels which take negative values.
  vil2_convolve_trim
};

//: Convolve edge with kernel[x*kstep] x in [k_lo,k_hi] (k_lo<=0)
//  Fills only edge: dest[i], i=0..(1-k_lo)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_edge_1d(const srcT* src, unsigned n, int s_step,
                                       destT* dest, int d_step,
                                       const kernelT* kernel, int k_lo, int k_hi, int kstep,
                                       accumT,
                                       vil2_convolve_boundary_option option)
{
  if (option==vil2_convolve_ignore_edge) return;
  if (option==vil2_convolve_no_extend)
  {
    // Initialise first elements of row to zero
    for (int i=k_lo;i<0;++i,dest+=d_step)
      *dest = 0;
  }
  else
  if (option==vil2_convolve_zero_extend)
  {
    // Assume src[i]==0 for i<0
    for (int i=k_lo+1;i<=0;++i,dest+=d_step,src+=s_step)
    {
      accumT sum = 0;
      const srcT* s = src;
      const kernelT* k = kernel+i*kstep;
      for (int j=i;j<=k_hi;++j,s+=s_step,k+=kstep) sum+= (*s)*(*k);
      *dest=sum;
    }
  }
  else
  if (option==vil2_convolve_constant_extend)
  {
    // Assume src[i]=src[0] for i<0
    int i_max = 1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
      {
        if ((i+j)<0) sum+=src[0]*kernel[j*kstep];
        else         sum+=src[(i+j)*s_step]*kernel[j*kstep];
      }
      dest[i]=sum;
    }
  }
  else
  if (option==vil2_convolve_periodic_extend)
  {
    // Assume src[i]=src[0] for i<0
    int i_max = 1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
      {
        if ((i+j)<0) sum+=src[0]*kernel[j*kstep];
        else         sum+=src[(i+j)*s_step]*kernel[j*kstep];
      }
      dest[i]=sum;
    }
  }
  else
  if (option==vil2_convolve_reflect_extend)
  {
    // Assume src[i]=src[n+i] for i<0
    int i_max = 1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
        sum+=src[((i+j+n)%n)*s_step]*kernel[j*kstep];
      dest[i]=sum;
    }
  }
  else
  if (option==vil2_convolve_trim)
  {
    // Truncate and reweight kernel
    accumT k_sum_all=0;
    for (int j=k_lo;j<=k_hi;++j) k_sum_all+=kernel[j*kstep];

    int i_max = 1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      accumT k_sum=0;
      // Sum elements which overlap src
      // ie i+j>=0  (so j starts at -i)
      for (int j=-i;j<=k_hi;++j)
      {
        sum+=src[(i+j)*s_step]*kernel[j*kstep];
        k_sum += kernel[j*kstep];
      }
      dest[i]=sum*k_sum_all/k_sum;
    }
  }
  else
  {
    vcl_cout<<"vil2_algo_convolve_edge_1d: ";
    vcl_cout<<"Sorry, can't deal with supplied edge option."<<vcl_endl;
    vcl_abort();
  }
}

//: Convolve kernel[x] x in [k_lo,k_hi] with srcT
// Assumes dest and src same size (nx)
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_1d(const srcT* src0, unsigned nx, int s_step,
                                  destT* dest0, int d_step,
                                  const kernelT* kernel, int k_lo, int k_hi,
                                  accumT ac,
                                  vil2_convolve_boundary_option start_option,
                                  vil2_convolve_boundary_option end_option)
{
  // Deal with start (fill elements 0..1-k_lo of dest)
  vil2_algo_convolve_edge_1d(src0,nx,s_step,dest0,d_step,kernel,k_lo,k_hi,1,ac,start_option);

  const kernelT* k_begin = kernel+k_lo;
  const kernelT* k_end   = kernel+k_hi+1;
  const srcT* src = src0;

  destT* end_dest = dest0 + d_step*(int(nx)-k_hi);
  for (destT* dest = dest0-d_step*k_lo;dest!=end_dest;dest+=d_step,src+=s_step)
  {
    accumT sum = 0;
    const srcT* s= src;
    for (const kernelT *k = k_begin;k!=k_end; ++k,s+=s_step) sum+= (*k)*(*s);
    *dest = destT(sum);
  }

  // Deal with end  (reflect data and kernel!)
  vil2_algo_convolve_edge_1d(src0+(nx-1)*s_step,nx,-s_step,
                             dest0+(nx-1)*d_step,-d_step,
                             kernel,-k_hi,-k_lo,-1,ac,end_option);
}

//: Convolve kernel[x] x in [k_lo,k_hi] with srcT
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_1d(const vil2_image_view<srcT>& src_im,
                                  vil2_image_view<destT>& dest_im,
                                  const kernelT* kernel, int k_lo, int k_hi,
                                  accumT ac,
                                  vil2_convolve_boundary_option start_option,
                                  vil2_convolve_boundary_option end_option)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  dest_im.resize(ni,nj,src_im.nplanes());

  for (int p=0;p<src_im.nplanes();++p)
  {
    // Select first row of p-th plane
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.pstep();
    const destT* dest_row = dest_im.top_left_ptr()+p*dest_im.pstep();

	// Apply convolution to each row in turn
	for (int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
      vil2_algo_convolve_1d(src_row,ni,s_istep,  dest_row,d_istep,
                            kernel,l_lo,k_hi,ac,start_option,end_option);
  }
}

#endif // vil2_algo_convolve_1d_h_
