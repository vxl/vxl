#ifndef osl_kernel_h_
#define osl_kernel_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME osl_kernel
// .INCLUDE osl/osl_kernel.h
// .FILE osl_kernel.cxx
// \author fsm@robots.ox.ac.uk

//: Compute one half of the 1st Derivative Of a Gaussian.
// '_k_size' is the actual size of the array pointed to by '_kernel'.
void osl_kernel_DOG(float _sigma, float *_kernel, int _k_size, int _width);

//: Fancy version
void osl_kernel_DOG(float *_kernel, float *_sub_area_OX, int &_k_size,
                    float _sigma, float _gauss_tail,
                    int _max_width_OX, int &_width);

#endif // osl_kernel_h_
