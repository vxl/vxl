INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_cpp_algo_sources
    boxm2_cast_ray_function.h
    boxm2_render_exp_image_functor.h
    boxm2_update_image_functor.h
    boxm2_refine_block_function.h
    boxm2_change_detection_functor.h
    boxm2_mog3_grey_processor.h boxm2_mog3_grey_processor.cxx
    boxm2_data_serial_iterator.h
   )

ADD_LIBRARY(boxm2_cpp_algo  ${boxm2_cpp_algo_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp_algo boxm2_cpp boct brdb vbl_io expatpp vpgl vil_algo vil vgl vgl_xio vbl vsl vcl)

SUBDIRS(tests)

