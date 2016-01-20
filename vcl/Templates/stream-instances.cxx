// This is vcl/Templates/stream-instances.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#if defined(VCL_SUNPRO_CC_50)
vcl_ostream &operator<<(vcl_ostream &s, short x)
{ return s << int(x); }

# include <string>
//void blah(vcl_istream &s1, std::string &s2) { s1 >> s2; }
//
#define type0 std::basic_istream<char,std::char_traits<char> >
#define type1 std::basic_string<char,std::char_traits<char>,std::allocator<char> >
#define type2 std::char_traits<char>

template type0 & __rwstd::rw_extract_string<type0, type1, type2>(type0 &, type1 &, type2);

#undef type0
#undef type1
#undef type2

#endif
