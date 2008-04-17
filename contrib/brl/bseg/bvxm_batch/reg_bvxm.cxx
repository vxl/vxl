#include "batch_bvxm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
// processes
#include <vil_pro/vil_load_image_view_process.h>
#include <vil_pro/vil_save_image_view_process.h>
#include <vpgl_pro/vpgl_load_proj_camera_process.h>
#include <vpgl_pro/vpgl_load_perspective_camera_process.h>
#include <vpgl_pro/vpgl_save_perspective_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_process.h>
#include <vpgl_pro/vpgl_load_rational_camera_nitf_process.h>
#include <bvxm/pro/bvxm_create_voxel_world_process.h>
#include <bvxm/pro/bvxm_update_process.h>
#include <bvxm/pro/bvxm_detect_changes_process.h>
#include <bvxm/pro/bvxm_render_virtual_view_process.h>
#include <bvxm/pro/bvxm_render_expected_image_process.h>
#include <bvxm/pro/bvxm_roi_init_process.h>
#include <bvxm/pro/bvxm_normalize_image_process.h>
#include <bvxm/pro/bvxm_illum_index_process.h>
#include <bvxm/pro/bvxm_create_local_rpc_process.h>
#include <bvxm/pro/bvxm_clean_world_process.h>
#include <bvxm/pro/bvxm_rpc_registration_process.h>
#include <bvxm/pro/bvxm_generate_edge_map_process.h>
#include <bvxm/pro/bvxm_save_edges_raw_process.h>
#include <bvxm/pro/bvxm_save_occupancy_raw_process.h>
#include <bvxm/pro/bvxm_change_detection_display_process.h>
#include <bvxm/pro/bvxm_gen_synthetic_world_process.h>
#include <bvxm/pro/bvxm_create_synth_lidar_data_process.h>
#include <bvxm/pro/bvxm_lidar_init_process.h>
#include <bvxm/pro/bvxm_update_lidar_process.h>
#include <bvxm/pro/bvxm_heightmap_process.h>

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
  REG_PROCESS(vpgl_load_rational_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_rational_camera_nitf_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_proj_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_load_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(vpgl_save_perspective_camera_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_voxel_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_detect_changes_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_virtual_view_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_expected_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_roi_init_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_normalize_image_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_illum_index_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_rpc_registration_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_generate_edge_map_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_change_detection_display_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_clean_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_edges_raw_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_raw_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_local_rpc_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_gen_synthetic_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_synth_lidar_data_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_lidar_init_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_lidar_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_heightmap_process, bprb_batch_process_manager);
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
  REGISTER_DATATYPE( bvxm_voxel_world_sptr );
  Py_INCREF(Py_None);
  return Py_None;
}
