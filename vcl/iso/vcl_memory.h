#ifndef vcl_iso_memory_h_
#define vcl_iso_memory_h_

#include <memory>

// [20.4] lib.memory

// allocator
#ifndef vcl_allocator
#define vcl_allocator std::allocator
#endif
// raw_storage_iterator
#ifndef vcl_raw_storage_iterator
#define vcl_raw_storage_iterator std::raw_storage_iterator
#endif
// get_temporary_buffer
#ifndef vcl_get_temporary_buffer
#define vcl_get_temporary_buffer std::get_temporary_buffer
#endif
// return_temporary_buffer
#ifndef vcl_return_temporary_buffer
#define vcl_return_temporary_buffer std::return_temporary_buffer
#endif
// uninitialized_copy
#ifndef vcl_uninitialized_copy
#define vcl_uninitialized_copy std::uninitialized_copy
#endif
// uninitialized_fill
#ifndef vcl_uninitialized_fill
#define vcl_uninitialized_fill std::uninitialized_fill
#endif
// uninitialized_fill_n
#ifndef vcl_uninitialized_fill_n
#define vcl_uninitialized_fill_n std::uninitialized_fill_n
#endif
// auto_ptr
#ifndef vcl_auto_ptr
#define vcl_auto_ptr std::auto_ptr
#endif

#endif // vcl_iso_memory_h_
