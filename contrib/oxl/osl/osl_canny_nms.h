#ifndef osl_canny_nms_h_
#define osl_canny_nms_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME osl_canny_nms
// .INCLUDE osl/osl_canny_nms.h
// .FILE osl_canny_nms.cxx
// \author fsm@robots.ox.ac.uk


unsigned int osl_canny_nms(int xsize_, int ysize_,
                           float * const * dx_, float * const * dy_, float const * const * grad_,
                           float * const *thick_, float * const * theta_);

#endif // osl_canny_nms_h_
