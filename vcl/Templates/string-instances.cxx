#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.txx>

#ifdef VCL_GCC_295
// gcc2.95 needs implicit templates tickled to link strings....

template class basic_string<char, string_char_traits<char> >;
template ostream& operator<<(ostream&, vcl_string const&);

namespace {
  void f(ostream& os) {
    char a;
    string_char_traits<char>::eq(a,a);
    vcl_string s;
    os << s;
  }
}

#endif

VCL_BASIC_STRING_INSTANTIATE(char, vcl_string_char_traits<char>);
