#if (defined _MSC_VER) && _MSC_VER == 1200
// disable warning buried deep in the standard library
// warning C4018: '<' : signed/unsigned mismatch: vector(159)
# pragma warning(disable: 4018)
#endif

#include <vcl_complex.h>
#include <vil1/io/vil1_io_memory_image_of.txx>

VIL1_IO_MEMORY_IMAGE_OF_INSTANTIATE(vcl_complex<float>);
