#ifndef vcl_cstring_h_
#define vcl_cstring_h_

#include "vcl_compiler.h"
#include <cstring>
//vcl alias names to std names
// NB: size_t is declared in <cstddef>, not <cstring>
#define vcl_memchr std::memchr
#define vcl_memcmp std::memcmp
#define vcl_memcpy std::memcpy
#define vcl_memmove std::memmove
#define vcl_memset std::memset
#define vcl_strcat std::strcat
#define vcl_strchr std::strchr
#define vcl_strcmp std::strcmp
#define vcl_strcoll std::strcoll
#define vcl_strcpy std::strcpy
#define vcl_strcspn std::strcspn
#define vcl_strerror std::strerror
#define vcl_strlen std::strlen
#define vcl_strncat std::strncat
#define vcl_strncmp std::strncmp
#define vcl_strncpy std::strncpy
#define vcl_strpbrk std::strpbrk
#define vcl_strrchr std::strrchr
#define vcl_strspn std::strspn
#define vcl_strstr std::strstr
#define vcl_strtok std::strtok
#define vcl_strxfrm std::strxfrm

#endif // vcl_cstring_h_
