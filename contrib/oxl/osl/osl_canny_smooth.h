#ifndef osl_canny_smooth_h_
#define osl_canny_smooth_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME osl_canny_smooth
// .INCLUDE osl/osl_canny_smooth.h
// .FILE osl_canny_smooth.cxx
// .FILE osl_canny_smooth.txx
// \author fsm@robots.ox.ac.uk

#include <vil/vil_memory_image_of.h>

#ifdef __SUNPRO_CC
# define unpro_const /*const*/
#else
# define unpro_const const
#endif

// from (Rothwell) Canny
void osl_canny_smooth_rothwell(vil_image const &image,
                               float const *_kernel, int _width, int _k_size,
                               float * const *_smooth);

template <class T>
void osl_canny_smooth_rothwell(T const *const *in, int _xsize, int _ysize,
                               float const *_kernel, int _width, int _k_size,
                               float * unpro_const *_smooth);

void osl_canny_smooth_rothwell_adaptive(vil_image const &image,
                                        int x0, int y0, int image_size,
                                        float const *_kernel, int _width, int _k_size,
                                        float * const *dx, float * const *dy, float * const *grad);

template <class T>
void osl_canny_smooth_rothwell_adaptive(T const * const *in, int _xsize, int _ysize,
                                        int x0, int y0, int image_size,
                                        float const *_kernel, int _width, int _k_size,
                                        float * unpro_const *dx, float * unpro_const *dy, float * unpro_const *grad);

// from CannyOX
void osl_canny_smooth(vil_image const &in,
                      float const *_kernel, int _width, float const *_sub_area_OX,
                      float * const * image_out);

template <class T>
void osl_canny_smooth(T const * const *in, int _xsize, int _ysize,
                      float const *_kernel, int _width, float const *_sub_area_OX,
                      float * unpro_const * image_out);

#endif // osl_canny_smooth_h_
