// fsm: I could have used vil1_rgb_byte instead, but it's the
// principle that counts. This is how clients must instantiate their
// own personal vil1_memory_image_of<T>

#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.hxx>

typedef unsigned char byte;

VIL1_MEMORY_IMAGE_OF_INSTANTIATE(vil1_rgb<byte>);
