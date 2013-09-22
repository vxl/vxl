#include "bwm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bwm_processes.h"

void bwm_register::register_datatype()
{}

void bwm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_create_corr_file_process, "bwmCreateCorrFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_create_satellite_resources_process, "bwmCreateSatResourcesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_save_satellite_resources_process, "bwmSaveSatResourcesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_load_satellite_resources_process, "bwmLoadSatResourcesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_add_satellite_resources_process, "bwmAddSatelliteResourcesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_query_satellite_resources_process, "bwmQuerySatelliteResourcesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_correct_rational_cameras_ransac_process, "bwmCorrectRationalCamerasRANSACProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_pick_nadir_resource_process, "bwmPickNadirResProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_map_osm_process, "bwmMapOSMProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_map_segments_process, "bwmMapSegmentsProcess");
}
