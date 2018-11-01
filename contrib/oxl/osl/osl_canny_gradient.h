// This is oxl/osl/osl_canny_gradient.h
#ifndef osl_canny_gradient_h_
#define osl_canny_gradient_h_
//:
// \file
// \author fsm

//: compute x,y-derivatives and absolute value of gradient.
// xsize_ is the number of rows [sic] and
// ysize_ the number of columns [sic].
void osl_canny_gradient(int xsize_, int ysize_,
                        float const * const * smooth_,
                        float * const * dx_,
                        float * const * dy_,
                        float * const * grad_);

//: computes doubled central derivatives : df[i] = f[i+1]-f[i-1].
// the boundary pixels are left untouched.
void osl_canny_gradient_central(int xsize_, int ysize_,
                                float const * const * smooth_,
                                float * const * dx_,
                                float * const * dy_,
                                float * const * grad_);

#endif // osl_canny_gradient_h_
