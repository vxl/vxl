#include "bstm_multi_cpp_register.h"

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

#include "bstm_multi_cpp_processes.h"

void bstm_multi_cpp_register::register_datatype() {}

void bstm_multi_cpp_register::register_process() {
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_ingest_boxm2_scene_process,
                        "bstmMultiCppIngestBoxm2SceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_bstm_to_bstm_multi_scene_process,
                        "bstmMultiCppBstmToBstmMultiSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_extract_point_cloud_process,
                        "bstmMultiCppExtractPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_export_point_cloud_process,
                        "bstmMultiCppExportPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_copy_data_to_future_process,
                        "bstmMultiCppCopyDataToFutureProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_analyze_coherency_process,
                        "bstmMultiCppAnalyzeCoherencyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_refine_spacetime_process,
                        "bstmMultiCppRefineSpacetimeProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_change_btw_frames_process,
                        "bstmMultiCppChangeBtwFramesProcess");
}
