#include "mil_algo_exp_filter_1d.h"
//:
//  \file
//  \brief Apply exponential filter to 1D data (eg one row of an image)
//  \author Tim Cootes

#include <vnl/vnl_math.h>

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
void mil_algo_exp_filter_1d(float* dest, int dstep,
                            const unsigned char* src, int sstep,
                            int n, double k)
{
  const unsigned char* s = src;
  const unsigned char* src_end = src + n*sstep;
  double f = (1-k)/(1+k);

  // Forward pass
  double rt=0;
  while (s!=src_end)
  {
    rt += *s;
    *dest = float(f * rt);
    rt *= k;
    s+=sstep; dest+=dstep;
  }

  // Backward pass
  s-=sstep; dest-=dstep;
  src_end = src-sstep;
  rt=0;
  while (s!=src_end)
  {
    // Central value already included once, so only add it after updating dest.
    *dest += float(f * rt);
    rt += *s;
    rt *= k;
    s-=sstep; dest-=dstep;
  }
}

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
void mil_algo_exp_filter_1d(float* dest, int dstep,
                            const float* src, int sstep,
                            int n, double k)
{
  const float* s = src;
  const float* src_end = src + n*sstep;
  double f = (1-k)/(1+k);

  // Forward pass
  double rt=0;
  while (s!=src_end)
  {
    rt += *s;
    *dest = float(f * rt);
    rt *= k;
    s+=sstep; dest+=dstep;
  }

  // Backward pass
  s-=sstep; dest-=dstep;
  src_end = src-sstep;
  rt=0;
  while (s!=src_end)
  {
    // Central value already included once, so only add it after updating dest.
    *dest += float(f * rt);
    rt += *s;
    rt *= k;
    s-=sstep; dest-=dstep;
  }
}

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
void mil_algo_exp_filter_1d(unsigned char* dest, int dstep,
                            const unsigned char* src, int sstep,
                            int n, double k)
{
  const unsigned char* s = src;
  const unsigned char* src_end = src + n*sstep;
  double f = (1-k)/(1+k);

  // Forward pass
  double rt=0;
  while (s!=src_end)
  {
    rt += *s;
    *dest = (unsigned char) vnl_math_rnd(f * rt);
    rt *= k;
    s+=sstep; dest+=dstep;
  }

  // Backward pass
  s-=sstep; dest-=dstep;
  src_end = src-sstep;
  rt=0;
  while (s!=src_end)
  {
    // Central value already included once, so only add it after updating dest.
    *dest += (unsigned char) vnl_math_rnd(f * rt);
    rt += *s;
    rt *= k;
    s-=sstep; dest-=dstep;
  }
}
