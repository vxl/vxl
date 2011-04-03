#include "bvxm_grid_register.h"
//:
// \file
#include "bvxm_grid_processes.h"
#include <bvxm/grid/bvxm_voxel_grid_base_sptr.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

void bvxm_grid_register::register_datatype()
{
  REGISTER_DATATYPE( bvxm_voxel_grid_base_sptr );
}

void bvxm_grid_register::register_process()
{
  //: Grid Processes
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_load_grid_process,"bvxmLoadGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_create_grid_process, "bvxmCreateGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_save_grid_raw_process,"bvxmSaveGridRawProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_crop_grid_process, "bvxmCropGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_multiply_grids_process, "bvxmMutliplyGridsProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_threshold_grid_process, "bvxmThresholdGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_grid_to_image_stack_process, "bvxmGridToImageStackProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_combine_grids_process, "bvxmCombineGridsProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_expectation_opinion_grid_process, "bvxmExpectationOpinionGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_fill_mesh_grid_process, "bvxmFillMeshGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_fill_mesh_normals_grid_process, "bvxmFillMeshNormalsGridProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_grid_distance_transform_process, "bvxmGridDistanceTransformProcess");
  REG_PROCESS_FUNC_CONS( bprb_func_process, bprb_batch_process_manager, bvxm_save_rgba_grid_vrml_process, "bvxmSaveRGBAGridVrmlProcess");
}
