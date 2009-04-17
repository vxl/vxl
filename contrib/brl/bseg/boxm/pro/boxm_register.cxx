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
  const bool null_func = 0;
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_load_scene_process, "boxmLoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_process, "boxmRenderExpectedProcess");  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_save_occupancy_raw_process, "boxmSaveOccupancyRawProcess"); 
  //REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, psm_save_alpha_raw_process, "psmSaveAlphaRawProcess");  
  
}
