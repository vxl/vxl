#ifndef vcl_memory_h_
#define vcl_memory_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
  20.4 [lib.memory]
*/

#if !VCL_USE_NATIVE_STL
// ?

#else // assuming working <memory>
# include <memory>
# define vcl_allocator               std :: allocator
# define vcl_raw_storage_iterator    std :: raw_storage_iterator
# define vcl_get_temporary_buffer    std :: get_temporary_buffer
# define vcl_return_temporary_buffer std :: return_temporary_buffer
# define vcl_uninitialized_copy      std :: uninitialized_copy
# define vcl_uninitialized_fill      std :: uninitialized_fill
# define vcl_uninitialized_fill_n    std :: uninitialized_fill_n
# define vcl_auto_ptr                std :: auto_ptr
#endif

#endif
