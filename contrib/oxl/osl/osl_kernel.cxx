// This is oxl/osl/osl_kernel.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "osl_kernel.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

// Construct one half of a Gaussian convolution kernel.
//
//   kernel_[i] = exp( (i-width_)^2/sigma^2 )/det
void osl_kernel_DOG(float sigma_, float *kernel_, int k_size_, int width_)
{
  float s2 = 2.0f*sigma_*sigma_;
  float det = sigma_*(float)vnl_math::sqrt2pi;

  for (int i=0,x=-width_; i<k_size_; ++i,++x)
    kernel_[i] = (float)std::exp(-x*x/s2)/det;
}

// Construct one half of a Gaussian convolution kernel.
// With fancy stuff.
void osl_kernel_DOG(float *kernel_, float *sub_area_, int &k_size_,
                    float sigma_, float gauss_tail_,
                    int max_width_, int &width_)
{
  const float s2 = 2.0f*sigma_*sigma_;

  for (int i=0; i<max_width_; ++i)  {
    width_ = i;                             // half Kernel width

    // the value of kernel_[i] is the average of the gaussian over
    // 11 points evenly spaced on the interval [i-0.5, i+0.5].
    kernel_[i] = 0.0;
    for (float x=i-0.5f; x<=i+0.5f; x+=0.1f)
      kernel_[i] += (float)std::exp(-x*x/s2);
    kernel_[i] /= 11.0f;

    if (i>0 && kernel_[i] < gauss_tail_)
      break;
  }

  // compute area under half-kernel.
  float area = 0.0f;
  for (int i=0; i<width_; ++i)
    area += kernel_[i];

  // Total area under whole profile curve.
  float total_area = 2.0f*area - kernel_[0];

  for (int i=0; i<width_; ++i)  {
    sub_area_[i] = (total_area - area)/total_area;
    area -= kernel_[i];
    kernel_[i] /= total_area;
  }

  // kernel size
  k_size_ = 2*width_ - 1;
}
