#ifndef mil_algo_exp_filter_1d_h_
#define mil_algo_exp_filter_1d_h_
//:
//  \file
//  \brief Apply exponential filter to 1D data (eg one row of an image)
//  \author Tim Cootes

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
void mil_algo_exp_filter_1d(float* dest, int dstep,
                            const unsigned char* src, int sstep,
                            int n, double k);

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
void mil_algo_exp_filter_1d(float* dest, int dstep,
                            const float* src, int sstep,
                            int n, double k);

//: Apply exponential filter to 1D data
//  Apply filter to n values src[i*sstep] to produce output dest[i*dstep]
//  Symmetric exponential filter of the form exp(c*|x|) applied. c=log(k)
//  Uses fast recursive implementation.
void mil_algo_exp_filter_1d(unsigned char* dest, int dstep,
                            const unsigned char* src, int sstep,
                            int n, double k);
#endif
