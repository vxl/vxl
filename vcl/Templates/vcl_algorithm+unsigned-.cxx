#include <vcl/vcl_algorithm.txx>

VCL_SORT_INSTANTIATE(unsigned*, unsigned);

#if defined(VCL_GCC_295) || defined(VCL_EGCS)
template unsigned *unique(unsigned *, unsigned *);
#endif
