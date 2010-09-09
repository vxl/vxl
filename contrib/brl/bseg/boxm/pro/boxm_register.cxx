#include "boxm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm/boxm_scene_base.h>

#include "boxm_processes.h"

void boxm_register::register_datatype()
{
  REGISTER_DATATYPE( boxm_scene_base_sptr );
}

void boxm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_load_scene_process, "boxmLoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_create_scene_process, "boxmCreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_proj_local_cam_process,"boxmProjLocalCamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_camera_viewing_scene_process,"boxmCameraViewingSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_construct_scene_from_image_process, "boxmConstructSceneFromImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_upload_mesh_process,"boxmUploadMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_clean_scene_process,"boxmCleanSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_force_write_blocks_process,"boxmForceWriteBlocksProcess");

}
