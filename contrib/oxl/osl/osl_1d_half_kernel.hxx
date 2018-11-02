// This is oxl/osl/osl_1d_half_kernel.hxx
#ifndef osl_1d_half_kernel_hxx_
#define osl_1d_half_kernel_hxx_
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "osl_1d_half_kernel.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: helper routine used by create_gaussian
//static/*FIXME*/
float osl_compute_gauss_weight (float sigma, int mask_index)
{
  float sum = 0;

  for (int repeat = 0; repeat < 6; repeat++)
  {
    double x = mask_index-0.5 + 0.2*repeat;
    sum += (float)std::exp( - (x*x) / (2 * sigma * sigma));
  }

  return sum / 6;
}

//: routine to generate a gaussian convolution mask.
template <class T>
void osl_create_gaussian (T gauss_sigma, osl_1d_half_kernel<T> *mask_ptr) {
  // stop adding entries to the Gauss mask when the size of an entry is less than this : */
  float const CN_GAUSS_CUTOFF_VALUE = 0.02f;

  unsigned mask_index = 0;

  double gauss_weight = osl_compute_gauss_weight((float)gauss_sigma, mask_index);
  while (gauss_weight > CN_GAUSS_CUTOFF_VALUE)
  {
    mask_ptr->array [mask_index] = gauss_weight;
    ++ mask_index;

    gauss_weight = osl_compute_gauss_weight((float)gauss_sigma, mask_index);

    if (mask_index == mask_ptr->capacity)
    {
      std::cerr << "mask size equal to capacity - must recompile with new mask size\n";
      assert(mask_index != mask_ptr->capacity);
    }
  }

  mask_ptr->count = mask_index;

  double total_mask_value = mask_ptr->array [0];
  for (mask_index = 1; mask_index < mask_ptr->count; mask_index++)
    total_mask_value += 2 * mask_ptr->array [mask_index];

  for (mask_index = 0; mask_index < mask_ptr->count; mask_index++)
  {
    mask_ptr->array [mask_index] /= total_mask_value;

    if (mask_ptr->array [mask_index] < CN_GAUSS_CUTOFF_VALUE)
      mask_ptr->count = mask_index;
  }
}

//----------------------------------------------------------------------------

#define OSL_1D_HALF_KERNEL_INSTANTIATE(T) \
template struct osl_1d_half_kernel<T >; \
template void osl_create_gaussian(T , osl_1d_half_kernel<T > *)

#endif // osl_1d_half_kernel_hxx_
