# brl/bpro/core/vil_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

SET(vil_pro_sources
    vil_io_image_view_base.h            vil_io_image_view_base.cxx
    vil_math_functors.h
    vil_register.h                       vil_register.cxx
    vil_processes.h
)

AUX_SOURCE_DIRECTORY(Templates vil_pro_sources)

AUX_SOURCE_DIRECTORY(processes vil_pro_sources)

ADD_LIBRARY(vil_pro ${vil_pro_sources})

TARGET_LINK_LIBRARIES(vil_pro bprb vul vsl vil_algo vil vil_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
