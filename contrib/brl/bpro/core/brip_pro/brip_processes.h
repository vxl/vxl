#ifndef brip_processes_h_
#define brip_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(brip_extrema_process);
DECLARE_FUNC_CONS(brip_blobwise_mutual_info_process);
DECLARE_FUNC_CONS(brip_blobwise_kl_div_process);
DECLARE_FUNC_CONS(brip_blob_intersection_process);
DECLARE_FUNC_CONS(brip_solve_gain_offset_process);
DECLARE_FUNC_CONS(brip_truncate_nitf_bit_process);
DECLARE_FUNC_CONS(brip_solve_gain_offset_constraints_process);
DECLARE_FUNC_CONS(brip_image_mutual_info_process);
DECLARE_FUNC_CONS(brip_phase_correlation_process);

#endif
