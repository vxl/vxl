/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_kernel.h"
#include <vcl_cmath.h>

// Construct one half of a Gaussian convolution kernel.
//   
//   _kernel[i] = exp( (i-_width)^2/sigma^2 )/det
void vsl_kernel_DOG(float _sigma, float *_kernel, int _k_size, int _width) {
  float s2 = 2.0*_sigma*_sigma;
  float det = _sigma*vcl_sqrt(2.0 * 3.1415926);
  
  for (int i=0,x=-_width; i<_k_size; ++i,++x) 
    _kernel[i] = vcl_exp(-x*x/s2)/det;
}

// Construct one half of a Gaussian convolution kernel.
// With fancy stuff.
void vsl_kernel_DOG(float *_kernel, float *_sub_area, int &_k_size,
		    float _sigma, float _gauss_tail,
		    int _max_width, int &_width) 
{
  const float s2 = 2.0*_sigma*_sigma;
  
  for (int i=0; i<_max_width; ++i)  { 
    _width = i;                             // half Kernel width

    // the value of _kernel[i] is the average of the gaussian over
    // 11 points evenly spaced on the interval [i-0.5, i+0.5].
    _kernel[i] = 0.0;
    for (float x=i-0.5; x<=i+0.5; x+=0.1) 
      _kernel[i] += vcl_exp(-x*x/s2);
    _kernel[i] /= 11.0;

    if (i>0 && _kernel[i] < _gauss_tail)
      break;
  }
  
  // compute area under half-kernel.
  float area = 0.0f; 
  for (int i=0; i<_width; ++i)
    area += _kernel[i];

  // Total area under whole profile curve.
  float total_area = 2.0*area - _kernel[0]; 

  for (int i=0; i<_width; ++i)  { 
    _sub_area[i] = (total_area - area)/total_area;
    area -= _kernel[i];
    _kernel[i] /= total_area;
  }

  // kernel size
  _k_size = 2*_width - 1;
}

