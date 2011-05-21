#include "bapl_register.h"
#include "bapl_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <bapl/bapl_keypoint_set_sptr.h>
#include <bapl/bapl_connectivity_sptr.h>
#include <bapl/bapl_dsift_sptr.h>

void bapl_register::register_datatype()
{
  REGISTER_DATATYPE( bapl_keypoint_set_sptr );
  REGISTER_DATATYPE( bapl_keypoint_match_set_sptr );
  REGISTER_DATATYPE( bapl_conn_table_sptr );
#if 0 // not needed -- see below
  REGISTER_DATATYPE( bapl_dsift_sptr );
#endif
}

void bapl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_extract_keypoints_process, "baplExtractKeypointsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_load_keypoints_process, "baplLoadKeypointsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_draw_keypoints_process, "baplDrawKeypointsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_match_keypoints_process, "baplMatchKeypointsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_match_display_process, "baplMatchDisplayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_refine_match_process, "baplRefineMatchProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_create_conn_table_process, "baplCreateConnTableProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_add_match_set_process, "baplAddMatchSetProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_add_image_keys_process, "baplAddImageKeysProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_compute_tracks_process, "baplComputeTracksProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_load_match_process, "baplLoadMatchSetProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_write_match_process, "baplWriteMatchSetProcess");
#if 0 // process seems to have disappeared from the processes subdirectory???
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bapl_dsift_sptr_process, "baplDsiftSptrProcess");
#endif
}
