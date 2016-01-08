#include "boxm2_view_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <vpgl/vpgl_camera.h>

#include "boxm2_view_processes.h"

void boxm2_view_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_scene_sptr );
  REGISTER_DATATYPE( vpgl_camera_double_sptr );
  REGISTER_DATATYPE( boxm2_trajectory_sptr );
}

void boxm2_view_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_expected_image_process,"boxm2ViewExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_init_trajectory_process,"boxm2ViewInitTrajectoryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_trajectory_next_process,"boxm2ViewTrajectoryNextProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_trajectory_direct_process,"boxm2ViewTrajectoryDirectProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_trajectory_size_process,"boxm2ViewTrajectorySizeProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_init_regular_trajectory_process, "boxm2ViewInitRegularTrajectoryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_init_height_map_trajectory_process, "boxm2ViewInitHeightMapTrajectoryProcess");
}
