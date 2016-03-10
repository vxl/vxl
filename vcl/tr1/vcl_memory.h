#ifndef vcl_tr1_memory_h_
#define vcl_tr1_memory_h_

#include "vcl_compiler.h"
// [20.6] lib.memory (additions in 0x draft: 2006-11-06)
#include <tr1/memory>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
#define vcl_bad_weak_ptr std::bad_weak_ptr
#define vcl_shared_ptr std::shared_ptr
#define vcl_swap std::swap
#define vcl_static_pointer_cast std::static_pointer_cast
#define vcl_dynamic_pointer_cast std::dynamic_pointer_cast
#define vcl_const_pointer_cast std::const_pointer_cast
#define vcl_get_deleter std::get_deleter
#define vcl_weak_ptr std::weak_ptr
#define vcl_enable_shared_from_this std::enable_shared_from_this

#endif // vcl_tr1_memory_h_
