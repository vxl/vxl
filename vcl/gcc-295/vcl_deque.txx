#ifndef vcl_gcc295_deque_cxx_
#define vcl_gcc295_deque_cxx_

#include <vcl_deque.h>
#include <vcl_functional.h> // for greater<T>
#include <vcl_algorithm.h> // for __push_heap, __adjust_heap

#undef  VCL_DEQUE_INSTANTIATE
#define VCL_DEQUE_INSTANTIATE(T) \
template class vcl_deque<T >; \
template class _Deque_base<T, allocator<T >, 0>; \
template void __push_heap<_Deque_iterator<T,T&,T*,0>,T,T,greater<T > >(_Deque_iterator<T,T&,T*,0>,T,T,T,greater<T >); \
template void __adjust_heap<_Deque_iterator<T,T&,T*,0>,T,T,greater<T > >(_Deque_iterator<T,T&,T*,0>,T,T,T,greater<T >)

#endif
