#ifndef osl_canny_nms_h_
#define osl_canny_nms_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME osl_canny_nms
// .INCLUDE osl/osl_canny_nms.h
// .FILE osl_canny_nms.cxx
// \author fsm@robots.ox.ac.uk


int osl_canny_nms(int _xsize, int _ysize,
                  float * const * _dx, float * const * _dy, float const * const * _grad,
                  float * const *_thick, float * const * _theta);

#endif // osl_canny_nms_h_
