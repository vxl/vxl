#ifndef image_ops__INCLUDED
#define image_ops__INCLUDED

#include <vil1/vil1_memory_image_of.h>
#include "image_convert.h"

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

template <class T>
T max_val(const vil1_memory_image_of<T> &image, T min);

template <class T>
T min_val(const vil1_memory_image_of<T> &image, T max);

template <class T>
vil1_memory_image_of<T> *normalise_image(const vil1_memory_image_of<T> &src, 
                                         T low, T high, T min, T max,
                                         T epsilon=(T)0);

typedef vil1_memory_image_of<int> IntImage;
typedef vil1_memory_image_of<vxl_byte> ByteImage;

inline void save_intimage_asbyte(vil1_memory_image_of<int> &src,
                                 char *filename)
{
  IntImage *normal = normalise_image(src, 0, 255, INT_MIN, INT_MAX); 
  vxl_byte b;
  ByteImage *byte_im = convert_image(*normal, b);
  vil1_save(*byte_im, filename);
  delete byte_im;
  delete normal;
}

/*
 * don't do normalisation in this version
 */
inline void save_intimage_asbyte_nonorm(vil1_memory_image_of<int> &src,
                                        char *filename)
{
  vxl_byte b;
  ByteImage *byte_im = convert_image(src, b);
  vil1_save(*byte_im, filename);
  delete byte_im;
}

#endif // image_ops__INCLUDED
