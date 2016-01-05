#ifndef vcl_sgi_vector_txx_
#define vcl_sgi_vector_txx_

#include <vcl_vector.h>

#define VCL_VECTOR_STLINST_uninitialized_copy(Inp, Fwd, Size) \
template Fwd std::copy(Inp, Inp, Fwd);\
template Fwd std::copy_backward(Inp, Inp, Fwd)

// --- Vector ---
#undef VCL_VECTOR_INSTANTIATE
#define VCL_VECTOR_INSTANTIATE(T) \
template class std::vector<T,std::__default_alloc_template<true,0> >

#endif // vcl_sgi_vector_txx_
