#include <vcl/vcl_string.txx>

//template class basic_string<char, string_char_traits<char> >;
VCL_BASIC_STRING_INSTANTIATE(char, vcl_string_char_traits<char> );


#include <vcl/vcl_iostream.h>

#if defined(VCL_GCC_27)

#elif defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
// gcc2.95 needs implicit templates tickled to link strings....

template ostream& operator<<(ostream&, vcl_string const&);

# define concat(x, y) x##y
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

#ifdef VCL_GCC_295
template
basic_string<char, string_char_traits<char>, __default_alloc_template<0, 0> > & basic_string<char, string_char_traits<char>, __default_alloc_template<0, 0> >::replace<char *>(char *, char *, char *, char *);
#endif
