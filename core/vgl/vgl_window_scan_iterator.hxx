// This is core/vgl/vgl_window_scan_iterator.hxx
#ifndef vgl_window_scan_iterator_hxx_
#define vgl_window_scan_iterator_hxx_

#include "vgl_window_scan_iterator.h"

#undef VGL_WINDOW_SCAN_ITERATOR_INSTANTIATE
#define VGL_WINDOW_SCAN_ITERATOR_INSTANTIATE(T) \
template class vgl_window_scan_iterator<T >

#endif // vgl_window_scan_iterator_hxx_
