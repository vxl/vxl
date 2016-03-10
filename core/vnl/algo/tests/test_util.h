#ifndef test_util_h_
#define test_util_h_

class vnl_random;
#include <vcl_compiler.h>
#include <complex>
#define macro(T) void test_util_fill_random(T *begin, T *end, vnl_random &rng)
macro(float);
macro(double);
macro(long double);
macro(std::complex<float>);
macro(std::complex<double>);
macro(std::complex<long double>);
#undef macro

#endif
