// fsm: I could have used vil_rgb_byte instead, but it's the
// principle that counts. This is how clients must instantiate their
// own personal vil_memory_image_of<T>

#include <vil/vil_rgb.h>
#include <vil/vil_memory_image_of.txx>

typedef unsigned char byte;

VIL_MEMORY_IMAGE_OF_INSTANTIATE(vil_rgb<byte>);
