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

#include <vil/vil_image.h>

#ifdef __SUNPRO_CC
# define unpro_const /*const*/
#else
# define unpro_const const
#endif

// from (Rothwell) Canny
void osl_canny_smooth_rothwell(vil_image const &image,
                               float const *kernel_, int width_, int k_size_,
                               float * const *smooth_);

template <class T>
void osl_canny_smooth_rothwell(T const *const *in, int xsize_, int ysize_,
                               float const *kernel_, int width_, int k_size_,
                               float * unpro_const *smooth_);

void osl_canny_smooth_rothwell_adaptive(vil_image const &image,
                                        int x0, int y0, int image_size,
                                        float const *kernel_, int width_, int k_size_,
                                        float * const *dx, float * const *dy, float * const *grad);

template <class T>
void osl_canny_smooth_rothwell_adaptive(T const * const *in, int xsize_, int ysize_,
                                        int x0, int y0, int image_size,
                                        float const *kernel_, int width_, int k_size_,
                                        float * unpro_const *dx, float * unpro_const *dy, float * unpro_const *grad);

// from CannyOX
void osl_canny_smooth(vil_image const &in,
                      float const *kernel_, int width_, float const *sub_area_OX_,
                      float * const * image_out);

template <class T>
void osl_canny_smooth(T const * const *in, int xsize_, int ysize_,
                      float const *kernel_, int width_, float const *sub_area_OX_,
                      float * unpro_const * image_out);

#endif // osl_canny_smooth_h_
