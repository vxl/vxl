
#include <vcl/vcl_complex.h>

#include <std/complext.cc>

#define VCL_COMPLEX_INSTANTIATE_INLINE(T) template T

#define VCL_COMPLEX_INSTANTIATE(FLOAT)\
VCL_COMPLEX_INSTANTIATE_INLINE(bool operator==(complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(bool operator==(FLOAT,complex<FLOAT >const&));	\
VCL_COMPLEX_INSTANTIATE_INLINE(bool operator==(complex<FLOAT >const&,FLOAT));	\
VCL_COMPLEX_INSTANTIATE_INLINE(FLOAT imag(complex<FLOAT >const&));		\
VCL_COMPLEX_INSTANTIATE_INLINE(FLOAT real(complex<FLOAT >const&));		\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > sqrt (complex<FLOAT >const& x));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator + (complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator + (complex<FLOAT >const&,FLOAT));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator + (FLOAT,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator - (complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator - (complex<FLOAT >const&,FLOAT));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator - (FLOAT,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator * (complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator * (complex<FLOAT >const&,FLOAT));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator * (FLOAT,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator / (complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator / (complex<FLOAT >const&,FLOAT));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > operator / (FLOAT,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > polar (FLOAT,FLOAT));		\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > pow (complex<FLOAT >const&,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > pow (complex<FLOAT >const&,FLOAT));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > pow (complex<FLOAT >const&,int));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > pow (FLOAT,complex<FLOAT >const&));\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > exp (complex<FLOAT >const&));	\
VCL_COMPLEX_INSTANTIATE_INLINE(complex<FLOAT > log (complex<FLOAT >const&));	\
VCL_COMPLEX_INSTANTIATE_INLINE(FLOAT arg (complex<FLOAT >const&));		\
VCL_COMPLEX_INSTANTIATE_INLINE(FLOAT abs (complex<FLOAT >const&));		\
VCL_COMPLEX_INSTANTIATE_INLINE(FLOAT norm (complex<FLOAT >const&)); \
template ostream& operator<<(ostream &, complex<FLOAT > const &);
