#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.h>

#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
// gcc2.95 needs implicit templates tickled to link strings....

template class basic_string<char, string_char_traits<char> >;
template ostream& operator<<(ostream&, vcl_string const&);

#define concat(x, y) x##y

namespace {
  void f(ostream& os) {
    char a;
    string_char_traits<char>::eq(a,a);
    concat(str,ing) s;
    os << s;

    vcl_string("foo", "bar");
  }
}

#endif
