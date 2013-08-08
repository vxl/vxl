#include "bvrml_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bvrml_processes.h"

void bvrml_register::register_datatype()
{

}

void bvrml_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_initialize_process, "bvrmlInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_write_box_process, "bvrmlWriteBoxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_write_perspective_cam_process, "bvrmlWritePerspectiveCamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_write_origin_and_axes_process, "bvrmlWriteOriginAndAxesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_write_point_process, "bvrmlWritePointProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_write_line_process, "bvrmlWriteLineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_filtered_ply_process, "bvrmlFilteredPlyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvrml_image_to_points_process, "bvrmlImageToPointsProcess");
}
