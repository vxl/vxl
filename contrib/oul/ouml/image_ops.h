#ifndef image_ops__INCLUDED
#define image_ops__INCLUDED

#include <vil1/vil1_memory_image_of.h>

template <class T>
vil1_memory_image_of<T> &operator -=
(
  vil1_memory_image_of<T> &im1,
  vil1_memory_image_of<T> &im2
);

template <class T>
vil1_memory_image_of<T> &operator +=
(
  vil1_memory_image_of<T> &im1,
  T constant_add
);

template <class T>
void threshold_abs(vil1_memory_image_of<T> &image, 
                   vil1_memory_image_of<T> &source_vals,
                   T threshold, T zero_val);

#endif // image_ops__INCLUDED
