#ifndef vil_processes_h_
#define vil_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute and const functions
DECLARE_FUNC_CONS(vil_load_image_view_process);
DECLARE_FUNC_CONS(vil_save_image_view_process);
DECLARE_FUNC_CONS(vil_map_image_process);
DECLARE_FUNC_CONS(vil_gaussian_process);
DECLARE_FUNC_CONS(vil_gradient_process);
DECLARE_FUNC_CONS(vil_binary_image_op_process);
DECLARE_FUNC_CONS(vil_rgbi_to_grey_process);
DECLARE_FUNC_CONS(vil_convert_to_n_planes_process);
DECLARE_FUNC_CONS(vil_combine_grey_images_process);
DECLARE_FUNC_CONS(vil_load_from_ascii_process);
DECLARE_FUNC_CONS(vil_stretch_image_process);
DECLARE_FUNC_CONS(vil_image_pair_process);
DECLARE_FUNC_CONS(vil_load_image_view_binary_process);
DECLARE_FUNC_CONS(vil_save_image_view_binary_process);
DECLARE_FUNC_CONS(vil_map_image_binary_process);
DECLARE_FUNC_CONS(vil_crop_image_process);


#endif
