#if (defined _MSC_VER) && _MSC_VER == 1200
// disable warning buried deep in the standard library
// warning C4018: '<' : signed/unsigned mismatch: vector(159)
# pragma warning(disable: 4018)
#endif
#include <vcl_complex.h>
#include <vcl_vector.txx>

VCL_VECTOR_INSTANTIATE(vcl_complex<float>);
