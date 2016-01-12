#ifndef image_convert__INCLUDED
#define image_convert__INCLUDED

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

template <class T1, class T2>
vil1_memory_image_of<T2> *convert_image
(
  const vil1_memory_image_of<T1> &from,
  T2 &var
);

template <> vil1_memory_image_of<unsigned char> *
convert_image<vil1_rgb<unsigned char>, unsigned char>
(
  const vil1_memory_image_of<vil1_rgb<unsigned char> > &from,
  unsigned char &var
);

template <> vil1_memory_image_of<double> *
convert_image<vil1_rgb<unsigned char>, double>
(
  const vil1_memory_image_of<vil1_rgb<unsigned char> > &from,
  double &var
);

#endif // image_convert__INCLUDED
