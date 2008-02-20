# brl/bpro/core/vil_pro/CMakeLists.txt

IF(BUILD_EXPAT)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

SET(vil_bpro_sources
    vil_io_image_view_base.h vil_io_image_view_base.cxx
    vil_load_image_view_process.h vil_load_image_view_process.cxx
    vil_save_image_view_process.h vil_save_image_view_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates vil_bpro_sources)

ADD_LIBRARY(vil_bpro ${vil_bpro_sources})
TARGET_LINK_LIBRARIES(vil_bpro bprb vsl vil vil_io)
ENDIF(BUILD_EXPAT)

