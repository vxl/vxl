#include <vcl_algorithm.txx>

VCL_SORT_INSTANTIATE(unsigned*, unsigned);
VCL_FIND_INSTANTIATE(unsigned*, unsigned);
VCL_FIND_INSTANTIATE(unsigned const*, unsigned);

#ifdef GNU_LIBSTDCXX_V3
# include <vcl_vector.h>
VCL_SORT_INSTANTIATE(vcl_vector<unsigned>::iterator, unsigned);
VCL_SORT_INSTANTIATE(vcl_vector<unsigned*>::iterator, unsigned*);
namespace std {
  template unsigned* std::fill_n<unsigned*, unsigned long, unsigned>(unsigned*, unsigned long, unsigned const&);
}
#endif

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template unsigned *unique(unsigned *, unsigned *);
#endif

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
template unsigned int* fill_n(unsigned int*, int, int const &);
#endif
