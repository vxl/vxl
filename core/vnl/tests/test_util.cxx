
#include <vcl_complex.h>
#include <vnl/vnl_sample.h>

#define macro(T) \
void test_util_fill_random(T *b, T *e) \
{ \
  for (T *p=b; p<e; ++p) \
    *p = vnl_sample_uniform(-1, +1); \
} \
void test_util_fill_random(vcl_complex<T> *b, vcl_complex<T> *e) \
{ \
  for (vcl_complex<T> *p=b; p<e; ++p) \
    *p = vcl_complex<T>(vnl_sample_uniform(-1, +1), vnl_sample_uniform(-1, +1)); \
}
macro(float);
macro(double);
macro(long double);
#undef macro
