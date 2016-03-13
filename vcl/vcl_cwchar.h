#ifndef vcl_cwchar_h_
#define vcl_cwchar_h_

#include "vcl_compiler.h"
#include <cwchar>
//vcl alias names to std names
#define vcl_mbstate_t std::mbstate_t
#define vcl_wint_t std::wint_t
// NB: size_t is declared in <cstddef>, not <cwchar>
#define vcl_btowc std::btowc
#define vcl_fgetwc std::fgetwc
#define vcl_fgetws std::fgetws
#define vcl_fputwc std::fputwc
#define vcl_fputws std::fputws
#define vcl_fwide std::fwide
#define vcl_fwprintf std::fwprintf
#define vcl_fwscanf std::fwscanf
#define vcl_getwc std::getwc
#define vcl_getwchar std::getwchar
#define vcl_mbrlen std::mbrlen
#define vcl_mbrtowc std::mbrtowc
#define vcl_putwc std::putwc
#define vcl_putwchar std::putwchar
#define vcl_swprintf std::swprintf
#define vcl_swscanf std::swscanf
#define vcl_ungetwc std::ungetwc
#define vcl_vfwprintf std::vfwprintf
#define vcl_vswprintf std::vswprintf
#define vcl_vwprintf std::vwprintf
#define vcl_wcrtomb std::wcrtomb
#define vcl_wcscat std::wcscat
#define vcl_wcschr std::wcschr
#define vcl_wcscmp std::wcscmp
#define vcl_wcscoll std::wcscoll
#define vcl_wcscpy std::wcscpy
#define vcl_wcscspn std::wcscspn
#define vcl_wcsftime std::wcsftime
#define vcl_wcslen std::wcslen
#define vcl_wcsncat std::wcsncat
#define vcl_wcsncmp std::wcsncmp
#define vcl_wcsncpy std::wcsncpy
#define vcl_wcspbrk std::wcspbrk
#define vcl_wcsrchr std::wcsrchr
#define vcl_wcsrtombs std::wcsrtombs
#define vcl_wcsspn std::wcsspn
#define vcl_wcsstr std::wcsstr
#define vcl_wcstod std::wcstod
#define vcl_wcstok std::wcstok
#define vcl_wcstol std::wcstol
#define vcl_wctoul std::wctoul
#define vcl_wcsxfrm std::wcsxfrm
#define vcl_wctob std::wctob
#define vcl_wmemchr std::wmemchr
#define vcl_wmemcmp std::wmemcmp
#define vcl_wmemcpy std::wmemcpy
#define vcl_wmemmove std::wmemmove
#define vcl_wmemset std::wmemset
#define vcl_wprintf std::wprintf
#define vcl_wscanf std::wscanf

#endif // vcl_cwchar_h_
