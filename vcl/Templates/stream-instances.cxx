/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iomanip.h>

#if defined(VCL_GCC_295) || defined(VCL_EGCS)
#if !defined(GNU_LIBSTDCXX_V3)
//template class smanip<int>;
template ostream & operator<<(ostream &, smanip<int> const &);
#endif
#endif

#if defined(VCL_SUNPRO_CC_50)
ostream &operator<<(ostream &s, short x)
{ return s << int(x); }
#endif
