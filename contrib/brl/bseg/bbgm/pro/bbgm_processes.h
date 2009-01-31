#ifndef bbgm_processes_h_
#define bbgm_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

DECLARE_FUNC_CONS(bbgm_load_image_of_process);
DECLARE_FUNC_CONS(bbgm_save_image_of_process);
DECLARE_FUNC_CONS_INIT(bbgm_update_dist_image_process);
DECLARE_FUNC_CONS_INIT(bbgm_update_dist_image_stream_process);
DECLARE_FUNC_CONS(bbgm_display_dist_image_process);
DECLARE_FUNC_CONS(bbgm_measure_process);
DECLARE_FUNC_CONS(bbgm_local_frame_trans_process);
DECLARE_FUNC_CONS_INIT(bbgm_update_parzen_dist_image_process);

#endif
