#ifndef vcl_iso_cstring_h_
#define vcl_iso_cstring_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <cstring>

// strcpy
#ifndef vcl_strcpy
#define vcl_strcpy std::strcpy
#endif
// strcat
#ifndef vcl_strcat
#define vcl_strcat std::strcat
#endif
// strlen
#ifndef vcl_strlen
#define vcl_strlen std::strlen
#endif
// memcpy
#ifndef vcl_memcpy
#define vcl_memcpy std::memcpy
#endif
// memset
#ifndef vcl_memset
#define vcl_memset std::memset
#endif
// memmove
#ifndef vcl_memmove
#define vcl_memmove std::memmove
#endif

#endif
