#include <vcl/vcl_string.txx>
#include <vcl/vcl_iostream.h>

VCL_BASIC_STRING_INSTANTIATE(char, vcl_char_traits<char> );

#if defined(VCL_EGCS)
# define bs basic_string<char, string_char_traits<char>, __default_alloc_template<1, 0> >
template bs &bs::replace(char *, char *, char *, char *);
template bs &bs::replace(char *, char *, char const *, char const *);
template bs &bs::replace(unsigned, unsigned, bs const &, unsigned, unsigned);
template bs &bs::replace(unsigned, unsigned, char const *, unsigned);
template bs &bs::replace(unsigned, unsigned, unsigned, char);
# undef bs
#endif

#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
void vcl_string_instance_tickler(ostream &os)
{
  char a;
  vcl_char_traits<char>::eq(a,a);
  vcl_string s("foo", "bar");
  os << s;
}
# define bs basic_string<char, string_char_traits<char>, __default_alloc_template<false, 0> >
template bs &bs::replace(char *, char *, char *, char *);
template bs &bs::replace(char *, char *, char const *, char const *);
template bs &bs::replace(unsigned, unsigned, bs const &, unsigned, unsigned);
template bs &bs::replace(unsigned, unsigned, char const *, unsigned);
template bs &bs::replace(unsigned, unsigned, unsigned, char);
# undef bs
#endif
