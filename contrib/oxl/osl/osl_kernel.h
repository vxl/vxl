// This is oxl/osl/osl_kernel.h
#ifndef osl_kernel_h_
#define osl_kernel_h_
//:
// \file
// \author fsm

//: Compute one half of the 1st Derivative Of a Gaussian.
// 'k_size_' is the actual size of the array pointed to by 'kernel_'.
void osl_kernel_DOG(float sigma_, float *kernel_, int k_size_, int width_);

//: Fancy version
void osl_kernel_DOG(float *kernel_, float *sub_area_OX_, int &k_size_,
                    float sigma_, float gauss_tail_,
                    int max_width_OX_, int &width_);

#endif // osl_kernel_h_
