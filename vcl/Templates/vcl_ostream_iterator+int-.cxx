#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.txx>

#ifndef WIN32
template class vcl_ostream_iterator<int>;
#endif
