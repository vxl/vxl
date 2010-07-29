#ifndef sdet_processes_h_
#define sdet_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(sdet_detect_edges_process);
DECLARE_FUNC_CONS(sdet_detect_edge_tangent_process);
DECLARE_FUNC_CONS(sdet_detect_edge_tangent_interp_process);
DECLARE_FUNC_CONS(sdet_write_edge_file_process);
DECLARE_FUNC_CONS(sdet_detect_edge_line_fitted_process);
DECLARE_FUNC_CONS(sdet_detect_third_order_edges_process);

#endif
