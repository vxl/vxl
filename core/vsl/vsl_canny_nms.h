#ifndef vsl_canny_nms_h_
#define vsl_canny_nms_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_canny_nms
// .INCLUDE vsl/vsl_canny_nms.h
// .FILE vsl_canny_nms.cxx
// \author fsm@robots.ox.ac.uk


int vsl_canny_nms(int _xsize, int _ysize, 
		  float * const * _dx, float * const * _dy, float const * const * _grad,
		  float * const *_thick, float * const * _theta);

#endif // vsl_canny_nms_h_
