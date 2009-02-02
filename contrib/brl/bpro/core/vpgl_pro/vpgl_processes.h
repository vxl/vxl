#ifndef vpgl_processes_h_
#define vpgl_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(vpgl_load_proj_camera_process);
DECLARE_FUNC_CONS(vpgl_load_rational_camera_process);
DECLARE_FUNC_CONS(vpgl_load_rational_camera_nitf_process);
DECLARE_FUNC_CONS(vpgl_load_perspective_camera_process);
DECLARE_FUNC_CONS(vpgl_save_perspective_camera_process);
DECLARE_FUNC_CONS(vpgl_save_rational_camera_process);
DECLARE_FUNC_CONS(vpgl_nitf_camera_coverage_process);

#endif
