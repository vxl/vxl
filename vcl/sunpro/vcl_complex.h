#ifndef vcl_sunpro_complex_h_
#define vcl_sunpro_complex_h_

#include "../iso/vcl_complex.h"

// Override these, as the SunPro compiler doesn't declare them as
// partial specializations.
inline bool operator!=(const vcl_complex<double>& a, const vcl_complex<double>& b) {
  return !(a == b);
}
inline bool operator!=(const vcl_complex<float>& a, const vcl_complex<float>& b) {
  return !(a == b);
}

// The const& on the double arg appears to be necessary to avoid an overloading
// conflict with complex<double> * double.
inline std:: complex<double> operator*(vcl_complex<float> const& a, const double& b) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}
inline std:: complex<double> operator*(double const& b, vcl_complex<float> const& a) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}

#undef  vcl_abs
#define vcl_abs vcl_abs
inline float  vcl_abs(vcl_complex<float>  z) { return std::abs(z); }
inline double vcl_abs(vcl_complex<double> z) { return std::abs(z); }

#endif // vcl_sunpro_complex_h_
