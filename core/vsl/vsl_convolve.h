#ifndef vsl_convolve_h_
#define vsl_convolve_h_

#include <vsl/vsl_1d_half_kernel.h>
#include <vsl/vsl_roi_window.h>
#include <vil/vil_memory_image_of.h>

// fancy :
void vsl_convolve  (vsl_roi_window const *window,
		    vsl_roi_window *new_window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

// plain :
void vsl_convolve2 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

void vsl_convolve3 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

void vsl_convolve4 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image,
		    vil_float_buffer *scratch);

void vsl_convolve5 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

void vsl_convolve6 (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

void vsl_convolven (vsl_roi_window const *window, 
		    vsl_1d_half_kernel<double> const *mask,
		    vil_float_buffer *image, 
		    vil_float_buffer *scratch);

#endif
