#include <vcl_deque.txx>

VCL_DEQUE_INSTANTIATE(int);
#if !defined(VCL_USE_IMPLICIT_TEMPLATES) && defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
VCL_DEQUE_INSTANTIATE_HEAP(int);
#endif
