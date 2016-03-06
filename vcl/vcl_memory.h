#ifndef vcl_memory_h_
#define vcl_memory_h_

#include "vcl_compiler.h"
#include <memory>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
//no dependancies remove comment above
//vcl alias names to std names
// [20.4] lib.memory
#define vcl_allocator std::allocator
#define vcl_raw_storage_iterator std::raw_storage_iterator
#define vcl_get_temporary_buffer std::get_temporary_buffer
#define vcl_return_temporary_buffer std::return_temporary_buffer
#define vcl_uninitialized_copy std::uninitialized_copy
#define vcl_uninitialized_fill std::uninitialized_fill
#define vcl_uninitialized_fill_n std::uninitialized_fill_n
#define vcl_auto_ptr std::auto_ptr

#endif // vcl_memory_h_
