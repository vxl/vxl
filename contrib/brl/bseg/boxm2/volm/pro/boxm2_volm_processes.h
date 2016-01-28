#ifndef boxm2_volm_processes_h_
#define boxm2_volm_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

// the init functions
DECLARE_FUNC_CONS(boxm2_create_index_process);
DECLARE_FUNC_CONS(boxm2_visualize_index_process);
DECLARE_FUNC_CONS(boxm2_create_index_process2);
DECLARE_FUNC_CONS(boxm2_create_label_index_process);
DECLARE_FUNC_CONS(boxm2_visualize_index_process2);
DECLARE_FUNC_CONS(boxm2_create_all_index_process);
DECLARE_FUNC_CONS(boxm2_geo_cover_with_osm_to_xyz_process);
DECLARE_FUNC_CONS(boxm2_change_scene_res_by_geo_cover_process);
DECLARE_FUNC_CONS(boxm2_load_score_binary_process);
DECLARE_FUNC_CONS(boxm2_location_pin_point_ranking_process);
#endif
