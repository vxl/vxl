#ifndef vil_scale_intensities_image_h_
#define vil_scale_intensities_image_h_

// Purpose: Support function for vil_scale_intensities_image_impl.

class vil_image;

template <class T>
bool vil_scale_intensities_image(vil_image const &base, double scale, double shift, 
				 T *buf, int x0, int y0, int w, int h);

#endif
