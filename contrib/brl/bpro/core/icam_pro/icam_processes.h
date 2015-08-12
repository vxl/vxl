#ifndef icam_processes_h_
#define icam_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(icam_create_view_sphere_process);
DECLARE_FUNC_CONS(icam_register_image_process);
DECLARE_FUNC_CONS(icam_correct_cam_rotation_process);

#endif
