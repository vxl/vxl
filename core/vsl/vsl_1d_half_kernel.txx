#ifndef vsl_1d_half_kernel_txx_
#define vsl_1d_half_kernel_txx_
/*
  fsm@robots.ox.ac.uk
*/

#include "vsl_1d_half_kernel.h"
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cmath.h>

//: helper routine used by create_gaussian
static/*FIXME*/ float compute_gauss_weight (float sigma, int mask_index) {
  float sum = 0;
  
  for (int repeat = 0; repeat < 6; repeat++) {
    float x = mask_index-0.5 + 0.2*repeat;
    sum += exp( - (x*x) / (2 * sigma * sigma));
  }
  
  return sum / 6;
}

//: routine to generate a gaussian convolution mask.
template <class T>
void vsl_create_gaussian (T gauss_sigma, vsl_1d_half_kernel<T> *mask_ptr) {
  // stop adding entries to the Gauss mask when the size of an entry is less than this : */
  float const CN_GAUSS_CUTOFF_VALUE = 0.02;
  
  int mask_index = 0;

  float gauss_weight = compute_gauss_weight (gauss_sigma, mask_index);
  while (gauss_weight > CN_GAUSS_CUTOFF_VALUE) {
    mask_ptr->array [mask_index] = gauss_weight;
    ++ mask_index;
    
    gauss_weight = compute_gauss_weight (gauss_sigma, mask_index);
    
    if (mask_index == mask_ptr->capacity) {
      cerr << "mask size equal to capacity - must recompile with new mask size";
      assert (false);
    }	
  }
  
  mask_ptr->count = mask_index;
  
  float total_mask_value = mask_ptr->array [0];
  for (mask_index = 1; mask_index < mask_ptr->count; mask_index++) {
    total_mask_value += 2 * mask_ptr->array [mask_index];
  }
  
  for (mask_index = 0; mask_index < mask_ptr->count; mask_index++) {
    mask_ptr->array [mask_index] /= total_mask_value;
    
    if (mask_ptr->array [mask_index] < CN_GAUSS_CUTOFF_VALUE)
      mask_ptr->count = mask_index;
  }
}

//----------------------------------------------------------------------------

#define VSL_1D_HALF_KERNEL_INSTANTIATE(T) \
template struct vsl_1d_half_kernel<T >; \
template void vsl_create_gaussian(T , vsl_1d_half_kernel<T> *);

#endif
