#include <vcl/vcl_string.txx>
#include <vcl/vcl_iostream.h>

VCL_BASIC_STRING_INSTANTIATE(char, vcl_char_traits<char> );

#if defined(VCL_SUNPRO_CC)
template class vcl_basic_string<char, vcl_char_traits<char> >;
#endif

// this should work for all compilers. by putting it in the
// library we (a) get the implicit template instances it 
// needs and (b) make sure that it does work.
void vcl_string_instance_tickler(ostream &os, char *a, char const *b)
{
  char ch;
  vcl_char_traits<char>::eq(ch, ch);
  vcl_string s(b, b);
  os << s;
  s.replace(a, a, a, a);
  s.replace(a, a, b, b);
}


#if defined(VCL_GCC_295) && !defined(GNU_LIBSTDCXX_V3)
# if !VCL_HAS_TEMPLATE_SYMBOLS
// The following is needed when using -fguiding-decls.

#  define inst \
template class __default_alloc_template<true, 0>; \
template bs &bs::replace(char *, char *, char *, char *); \
template bs &bs::replace(char *, char *, char const *, char const *); \
template bs &bs::replace(size_t, size_t, bs const &, size_t, size_t); \
template bs &bs::replace(size_t, size_t, char const *, size_t); \
template bs &bs::replace(size_t, size_t, size_t, char)

#  undef bs
#  define bs basic_string<char, string_char_traits<char>, __default_alloc_template<true , 0> >
inst;

#  undef bs
#  define bs basic_string<char, string_char_traits<char>, __default_alloc_template<false, 0> >
inst;

#  undef inst
# endif
#endif
