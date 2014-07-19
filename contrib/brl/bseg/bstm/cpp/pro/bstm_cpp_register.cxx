#include "bstm_cpp_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bstm_cpp_processes.h"

#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>

void bstm_cpp_register::register_datatype()
{
}

void bstm_cpp_register::register_process()
{
  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_ingest_boxm2_scene_process, "bstmCppIngestBoxm2SceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_label_bb_process, "bstmCppLabelBBProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_extract_point_cloud_process, "bstmCppExtractPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_export_point_cloud_process, "bstmCppExportPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_majority_filter_process, "bstmCppMajorityFilterProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_copy_data_to_future_process, "bstmCppCopyDataToFutureProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_label_tt_depth_process, "bstmCppLabelTTDepthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_analyze_coherency_process, "bstmCppAnalyzeCoherencyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_refine_spacetime_process, "bstmCppRefineSpacetimeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_refine_space_process, "bstmCppRefineSpaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_refine_tt_process, "bstmCppRefineTTProcess");


  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_merge_tt_process, "bstmCppMergeTTProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_change_btw_frames_process, "bstmCppChangeBtwFramesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_box_roc_process, "bstmCppBoxROCProcess");


}
