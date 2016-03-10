#ifndef vcl_locale_h_
#define vcl_locale_h_

#include "vcl_compiler.h"
#include <locale>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_use_facet std::use_facet
#define vcl_has_facet std::has_facet
#define vcl_isspace std::isspace
#define vcl_isprint std::isprint
#define vcl_iscntrl std::iscntrl
#define vcl_isupper std::isupper
#define vcl_islower std::islower
#define vcl_isalpha std::isalpha
#define vcl_isdigit std::isdigit
#define vcl_ispunct std::ispunct
#define vcl_isxdigit std::isxdigit
#define vcl_isalnum std::isalnum
#define vcl_isgraph std::isgraph
#define vcl_toupper std::toupper
#define vcl_tolower std::tolower
#define vcl_ctype std::ctype
#define vcl_codecvt_base std::codecvt_base
#define vcl_codecvt std::codecvt
#define vcl_codecvt_byname std::codecvt_byname
#define vcl_num_get std::num_get
#define vcl_num_put std::num_put
#define vcl_numpunct std::numpunct
#define vcl_numpunct_byname std::numpunct_byname
#define vcl_collate std::collate
#define vcl_collate_byname std::collate_byname
#define vcl_time_get std::time_get
#define vcl_time_get_byname std::time_get_byname
#define vcl_time_put std::time_put
#define vcl_time_put_byname std::time_put_byname
#define vcl_money_get std::money_get
#define vcl_money_put std::money_put
#define vcl_moneypunct std::moneypunct
#define vcl_moneypunct_byname std::moneypunct_byname
#define vcl_messages std::messages
#define vcl_messages_byname std::messages_byname

#endif // vcl_locale_h_
