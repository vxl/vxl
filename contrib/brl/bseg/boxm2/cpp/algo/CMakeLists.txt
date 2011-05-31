INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_cpp_algo_sources
    boxm2_cast_ray_function.h
    boxm2_cast_cone_ray_function.h
    boxm2_render_functions.h          boxm2_render_functions.cxx
    boxm2_render_exp_image_functor.h 
    boxm2_render_exp_depth_functor.h 
    boxm2_render_cone_functor.h
    boxm2_update_image_functor.h
    boxm2_update_functions.h		  boxm2_update_functions.cxx
    boxm2_cone_update_image_functor.h
    boxm2_refine_block_function.h     boxm2_refine_block_function.cxx
    boxm2_change_detection_functor.h
    boxm2_mog3_grey_processor.h       boxm2_mog3_grey_processor.cxx
    boxm2_data_serial_iterator.h
    boxm2_cast_intensities_functor.h
    boxm2_mean_intensities_batch_functor.h
    boxm2_filter_block_function.h     boxm2_filter_block_function.cxx
    boxm2_batch_functors.h
   )

ADD_LIBRARY(boxm2_cpp_algo  ${boxm2_cpp_algo_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp_algo boxm2_cpp boct brdb vbl_io expatpp vpgl vil_algo vil bvgl vgl vgl_xio vnl_algo vbl vsl vcl)

SUBDIRS(tests)

