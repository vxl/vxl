#ifndef boxm2_view_processes_h_
#define boxm2_view_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm2_view_expected_image_process);
DECLARE_FUNC_CONS(boxm2_view_init_trajectory_process);
DECLARE_FUNC_CONS(boxm2_view_trajectory_next_process);
DECLARE_FUNC_CONS(boxm2_view_trajectory_direct_process);
DECLARE_FUNC_CONS(boxm2_view_trajectory_size_process);
DECLARE_FUNC_CONS(boxm2_view_init_regular_trajectory_process);
DECLARE_FUNC_CONS(boxm2_view_init_height_map_trajectory_process);

#endif
