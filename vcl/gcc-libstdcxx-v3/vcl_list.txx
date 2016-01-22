#ifndef vcl_gcc_libstdcxx_v3_list_txx_
#define vcl_gcc_libstdcxx_v3_list_txx_

#include <vcl_list.h>

#undef VCL_LIST_INSTANTIATE
#define VCL_LIST_INSTANTIATE(T) \
template class vcl_list<T >; \
/* a member template : */ \
template void \
std::list<T >::_M_insert_dispatch(std::list<T >::iterator,std::list<T >::iterator,std::list<T >::iterator,_Bool<false >)

#endif // vcl_gcc_libstdcxx_v3_list_txx_
