#ifndef vil_clamp_image_h_
#define vil_clamp_image_h_

// Purpose: Support function for vil_clamp_image_impl.

class vil_image;

template <class T>
bool vil_clamp_image(vil_image const &base, double low, double high, 
		     T *buf, int x0, int y0, int w, int h);

#endif
