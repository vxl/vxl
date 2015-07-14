#include "boxm2_volm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/volm/boxm2_volm_locations_sptr.h>
#include "boxm2_volm_processes.h"

void boxm2_volm_register::register_datatype()
{
}

void boxm2_volm_register::register_process()
{
#ifdef HAVE_OPENCL
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_index_process, "boxm2IndexHypothesesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_visualize_index_process, "boxm2VisualizeIndicesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_index_process2, "boxm2IndexHypothesesProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_label_index_process, "boxm2IndexLabelDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_visualize_index_process2, "boxm2VisualizeHypIndexProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_all_index_process, "boxm2IndexLabelCombinedDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_geo_cover_with_osm_to_xyz_process, "boxm2OSMToXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_change_scene_res_by_geo_cover_process, "boxm2ChangeSceneResByGeoCover");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_load_score_binary_process, "boxm2LoadScoreBinary");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_location_pin_point_ranking_process, "boxm2LocationPinPointRanking");
#endif
}
