#ifndef vsl_kernel_h_
#define vsl_kernel_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: Compute one half of the 1st Derivative Of a Gaussian.
// '_k_size' is the actual size of the array pointed to by '_kernel'.
void vsl_kernel_DOG(float _sigma, float *_kernel, int _k_size, int _width);

//: Fancy version
void vsl_kernel_DOG(float *_kernel, float *_sub_area_OX, int &_k_size,
		    float _sigma, float _gauss_tail,
		    int _max_width_OX, int &_width);

#endif
