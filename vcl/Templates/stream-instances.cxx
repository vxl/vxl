#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iomanip.h>
#include <vcl/vcl_fstream.h>

#if defined(VCL_EGCS)
# if !VCL_HAS_TEMPLATE_SYMBOLS
//template class smanip<int>;
template ostream & operator<<(ostream &, smanip<int> const &);
# endif
#endif

#if defined(VCL_GCC_295)
# if defined(GNU_LIBSTDCXX_V3)
#  include <bits/istream.tcc>
#  include <bits/ostream.tcc>
// for some reason these templates aren't in libstdc++ (yet).
template class std::basic_fstream<char, std::char_traits<char> >;
template class std::basic_ifstream<char, std::char_traits<char> >;
//template class std::basic_ofstream<char, std::char_traits<char> >;
namespace {
  void tic(std::ostream &s, int x) { s << std::setw(14) << x; }
  void toc(std::basic_ostream<char, std::char_traits<char> > &s,
	   std::fpos<__mbstate_t> x) { s.seekp(x); }
  void tac(std::basic_istream<char, std::char_traits<char> > &s,
	   std::fpos<__mbstate_t> x) { s.seekg(x); }
}
# else
#  if !VCL_HAS_TEMPLATE_SYMBOLS
//template class smanip<int>;
template ostream & operator<<(ostream &, smanip<int> const &);
#  endif
# endif
#endif



#if defined(VCL_SUNPRO_CC_50)
ostream &operator<<(ostream &s, short x)
{ return s << int(x); }
#endif
