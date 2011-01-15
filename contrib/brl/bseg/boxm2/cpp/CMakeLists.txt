INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_cpp_sources
    boxm2_cpp_processor.h         boxm2_cpp_processor.cxx
    boxm2_cpp_process_base.h
    boxm2_cpp_render_process.h    boxm2_cpp_render_process.cxx
    boxm2_cpp_update_process.h    boxm2_cpp_update_process.cxx
    boxm2_cpp_refine_process.h    boxm2_cpp_refine_process.cxx
    boxm2_cpp_change_detection_process.h    boxm2_cpp_change_detection_process.cxx
    boxm2_cast_ray_function.h
    boxm2_render_exp_image_functor.h
    boxm2_update_image_functor.h
    boxm2_refine_block_function.h
    boxm2_mog3_grey_processor.h boxm2_mog3_grey_processor.cxx
    boxm2_data_serial_iterator.h
   )
AUX_SOURCE_DIRECTORY(Templates boxm2_cpp_sources)
ADD_LIBRARY(boxm2_cpp  ${boxm2_cpp_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp boxm2 boxm2_basic boxm2_io bocl boct brdb vbl_io expatpp vpgl vil vgl vgl_xio vbl vsl vcl)
