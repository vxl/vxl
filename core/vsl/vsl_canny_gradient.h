#ifndef vsl_canny_gradient_h_
#define vsl_canny_gradient_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_canny_gradient
// .INCLUDE vsl/vsl_canny_gradient.h
// .FILE vsl_canny_gradient.cxx
// \author fsm@robots.ox.ac.uk

//: compute x,y-derivatives and absolute value of gradient.
// _xsize is the number of rows [sic] and
// _ysize the number of columns [sic].
void vsl_canny_gradient(int _xsize, int _ysize, 
			float const * const * _smooth,
			float * const * _dx, 
			float * const * _dy, 
			float * const * _grad);

//: computes doubled central derivatives : df[i] = f[i+1]-f[i-1].
// the boundary pixels are left untouched.
void vsl_canny_gradient_central(int _xsize, int _ysize, 
				float const * const * _smooth,
				float * const * _dx, 
				float * const * _dy, 
				float * const * _grad);

#endif
