#include <vcl/vcl_string.txx>
#include <vcl/vcl_iostream.h>

VCL_BASIC_STRING_INSTANTIATE(char, vcl_char_traits<char> );

#if defined(VCL_SUNPRO_CC)
template class vcl_basic_string<char, vcl_char_traits<char> >;
#endif



#if defined(VCL_EGCS)
# define bs basic_string<char, string_char_traits<char>, __default_alloc_template<1, 0> >
template bs &bs::replace(char *, char *, char *, char *);
template bs &bs::replace(char *, char *, char const *, char const *);
template bs &bs::replace(size_t, size_t, bs const &, size_t, size_t);
template bs &bs::replace(size_t, size_t, char const *, size_t);
template bs &bs::replace(size_t, size_t, size_t, char);
# undef bs
#endif



#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3) && !defined(VCL_EGCS) && !defined(VCL_GCC_27)
void vcl_string_instance_tickler(ostream &os, char *a, char const *b)
{
  char ch;
  vcl_char_traits<char>::eq(ch, ch);
  vcl_string s(b, b);
  os << s;
  s.replace(a, a, a, a);
  s.replace(a, a, b, b);
}

#if !VCL_HAS_TEMPLATE_SYMBOLS // not needed except with -fguiding-decls (fsm)
// this is defined in libstdc++ for egcs. it causes multiply
// defined symbols on static builds. -- fsm
//template class __default_alloc_template<true, 0>;
# define bs basic_string<char, string_char_traits<char>, __default_alloc_template<true, 0> >
template bs &bs::replace(char *, char *, char *, char *);
template bs &bs::replace(char *, char *, char const *, char const *);
template bs &bs::replace(size_t, size_t, bs const &, size_t, size_t);
template bs &bs::replace(size_t, size_t, char const *, size_t);
template bs &bs::replace(size_t, size_t, size_t, char);
# undef bs
// this is defined in libstdc++ for egcs. it causes multiply
// defined symbols on static builds. -- fsm
//template class __default_alloc_template<false, 0>;
# define bs basic_string<char, string_char_traits<char>, __default_alloc_template<false, 0> >
template bs &bs::replace(char *, char *, char *, char *);
template bs &bs::replace(char *, char *, char const *, char const *);
template bs &bs::replace(size_t, size_t, bs const &, size_t, size_t);
template bs &bs::replace(size_t, size_t, char const *, size_t);
template bs &bs::replace(size_t, size_t, size_t, char);
# undef bs
#endif

#endif
