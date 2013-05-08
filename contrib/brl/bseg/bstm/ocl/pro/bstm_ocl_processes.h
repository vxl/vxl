#ifndef bstm_ocl_processes_h_
#define bstm_ocl_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

DECLARE_FUNC_CONS(bstm_ocl_render_expected_image_process);
DECLARE_FUNC_CONS(bstm_ocl_render_gl_expected_image_process);
DECLARE_FUNC_CONS(bstm_ocl_render_gl_expected_color_process);

DECLARE_FUNC_CONS(bstm_ocl_get_surface_pt_process);
DECLARE_FUNC_CONS(bstm_ocl_minfo_process);
DECLARE_FUNC_CONS(bstm_ocl_track_process);


DECLARE_FUNC_CONS(bstm_create_opencl_cache_process);
DECLARE_FUNC_CONS(bstm_clear_opencl_cache_process);
#endif
