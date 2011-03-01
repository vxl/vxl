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
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_local_rational_camera_process, "vpglLoadLocalRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_nitf_camera_coverage_process, "vpglNITFCameraCoverageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_camera_process, "vpglSavePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_rational_camera_process, "vpglSaveRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_local_rational_camera_process, "vpglCreateLocalRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_local_rational_camera_nitf_process, "vpglCreateLocalRationalCameraNITFProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_projective_to_rational_camera_process, "vpglProjectiveToRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_scale_perspective_camera_process, "vpglScalePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_camera_process, "vpglCorrectRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process, "vpglCreatePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_camera_vrml_process, "vpglSavePerspectiveCameraVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_process, "vpglCorrectRationalCamerasProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_mult_corr_process, "vpglCorrectRationalCamerasMultCorrProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_mult_corr_refine_process, "vpglCorrectRationalCamerasMultCorrRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_project_process, "vpglProjectProcess");
}

