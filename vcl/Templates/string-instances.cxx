#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.h>

#ifdef VCL_GCC_295
// gcc2.95 needs implicit templates tickled to link strings....

template class basic_string<char, string_char_traits<char> >;
template ostream& operator<<(ostream&, vcl_string const&);

namespace {
  void f(ostream& os) {
    char a;
    string_char_traits<char>::eq(a,a);
    string s;
    os << s;
  }
}

#endif
