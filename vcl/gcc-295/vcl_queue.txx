#include <vcl_queue.h>

#undef VCL_QUEUE_INSTANTIATE
#define VCL_QUEUE_INSTANTIATE(T) \
template class vcl_queue<T >

#undef VCL_PRIORITY_QUEUE_INSTANTIATE
#define VCL_PRIORITY_QUEUE_INSTANTIATE(I, T, C) \
template class vcl_priority_queue<T, I, C >; \
template void __push_heap<T*, int, T, C >(T*, int, int, T, C); \
template void __adjust_heap<T*, int, T, C >(T*, int, int, T, C)
