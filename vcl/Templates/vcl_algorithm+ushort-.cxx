#include <vcl_algorithm.txx>

typedef unsigned short ushort;

VCL_SORT_INSTANTIATE(ushort*, ushort);
VCL_FIND_INSTANTIATE(ushort*, ushort);
VCL_FIND_INSTANTIATE(ushort const*, ushort);

#ifdef GNU_LIBSTDCXX_V3
# include <vcl_vector.h>
VCL_SORT_INSTANTIATE(vcl_vector<ushort>::iterator, ushort);
VCL_SORT_INSTANTIATE(vcl_vector<ushort*>::iterator, ushort*);
namespace std {
  template ushort* std::fill_n<ushort*, unsigned long, ushort>(ushort*, unsigned long, ushort const&);
}
#endif

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template ushort* unique(ushort *, ushort *);
#endif

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
template ushort* fill_n(ushort*, unsigned int, ushort const &);
#endif
