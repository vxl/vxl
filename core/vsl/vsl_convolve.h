#ifndef vsl_convolve_h_
#define vsl_convolve_h_
// .NAME vsl_convolve
// .INCLUDE vsl/vsl_convolve.h
// .FILE vsl_convolve.cxx

#include <vsl/vsl_1d_half_kernel.h>
#include <vsl/vsl_roi_window.h>
#include <vil/vil_memory_image_of.h>

// this routine will select the appropriate routine from the ones
// listed below.
void vsl_convolve  (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);

// there's no good reason why these should be available to clients.
#if 1
void vsl_convolve2 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);

void vsl_convolve3 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);

void vsl_convolve4 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch);

void vsl_convolve5 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);

void vsl_convolve6 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);

void vsl_convolven (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image, 
		    vil_memory_image_of<float> *scratch);
#endif

#endif // vsl_convolve_h_
