#ifndef boxm2_volm_processes_h_
#define boxm2_volm_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm2_load_hypotheses_process);
DECLARE_FUNC_CONS(boxm2_create_index_process);
DECLARE_FUNC_CONS(boxm2_get_hypothesis_process);
DECLARE_FUNC_CONS(boxm2_create_hypotheses_process);
DECLARE_FUNC_CONS(boxm2_add_hypothesis_process);
DECLARE_FUNC_CONS(boxm2_save_hypotheses_process);
DECLARE_FUNC_CONS(boxm2_visualize_index_process);
DECLARE_FUNC_CONS(boxm2_partition_hypotheses_process);
DECLARE_FUNC_CONS(boxm2_hypotheses_kml_process);

#endif
