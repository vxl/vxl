#ifndef vcl_gcc_295_set_txx_
#define vcl_gcc_295_set_txx_

#include <vcl/vcl_set.h>
#include <vcl/vcl_compiler.h>


#define VCL_SET_INSTANTIATE_ITERATOR(InputIterator, Distance)\
template void distance (InputIterator , InputIterator , Distance& ) 

#define VCL_SET_INSTANTIATE(T, Comp)\
template class vcl_set<T, Comp >; \
VCL_SET_INSTANTIATE_ITERATOR(vcl_set<T VCL_COMMA Comp >::iterator, unsigned)

#endif
