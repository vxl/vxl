#include "bbas_register.h"
#include "bbas_processes.h"


#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bsta/bsta_joint_histogram_3d.h>

void bbas_register::register_datatype()
{
  REGISTER_DATATYPE( bsta_joint_histogram_3d_base_sptr );
}

void bbas_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbas_atmospheric_corr_process,"bbasAtmosphericCorrProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbas_estimate_irradiance_process,"bbasEstimateIrradianceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbas_camera_angles_process,"bbasCameraAnglesProcess");
  REG_PROCESS_FUNC_CONS_FIN(bprb_func_process, bprb_batch_process_manager,bbas_string_array_process , "bbasStringArrayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bbas_merge_string_array_process , "bbasMergeStringArrayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bsta_load_joint_hist_3d_process , "bstaLoadJointHist3dProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bsta_save_joint_hist_3d_process , "bstaSaveJointHist3dProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bsta_joint_hist_3d_vrml_process , "bstaJointHist3dVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bsta_clone_joint_hist_3d_process , "bstaCloneJointHist3dProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbas_remove_from_db_process, "bbasRemoveFromDbProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsl_expected_image_process, "bslExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsl_fusion_process, "bslFusionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, imesh_ply_bbox_process, "imeshPlyBboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bpgl_generate_depth_maps_process, "bpglGenerateDepthMapsProcess");
}
