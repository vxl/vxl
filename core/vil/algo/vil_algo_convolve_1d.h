#ifndef vil2_algo_convolve_1d_h_
#define vil2_algo_convolve_1d_h_
//: \file
//  \brief 1D Convolution with cunning boundary options
//  \author Tim Cootes (based on work by fsm)

#include <vcl_compiler.h>

//: Available options for boundary behavior
// When convolving a finite signal the boundaries may be
// treated in various ways which can often be expressed in terms
// of ways to extend the signal outside its original range.
enum vil2_convolve_boundary_option {
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
inline void vil2_algo_convolve_edge_1d(destT* dest, int d_step,
                          const srcT* src, int s_step,
						  const kernelT* kernel, int k_lo, int k_hi, int kstep,
						  accumT,
						  vil2_convolve_boundary_option option)
{
  if (option==vil2_convolve_no_extend)
  {
    // Initialise first elements of row to zero
    for (int i=k_lo;i<0;++i,dest+=d_step)
      *dest = 0;
  }
  else
  {
    vcl_cout<<"vil2_algo_convolve_edge_1d: ";
	vcl_cout<<"Sorry, can't deal with supplied edge option."<<vcl_endl;
	abort();
  }
}

//: Convolve kernel[x] x in [k_lo,k_hi] with srcT
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_1d(destT* dest0, int d_step,
                          const srcT* src0, unsigned nx, int s_step,
						  const kernelT* kernel, int k_lo, int k_hi,
						  accumT ac,
						  vil2_convolve_boundary_option start_option,
						  vil2_convolve_boundary_option end_option)
{
  // Deal with start (fill elements 0..1-k_lo of dest)
  vil2_algo_convolve_edge_1d(dest0,d_step,src0,s_step,kernel,k_lo,k_hi,1,ac,start_option);

  const kernelT* k_begin = kernel+k_lo;
  const kernelT* k_end   = kernel+k_hi+1;
  const srcT* src = src0;
  destT* dest = dest0-d_step*k_lo;

  destT* end_dest = dest + d_step*(int(nx)-k_hi);
  while (dest!=end_dest)
  {
    accumT sum = 0;
	const srcT* s= src;
	for (const kernelT *k = k_begin;k!=k_end; k++,s+=s_step) sum+= (*k)*(*s);
	*dest = destT(sum);
    src+=s_step;
    dest+=d_step;
  }

  // Deal with end  (reflect data and kernel!)
  vil2_algo_convolve_edge_1d(dest0+(nx-1)*d_step,-d_step,
                             src0+(nx-1)*s_step,-s_step,kernel,
							 -k_hi,-k_lo,-1,ac,end_option);
}
#endif