#ifndef vil_resample_image_h_
#define vil_resample_image_h_

// Purpose: Support function for vil_resample_image_impl.

class vil_image;

// T: pixel type
// A: accumulator type
template <class T, class A>
bool vil_resample_image(vil_image const &base, unsigned new_width, unsigned new_height,
                  T *buf, A *dummy,
                  int x0, int y0, int w, int h);

#endif // vil_resample_image_h_
