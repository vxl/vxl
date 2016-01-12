#ifndef vcl_sgi_complex_txx_
#define vcl_sgi_complex_txx_

#undef VCL_COMPLEX_INSTANTIATE
#include <vcl_iostream.h>
#define VCL_COMPLEX_INSTANTIATE(T) \
template vcl_ostream& operator<<(vcl_ostream&,const std::complex<T >&)

#endif // vcl_sgi_complex_txx_
