#include <vcl_algorithm.txx>
#include <vcl_cstddef.h> // for vcl_size_t

VCL_SORT_INSTANTIATE(unsigned*, unsigned);
VCL_FIND_INSTANTIATE(unsigned*, unsigned);
VCL_FIND_INSTANTIATE(unsigned const*, unsigned);

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template unsigned *unique(unsigned *, unsigned *);
#endif

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
template unsigned* fill_n(unsigned*, int, int const &);
#endif
