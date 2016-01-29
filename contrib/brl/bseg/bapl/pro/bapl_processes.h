#ifndef bapl_processes_h_
#define bapl_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(bapl_extract_keypoints_process);
DECLARE_FUNC_CONS(bapl_load_keypoints_process);
DECLARE_FUNC_CONS(bapl_draw_keypoints_process);
DECLARE_FUNC_CONS(bapl_match_keypoints_process);
DECLARE_FUNC_CONS(bapl_match_display_process);
DECLARE_FUNC_CONS(bapl_refine_match_process);
DECLARE_FUNC_CONS(bapl_create_conn_table_process);
DECLARE_FUNC_CONS(bapl_add_match_set_process);
DECLARE_FUNC_CONS(bapl_add_image_keys_process);
DECLARE_FUNC_CONS(bapl_compute_tracks_process);
DECLARE_FUNC_CONS(bapl_load_match_process);
DECLARE_FUNC_CONS(bapl_write_match_process);
DECLARE_FUNC_CONS(bapl_dsift_sptr_process);

#endif  // bapl_processes_h_
