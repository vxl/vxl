#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iomanip.h>

#if (defined(VCL_GCC_295) || defined(VCL_EGCS)) && !defined(GNU_LIBSTDCXX_V3)

#if !VCL_HAS_TEMPLATE_SYMBOLS // not needed except with -fguiding-decls (fsm)
//template class smanip<int>;
template ostream & operator<<(ostream &, smanip<int> const &);
#endif

#endif



#if defined(VCL_SUNPRO_CC_50)
ostream &operator<<(ostream &s, short x)
{ return s << int(x); }
#endif
