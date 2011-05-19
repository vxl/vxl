#include "boxm2_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>

#include <imesh/imesh_mesh.h>

#include "boxm2_processes.h"

void boxm2_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_scene_sptr );
  REGISTER_DATATYPE( boxm2_cache_sptr );
  REGISTER_DATATYPE( imesh_mesh_sptr );
  REGISTER_DATATYPE( boxm2_stream_cache_sptr );
}

void boxm2_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_load_scene_process,      "boxm2LoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_cache_process,    "boxm2CreateCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_scene_process,    "boxm2CreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_scene_xml_process, "boxm2WriteSceneXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_add_block_process,       "boxm2AddBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_save_data_process,       "boxm2SaveDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_textured_mesh_process,"boxm2ExportTexturedMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_mesh_process,"boxm2ExportMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_texture_mesh_process,"boxm2TextureMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_describe_scene_process,"boxm2DescribeSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_rw_cache_process, "boxm2CreateRWCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_stream_cache_process, "boxm2CreateStreamCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_illumination_info_process, "boxm2SceneIlluminationInfoProcess");
}
