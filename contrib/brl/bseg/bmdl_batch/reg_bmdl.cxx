#include "batch_bmdl.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
// processes
#include <vil_pro/vil_load_image_view_process.h>
#include <vil_pro/vil_save_image_view_process.h>
#include <vil_pro/vil_rgbi_to_grey_process.h>
#include <vpgl_pro/vpgl_load_proj_camera_process.h>
#include <vpgl_pro/vpgl_load_perspective_camera_process.h>
#include <vpgl_pro/vpgl_save_perspective_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_process.h>
#include <vpgl_pro/vpgl_save_rational_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_nitf_process.h>

// datatypes
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <bvxm/bvxm_voxel_world.h>
#include <vpgl/vpgl_camera.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(vil_load_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_rgbi_to_grey_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_nitf_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_proj_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_perspective_camera_process, bprb_batch_process_manager);

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;
  REGISTER_DATATYPE( vpgl_camera_double_sptr );

  Py_INCREF(Py_None);
  return Py_None;
}
