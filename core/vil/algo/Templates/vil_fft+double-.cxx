#if (defined _MSC_VER) && _MSC_VER == 1200
// disable warning buried deep in the standard library
// warning C4018: '<' : signed/unsigned mismatch: vector(159)
# pragma warning(disable: 4018)
#endif

#include <vil/algo/vil_fft.txx>
VIL_FFT_INSTANTIATE(double);
