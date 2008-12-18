#include "batch_bvxm.h"
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
#include <bvxm/pro/bvxm_rpc_prob_registration_process.h>
#include <bvxm/pro/bvxm_rpc_registration_process.h>
#include <bvxm/pro/bvxm_generate_edge_map_process.h>
#include <bvxm/pro/bvxm_save_edges_raw_process.h>
#include <bvxm/pro/bvxm_save_occupancy_raw_process.h>
#include <bvxm/pro/bvxm_save_occupancy_vff_process.h>
#include <bvxm/pro/bvxm_change_detection_display_process.h>
#include <bvxm/pro/bvxm_gen_synthetic_world_process.h>
#include <bvxm/pro/bvxm_create_synth_lidar_data_process.h>
#include <bvxm/pro/bvxm_lidar_init_process.h>
#include <bvxm/pro/bvxm_update_lidar_process.h>
#include <bvxm/pro/bvxm_heightmap_process.h>
#include <bvxm/pro/bvxm_create_normalized_image_process.h>
#include <bvxm/pro/bvxm_compare_rpc_process.h>
#include <bvxm/pro/bvxm_ocp_compare_process.h>
#include <bvxm/pro/bvxm_ocp_hist_process.h>
#include <bvxm/pro/bvxm_pmap_hist_process.h>
#include <bvxm/pro/bvxm_pmap_ratio_process.h>
#include <bvxm/pro/bvxm_roc_process.h>
#include <bvxm/pro/bvxm_update_edges_lidar_process.h>
#include <bvxm/pro/bvxm_lidar_edge_detection_process.h>

// multiscale
#include <bvxm/pro/multiscale_pro/bvxm_detect_scale_process.h>
//#include <bvxm/pro/multiscale_pro/bvxm_create_multiscale_voxel_world_process.h>
//#include <bvxm/pro/multiscale_pro/bvxm_update_multiscale_process.h>
//#include <bvxm/pro/multiscale_pro/bvxm_detect_multiscale_process.h>
#include <bvxm/pro/multiscale_pro/bvxm_compare_3d_voxels_process.h>

//#include <bvxm/multiscale/bvxm_multi_scale_voxel_world.h>
//#include <bvxm/pro/multiscale_pro/bvxm_save_multiscale_occupancy_raw_process.h>

// recognition
#include <brec/pro/brec_update_changes_process.h>
#include <brec/pro/brec_density_to_prob_map_process.h>
#include <brec/pro/brec_prob_map_roc_compute_process.h>
#include <brec/pro/brec_prob_map_threshold_process.h>
#include <brec/pro/brec_recognize_structure_process.h>
#include <brec/pro/brec_prob_map_supress_process.h>
#include <brec/pro/brec_prob_map_area_process.h>
#include <brec/pro/brec_change_area_process.h>
#include <brec/pro/brec_bayesian_update_process.h>
#include <brec/pro/brec_glitch_process.h>
#include <brec/pro/brec_glitch_overlay_process.h>
#include <brec/pro/brec_create_mog_image_process.h>

#include <bvgl_pro/bvgl_generate_mask_process.h>
#include <vil_pro/vil_load_image_view_binary_process.h>
#include <vil_pro/vil_save_image_view_binary_process.h>
#include <vil_pro/vil_map_image_process.h>
#include <vil_pro/vil_map_image_binary_process.h>

// bbgm
#include <bbgm/pro/bbgm_load_image_of_process.h>
#include <bbgm/pro/bbgm_save_image_of_process.h>
#include <bbgm/pro/bbgm_update_dist_image_process.h>
#include <bbgm/pro/bbgm_update_dist_image_stream_process.h>
#include <bbgm/pro/bbgm_display_dist_image_process.h>
#include <bbgm/pro/bbgm_measure_process.h>
#include <bbgm/bbgm_image_sptr.h>

// vidl2
#include <core/vil_pro/vil_load_image_view_process.h>
#include <core/vil_pro/vil_save_image_view_process.h>
#include <core/vidl2_pro/vidl2_open_istream_process.h>
#include <core/vidl2_pro/vidl2_close_istream_process.h>
//#define HAS_DIRECTSHOW
#ifdef HAS_DIRECTSHOW
#include <core/vidl2_pro/vidl2_open_dshow_istream_process.h>
#endif
#include <core/vidl2_pro/vidl2_get_frame_istream_process.h>
#include <core/vidl2_pro/vidl2_open_ostream_process.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <vidl2/vidl2_ostream_sptr.h>

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
  REG_PROCESS(bvxm_create_voxel_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_detect_changes_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_virtual_view_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_expected_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_roi_init_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_normalize_image_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_illum_index_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_rpc_registration_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_rpc_prob_registration_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_generate_edge_map_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_change_detection_display_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_clean_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_edges_raw_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_raw_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_vff_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_local_rpc_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_gen_synthetic_world_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_synth_lidar_data_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_lidar_init_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_lidar_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_heightmap_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_normalized_image_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_compare_rpc_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_ocp_compare_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_ocp_hist_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_pmap_hist_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_pmap_ratio_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_roc_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_edges_lidar_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_lidar_edge_detection_process, bprb_batch_process_manager);

  // multiscale
  REG_PROCESS(bvxm_detect_scale_process, bprb_batch_process_manager);
  //REG_PROCESS(bvxm_create_multiscale_voxel_world_process, bprb_batch_process_manager);
  //REG_PROCESS(bvxm_update_multiscale_process, bprb_batch_process_manager);
  //REG_PROCESS(bvxm_detect_multiscale_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_compare_3d_voxels_process, bprb_batch_process_manager);

  // recognition
  REG_PROCESS(brec_update_changes_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_roc_compute_process, bprb_batch_process_manager);
  REG_PROCESS(brec_density_to_prob_map_process, bprb_batch_process_manager);
  REG_PROCESS(brec_recognize_structure_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_threshold_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_supress_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_area_process, bprb_batch_process_manager);
  //REG_PROCESS(bvxm_rec_update_changes_wrt_area_process, bprb_batch_process_manager);
  REG_PROCESS(brec_change_area_process, bprb_batch_process_manager);
  REG_PROCESS(brec_bayesian_update_process, bprb_batch_process_manager);
  REG_PROCESS(brec_glitch_process, bprb_batch_process_manager);
  REG_PROCESS(brec_glitch_overlay_process, bprb_batch_process_manager);
  REG_PROCESS(brec_create_mog_image_process, bprb_batch_process_manager);

  REG_PROCESS(vil_map_image_process, bprb_batch_process_manager);
  REG_PROCESS(vil_map_image_binary_process, bprb_batch_process_manager);
  REG_PROCESS(bvgl_generate_mask_process, bprb_batch_process_manager);
  REG_PROCESS(vil_load_image_view_binary_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_binary_process, bprb_batch_process_manager);

  // bbgm
  REG_PROCESS(bbgm_load_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_save_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_stream_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_display_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_measure_process, bprb_batch_process_manager);

  // vidl2
  REG_PROCESS(vidl2_open_istream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_close_istream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_open_ostream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_get_frame_istream_process, bprb_batch_process_manager);
#ifdef HAS_DIRECTSHOW
  REG_PROCESS(vidl2_open_dshow_istream_process, bprb_batch_process_manager);
#endif

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
  //REGISTER_DATATYPE( bvxm_multi_scale_voxel_world_sptr );
  REGISTER_DATATYPE( bbgm_image_sptr );
  REGISTER_DATATYPE(vidl2_istream_sptr);
  REGISTER_DATATYPE(vidl2_ostream_sptr);

  Py_INCREF(Py_None);
  return Py_None;
}
