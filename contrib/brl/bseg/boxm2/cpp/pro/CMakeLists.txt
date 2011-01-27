INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_cpp_pro_sources
    boxm2_cpp_render_process.h				boxm2_cpp_render_process.cxx
    boxm2_cpp_update_process.h				boxm2_cpp_update_process.cxx
    boxm2_cpp_refine_process.h				boxm2_cpp_refine_process.cxx
    boxm2_cpp_change_detection_process.h    boxm2_cpp_change_detection_process.cxx
    boxm2_cpp_change_detection_uncertainity_process.h boxm2_cpp_change_detection_uncertainity_process.cxx
   )
ADD_LIBRARY(boxm2_cpp_pro  ${boxm2_cpp_pro_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp_pro boxm2_cpp_algo brdb vbl_io expatpp vpgl vil vgl vgl_xio vbl vsl vcl)
