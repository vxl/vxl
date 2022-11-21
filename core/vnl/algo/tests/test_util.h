#ifndef test_util_h_
#define test_util_h_
#include <Eigen/Core>

class vnl_random;
#include <complex>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#define tumacro(T) void test_util_fill_random(T *begin, T *end, vnl_random &rng)

tumacro(float);
tumacro(std::complex<float>);
tumacro(double);
tumacro(std::complex<double>);
#ifdef INCLUDE_LONG_DOUBLE_TESTS
tumacro(long double);
tumacro(std::complex<long double>);
#endif
#undef tumacro

#endif
