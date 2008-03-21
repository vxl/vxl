# brl/bpro/core/vil_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )


SET(vil_pro_sources
    vil_io_image_view_base.h        vil_io_image_view_base.cxx
    vil_load_image_view_process.h   vil_load_image_view_process.cxx
    vil_save_image_view_process.h   vil_save_image_view_process.cxx
    vil_map_image_process.h         vil_map_image_process.cxx
    vil_math_functors.h
    vil_binary_image_op_process.h   vil_binary_image_op_process.cxx 

)

AUX_SOURCE_DIRECTORY(Templates vil_pro_sources)

ADD_LIBRARY(vil_pro ${vil_pro_sources})
TARGET_LINK_LIBRARIES(vil_pro bprb vul vsl vil vil_io)



