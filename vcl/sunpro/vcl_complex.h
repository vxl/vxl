#ifndef vcl_sunpro_complex_h_
#define vcl_sunpro_complex_h_

//# include <vcl/vcl_functional.h> // get !=(T,T) declared. so that !=(complex<T>..) specializes

# include <complex>              // get compiler <complex>
# define vcl_complex std::complex
using std :: sqrt;
using std :: arg;
using std :: polar;
using std :: conj;
using std :: real;
using std :: imag;
using std :: norm;
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

#endif // vcl_sunpro_complex_h_
