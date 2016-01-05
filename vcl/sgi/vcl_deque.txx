#ifndef vcl_sgi_deque_txx_
#define vcl_sgi_deque_txx_

#undef VCL_DEQUE_INSTANTIATE
#define VCL_DEQUE_INSTANTIATE(T) \
template class std::deque<T,std::__default_alloc_template<true,0>, 0U>

#endif // vcl_sgi_deque_txx_
