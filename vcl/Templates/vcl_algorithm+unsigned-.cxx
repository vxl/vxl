#include <vcl/vcl_algorithm.txx>

VCL_SORT_INSTANTIATE(unsigned*, unsigned);

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template unsigned *unique(unsigned *, unsigned *);
#endif
