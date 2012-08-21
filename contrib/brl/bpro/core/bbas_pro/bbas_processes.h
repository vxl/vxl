#ifndef bbas_processes_h_
#define bbas_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>


DECLARE_FUNC_CONS(bbas_atmospheric_corr_process);
DECLARE_FUNC_CONS(bbas_estimate_irradiance_process);
DECLARE_FUNC_CONS(bbas_camera_angles_process);
DECLARE_FUNC_CONS_FIN(bbas_string_array_process);
DECLARE_FUNC_CONS(bbas_merge_string_array_process);

DECLARE_FUNC_CONS(bsta_load_joint_hist_3d_process);
DECLARE_FUNC_CONS(bsta_save_joint_hist_3d_process);
DECLARE_FUNC_CONS(bsta_joint_hist_3d_vrml_process);
DECLARE_FUNC_CONS(bsta_clone_joint_hist_3d_process);
DECLARE_FUNC_CONS(bbas_remove_from_db_process);
DECLARE_FUNC_CONS(bsl_expected_image_process);
DECLARE_FUNC_CONS(bsl_fusion_process);

DECLARE_FUNC_CONS(imesh_ply_bbox_process);
DECLARE_FUNC_CONS(bpgl_generate_depth_maps_process);

#endif // bbas_processes_h_
