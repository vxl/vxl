#ifndef image_ops__INCLUDED
#define image_ops__INCLUDED

#include <vil/vil_memory_image_of.h>

template <class T>
vil_memory_image_of<T> &operator -=
(
  vil_memory_image_of<T> &im1,
  vil_memory_image_of<T> &im2
);

template <class T>
vil_memory_image_of<T> &operator +=
(
  vil_memory_image_of<T> &im1,
  T constant_add
);

template <class T>
void threshold_abs(vil_memory_image_of<T> &image, 
                   vil_memory_image_of<T> &source_vals,
                   T threshold, T zero_val);

#endif // image_ops__INCLUDED
