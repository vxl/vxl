#ifndef vidl_processes_h_
#define vidl_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(vidl_open_istream_process);
DECLARE_FUNC_CONS(vidl_open_ostream_process);
DECLARE_FUNC_CONS(vidl_get_frame_istream_process);
DECLARE_FUNC_CONS(vidl_put_frame_ostream_process);
DECLARE_FUNC_CONS(vidl_close_istream_process);
DECLARE_FUNC_CONS(vidl_close_ostream_process);
#if defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
DECLARE_FUNC_CONS(vidl_open_dshow_istream_process);
#endif

#endif
