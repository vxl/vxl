#include "icam_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "icam_processes.h"
#include <icam/icam_view_sphere_sptr.h>

void icam_register::register_datatype()
{
  REGISTER_DATATYPE( icam_view_sphere_sptr );
}

void icam_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, icam_create_view_sphere_process, "icamCreateViewSphereProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, icam_register_image_process, "icamRegisterImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, icam_correct_cam_rotation_process, "icamCorrectCamRotationProcess");
}
