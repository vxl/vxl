#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.h>
#include <vcl/vcl_string.txx>

// We only want the instances to see this
#ifdef __SUNPRO_CC
template <class charT, class traits>
const size_t basic_string <charT, traits>::npos = (size_t)(-1);
#endif

//template class vcl_basic_string<char, vcl_string_char_traits<char> >;
//VCL_BASIC_STRING_INSTANTIATE(char, vcl_string_char_traits<char>);
