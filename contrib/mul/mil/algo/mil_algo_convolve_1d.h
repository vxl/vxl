#ifndef mil_algo_convolve_1d_h_
#define mil_algo_convolve_1d_h_
//:
// \file
// \author Tim Cootes


//: Convolve kernel[x] x in [k_lo,k_hi] with srcT
// Doesn't give options for boundary yet - just zeros it
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void mil_algo_convolve_1d(destT* dest, int d_step,
                          const srcT* src, unsigned nx, int s_step,
						  const kernelT* kernel, int k_lo, int k_hi,
						  accumT)
{
  // Initialise first elements of row to zero
  for (int i=k_lo;i<0;++i,dest+=d_step)
    *dest = 0;

  kernelT* k_begin = kernel+k_lo;
  kernelT* k_end   = kernel+k_hi;

  destT* end_dest = dest + d_step*(nx-k_hi);
  while (dest!=end_dest)
  {
    accumT sum = 0;
    kernelT *k = k_begin;
	srcT s= src;
	while (k!=k_end, k++,s+=s_step) sum+= (*k)*(*s);
    src+=s_step;
    dest+=d_step;
  }

  // Zero last elements of row
  for (int i=0;i<=k_hi;++i,dest+=d_step) *dest = 0;
}