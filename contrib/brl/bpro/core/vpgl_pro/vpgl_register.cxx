#include "vpgl_register.h"
#include "vpgl_load_perspective_camera_process.h"
#include "vpgl_load_proj_camera_process.h"
#include "vpgl_load_rational_camera_nitf_process.h"
#include "vpgl_load_rational_camera_process.h"
#include "vpgl_nitf_camera_coverage_process.h"
#include "vpgl_save_perspective_camera_process.h"
#include "vpgl_save_rational_camera_process.h"
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
  REG_PROCESS(vpgl_load_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_proj_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_nitf_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_nitf_camera_coverage_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_rational_camera_process, bprb_batch_process_manager);
}

