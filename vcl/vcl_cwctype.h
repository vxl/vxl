#ifndef vcl_cwctype_h_
#define vcl_cwctype_h_

#include "vcl_compiler.h"
#include <cwctype>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_iswalnum std::iswalnum
#define vcl_iswalpha std::iswalpha
#define vcl_iswcntrl std::iswcntrl
#define vcl_iswctrans std::iswctrans
#define vcl_iswctype std::iswctype
#define vcl_iswdigit std::iswdigit
#define vcl_iswgraph std::iswgraph
#define vcl_iswlower std::iswlower
#define vcl_iswprint std::iswprint
#define vcl_iswpunct std::iswpunct
#define vcl_iswspace std::iswspace
#define vcl_iswupper std::iswupper
#define vcl_iswxdigit std::iswxdigit
#define vcl_towctrans std::towctrans
#define vcl_towlower std::towlower
#define vcl_towupper std::towupper
#define vcl_wctrans std::wctrans
#define vcl_wctrans_t std::wctrans_t
#define vcl_wctype std::wctype
#define vcl_wctype_t std::wctype_t
#define vcl_wint_t std::wint_t

#endif // vcl_cwctype_h_
