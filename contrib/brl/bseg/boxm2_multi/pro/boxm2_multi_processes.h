#ifndef boxm2_processes_h_
#define boxm2_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm2_multi_render_process);
DECLARE_FUNC_CONS(boxm2_multi_update_process);
DECLARE_FUNC_CONS(boxm2_create_multi_cache_process);
DECLARE_FUNC_CONS(boxm2_write_multi_cache_process);
#endif
