#ifndef vsl_canny_nms_h_
#define vsl_canny_nms_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/


int vsl_canny_nms(int _xsize, int _ysize, 
		  float * const * _dx, float * const * _dy, float const * const * _grad,
		  float * const *_thick, float * const * _theta);

#endif
