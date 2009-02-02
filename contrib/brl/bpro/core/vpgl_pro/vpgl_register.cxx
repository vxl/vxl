#include "vpgl_register.h"
#include "vpgl_processes.h"
#include <vpgl/vpgl_camera.h>

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void vpgl_register::register_datatype()
{
  typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;
  REGISTER_DATATYPE( vpgl_camera_double_sptr );
}

void vpgl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_perspective_camera_process, "vpglLoadPerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_proj_camera_process, "vpglLoadProjCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_rational_camera_nitf_process, "vpglLoadRationalCameraNITFProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_rational_camera_process, "vpglLoadRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_nitf_camera_coverage_process, "vpglNITFCameraCoverageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_camera_process, "vpglSavePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_rational_camera_process, "vpglSaveRetionalCameraProcess");
}

