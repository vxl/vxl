#ifndef bvrml_processes_h_
#define bvrml_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(bvrml_initialize_process);
DECLARE_FUNC_CONS(bvrml_write_box_process);
DECLARE_FUNC_CONS(bvrml_write_perspective_cam_process);
DECLARE_FUNC_CONS(bvrml_write_origin_and_axes_process);
DECLARE_FUNC_CONS(bvrml_write_point_process);

#endif
