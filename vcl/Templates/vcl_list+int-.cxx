#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.h>

#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>

VCL_LIST_INSTANTIATE(int);

VCL_COPY_INSTANTIATE(vcl_list<int>::const_iterator, vcl_ostream_iterator<int>);
VCL_COPY_INSTANTIATE(vcl_list<int>::iterator, vcl_ostream_iterator<int>);

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
//fsm: the instantiation macro for find() needs to instantiate find(I, I, T, tag)
//for noshared builds. however, there seems to be no way to get the iterator 
//category of I other than using iterator_traits<I>::iterator_category. the 
//problem then is that find() is only defined for input_iterators and random_access_iterators.
//since a bidirectional_iterator_tag is an input_iterator, the following should
//be harmless.
//it could be moved into vcl_algorithm.txx
template <class _BdIter, class _Tp>
inline _BdIter find(_BdIter __first, _BdIter __last,
                    _Tp const & __val,
		    bidirectional_iterator_tag)
{
  return ::find(__first, __last, __val, input_iterator_tag());
}
#endif
VCL_FIND_INSTANTIATE_ITER(vcl_list<int>::iterator, int);


#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
//vxl_filter:skip
template void list<int, allocator<int> >::_M_insert_dispatch(_List_iterator<int, int &, int *>, _List_iterator<int, int &, int *>, _List_iterator<int, int &, int *>, __false_type);
#endif
