#ifndef ihog_processes_h_
#define ihog_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(ihog_register_translational_process);
DECLARE_FUNC_CONS(ihog_compute_mi_cost_surface_process);
DECLARE_FUNC_CONS(ihog_mutual_information_process);

#endif
