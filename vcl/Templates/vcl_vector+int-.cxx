#include <vcl/vcl_iostream.h>

#include <vcl/vcl_iterator.h>
#include <vcl/vcl_iterator.txx>

#include <vcl/vcl_vector.h>
#include <vcl/vcl_vector.txx>

#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_algorithm.txx>

VCL_VECTOR_INSTANTIATE(int);

#if defined(VCL_EGCS)
template vcl_vector<int>::iterator max_element(vcl_vector<int>::iterator ,vcl_vector<int>::iterator );
template vcl_vector<int>::iterator find(vcl_vector<int>::iterator ,vcl_vector<int>::iterator , const int &);
template vcl_vector<int>::iterator find(vcl_vector<int>::iterator ,vcl_vector<int>::iterator , const unsigned &);
#else
VCL_COPY_INSTANTIATE(vcl_vector<int>::iterator, vcl_ostream_iterator<int>);
#endif

#if defined(VCL_GCC_295)
template int * fill_n(int *, int, int const &);
#endif
