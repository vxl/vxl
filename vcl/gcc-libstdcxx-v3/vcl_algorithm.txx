// -*- c++ -*-
#ifndef vcl_gcc_libstdcxx_v3_algorithm_txx_
#define vcl_gcc_libstdcxx_v3_algorithm_txx_

#include <vcl/vcl_algorithm.h>

#define VCL_SWAP_INSTANTIATE(T)
 
#define VCL_OPERATOR_NE_INSTANTIATE(T)

#define VCL_CONTAINABLE_INSTANTIATE(T)
 
#define VCL_SORT_INSTANTIATE(I, T) \
namespace std {template void sort(I, I);}

#define VCL_SORT_INSTANTIATE_CMP(I, T, C)

#define VCL_COPY_INSTANTIATE(Inp, Out)

#define VCL_FIND_INSTANTIATE(I, T)

#define VCL_FIND_IF_INSTANTIATE(I, P)

#define VCL_REMOVE_INSTANTIATE(I, T)


#endif

