// This is ./vxl/vnl/vnl_rational.cxx
#include <vnl/vnl_rational.h>

//:
// \file

vnl_rational::vnl_rational(double d)
{
  bool sign = d<0;
  if (sign) d = -d;

  // Continued fraction approximation of abs(d): recursively determined
  long den=0L, num=1L, prev_den=1L, prev_num=0L;

  while (d*num < 1e9 && d*den < 1e9) {
    long a = (long)d; // integral part of d
    d -= a; // certainly >= 0
    long temp = num; num = a*num + prev_num; prev_num = temp;
         temp = den; den = a*den + prev_den; prev_den = temp;
    if (d < 1e-6) break;
    d = 1/d;
  }
  num_ = num; den_ = den;
  if (sign) num_ = -num_;
  // no need to normalize() since prev_num and prev_den have guaranteed a gcd=1
}

const vnl_rational vnl_numeric_traits<vnl_rational>::zero = vnl_rational(0L,1L);
const vnl_rational vnl_numeric_traits<vnl_rational>::one = vnl_rational(1L,1L);

const vcl_complex<vnl_rational> vnl_numeric_traits<vcl_complex<vnl_rational> >::zero
  = vcl_complex<vnl_rational>(vnl_rational(0L,1L),vnl_rational(0L,1L));
const vcl_complex<vnl_rational> vnl_numeric_traits<vcl_complex<vnl_rational> >::one
  = vcl_complex<vnl_rational>(vnl_rational(1L,1L),vnl_rational(0L,1L));

#ifdef VCL_GCC_EGCS // this includes egcs, gcc 2.8 and gcc 2.95
#include <std/complext.cc>
template complex<vnl_rational>& __doadv<vnl_rational>(complex<vnl_rational>*, complex<vnl_rational> const&);
template complex<vnl_rational> operator/(complex<vnl_rational> const&, complex<vnl_rational> const&);
#endif
