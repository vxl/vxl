// This file is supposed to define any template instances needed
// to give a sensible complex type.
// e.g. ensure that "operator/(complex<float>, float)" exists
// it is in Templates because it may need implicit templates to work properly

#include <vcl/vcl_iostream.h> 
#include <vcl/vcl_complex.txx> 

//#if 0
#if defined(VCL_GCC_27) || defined(VCL_GCC_EGCS)
// ---------- emulation
#if !VCL_USE_NATIVE_COMPLEX
// ** make sure gcc 2.7 sees this **
VCL_COMPLEX_INSTANTIATE(float);
VCL_COMPLEX_INSTANTIATE(double);
#endif

// ---------- egcs and gcc 2.95
#elif defined(VCL_GCC_295) || defined(VCL_GCC_EGCS)
# define implement_rsh(T) \
istream &operator>>(istream &is, vcl_complex<T > &z) { \
  T r, i; \
  is >> r >> i; \
  z = vcl_complex<T >(r, i); \
  return is; \
}
# define F(x) template x
# define do_inlines(FLOAT) \
      F(bool operator==(complex<FLOAT >const&,complex<FLOAT >const&));\
      F(bool operator==(FLOAT,complex<FLOAT >const&));	\
      F(bool operator==(complex<FLOAT >const&,FLOAT));	\
      F(FLOAT imag(complex<FLOAT >const&));		\
      F(FLOAT real(complex<FLOAT >const&));		\
      F(complex<FLOAT > sqrt (complex<FLOAT >const& x));\
      F(complex<FLOAT > operator + (complex<FLOAT >const&,complex<FLOAT >const&));\
      F(complex<FLOAT > operator + (complex<FLOAT >const&,FLOAT));\
      F(complex<FLOAT > operator + (FLOAT,complex<FLOAT >const&));\
      F(complex<FLOAT > operator - (complex<FLOAT >const&,complex<FLOAT >const&));\
      F(complex<FLOAT > operator - (complex<FLOAT >const&,FLOAT));\
      F(complex<FLOAT > operator - (FLOAT,complex<FLOAT >const&));\
      F(complex<FLOAT > operator * (complex<FLOAT >const&,complex<FLOAT >const&));\
      F(complex<FLOAT > operator * (complex<FLOAT >const&,FLOAT));\
      F(complex<FLOAT > operator * (FLOAT,complex<FLOAT >const&));\
      F(complex<FLOAT > operator / (complex<FLOAT >const&,complex<FLOAT >const&));\
      F(complex<FLOAT > operator / (complex<FLOAT >const&,FLOAT));\
      F(complex<FLOAT > operator / (FLOAT,complex<FLOAT >const&));\
      F(complex<FLOAT > polar (FLOAT,FLOAT));		\
      F(complex<FLOAT > pow (complex<FLOAT >const&,complex<FLOAT >const&));\
      F(complex<FLOAT > pow (complex<FLOAT >const&,FLOAT));\
      F(complex<FLOAT > pow (complex<FLOAT >const&,int));\
      F(complex<FLOAT > pow (FLOAT,complex<FLOAT >const&));\
      F(complex<FLOAT > exp (complex<FLOAT >const&));	\
      F(complex<FLOAT > log (complex<FLOAT >const&));	\
      F(FLOAT arg (complex<FLOAT >const&));		\
      F(FLOAT abs (complex<FLOAT >const&));		\
      F(FLOAT norm (complex<FLOAT >const&)); \
      F(complex<FLOAT>& __doadv (complex<FLOAT>* ths, const complex<FLOAT>& y)); \
      template ostream& operator<<(ostream &, complex<FLOAT > const &);
//#pragma implementation "fcomplex"
//VCL_COMPLEX_INSTANTIATE(float);
do_inlines(float); implement_rsh(float);
//#pragma implementation "dcomplex"
//VCL_COMPLEX_INSTANTIATE(double);
do_inlines(double); implement_rsh(double);
#if 0
// tickle implicit templates :
namespace {
  void foo() {
    complex<float> *a, *b;
    (*a) += (*b);
    (*a) -= (*b);
    (*a) *= (*b);
    (*a) /= (*b);
  }
};
#endif

// ---------- sunpro
#elif defined(VCL_SUNPRO_CC)
template std::complex<double> std::conj<double>(std::complex<double> const &);

#endif
