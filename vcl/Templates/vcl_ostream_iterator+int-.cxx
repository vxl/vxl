#include <vcl_iterator.txx>

#if !defined(VCL_SGI_CC) && !defined(VCL_WIN32) && !defined(GNU_LIBSTDCXX_V3) && !defined(__ICC)
template class vcl_ostream_iterator<int>;
#endif
