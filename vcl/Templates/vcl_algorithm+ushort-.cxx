#include <vcl_algorithm.txx>
#include <vcl_cstddef.h> // for vcl_size_t

VCL_SORT_INSTANTIATE(unsigned short*, unsigned short);
VCL_FIND_INSTANTIATE(unsigned short*, unsigned short);
VCL_FIND_INSTANTIATE(unsigned short const*, unsigned short);

#if defined(VCL_EGCS) && !defined(GNU_LIBSTDCXX_V3)
template unsigned *unique(unsigned short *, unsigned short *);
#endif

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
template unsigned short* fill_n(unsigned short*, unsigned int, unsigned short const &);
#endif
