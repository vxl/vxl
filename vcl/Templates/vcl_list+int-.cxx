#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.h>

#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>

VCL_LIST_INSTANTIATE(int);

VCL_COPY_INSTANTIATE(vcl_list<int>::const_iterator, vcl_ostream_iterator<int>);
VCL_COPY_INSTANTIATE(vcl_list<int>::iterator, vcl_ostream_iterator<int>);

VCL_FIND_INSTANTIATE(vcl_list<int>::iterator, int);

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
//vxl_filter:skip
template void list<int, allocator<int> >::_M_insert_dispatch(_List_iterator<int, int &, int *>, _List_iterator<int, int &, int *>, _List_iterator<int, int &, int *>, __false_type);
#endif
