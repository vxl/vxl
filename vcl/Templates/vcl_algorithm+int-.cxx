#include <vcl_iterator.h>
#include <vcl_algorithm.txx>

VCL_SWAP_INSTANTIATE(int);

VCL_SORT_INSTANTIATE(int *, int);
VCL_SORT_INSTANTIATE(int**, int*);
VCL_FIND_INSTANTIATE(int *, int);
#if 0
VCL_FIND_INSTANTIATE(int *, unsigned);
#endif
VCL_COPY_INSTANTIATE(int *, vcl_ostream_iterator<int>);
#include <vcl_list.h>
VCL_COPY_INSTANTIATE(vcl_list<int>::const_iterator, vcl_ostream_iterator<int>);
VCL_COPY_INSTANTIATE(vcl_list<int>::iterator, vcl_ostream_iterator<int>);
VCL_FIND_INSTANTIATE(vcl_list<int>::iterator, int);
#ifdef GNU_LIBSTDCXX_V3
#include <vcl_vector.h>
VCL_FIND_INSTANTIATE(vcl_vector<int>::iterator, int);
#if 0
VCL_FIND_INSTANTIATE(vcl_vector<int>::iterator, unsigned);
#endif
namespace std
{
  template int * std::fill_n<int *, int, int>(int *, int, int const &);
}
#endif

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template int * max_element(int * , int * );
#endif

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
template int * fill_n(int *, int, int const &);
template int * unique(int *, int *);
#endif
