#ifndef osl_convolve_h_
#define osl_convolve_h_
// .NAME osl_convolve
// .INCLUDE osl/osl_convolve.h
// .FILE osl_convolve.cxx

#include <osl/osl_1d_half_kernel.h>
#include <osl/osl_roi_window.h>
#include <vil1/vil1_memory_image_of.h>

// this routine will select the appropriate routine from the ones
// listed below.
void osl_convolve  (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

// there's no good reason why these should be available to clients.
#if 1
void osl_convolve2 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

void osl_convolve3 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

void osl_convolve4 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

void osl_convolve5 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

void osl_convolve6 (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);

void osl_convolven (osl_roi_window const *window,
                    osl_1d_half_kernel<double> const *mask,
                    vil1_memory_image_of<float> *image,
                    vil1_memory_image_of<float> *scratch);
#endif

#endif // osl_convolve_h_
