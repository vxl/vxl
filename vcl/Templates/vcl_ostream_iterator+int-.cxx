#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.txx>

#if !defined(WIN32) && !defined(GNU_LIBSTDCXX_V3) 
template class vcl_ostream_iterator<int>;
#endif
