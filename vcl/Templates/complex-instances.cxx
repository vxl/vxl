// This file is supposed to define any template instances needed
// to give a sensible complex type.

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_complex.h> 
#include <vcl/vcl_complex.txx> 

#if !VCL_USE_NATIVE_COMPLEX
  VCL_COMPLEX_INSTANTIATE(float);
  VCL_COMPLEX_INSTANTIATE(double);
#elif defined(VCL_GCC_295)
# include <vcl/vcl_iostream.h> 
  //fsm@robots: where, if at all, is this declared?
# define implement_rsh(T) \
       istream &operator>>(istream &is, vcl_complex<T > &z) { \
       T r, i; \
       is >> r >> i; \
       z = vcl_complex<T >(r, i); \
       return is; \
       }
  implement_rsh(float);
  implement_rsh(double);
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
      template ostream& operator<<(ostream &, complex<FLOAT > const &);
  do_inlines(float)
  do_inlines(double)


#endif
