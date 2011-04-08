# boxm opencl opengl display
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
SET(boxm2_view_pro_sources
    boxm2_view_register.h      boxm2_view_register.cxx
    boxm2_view_processes.h
   )
AUX_SOURCE_DIRECTORY(processes boxm2_view_pro_sources)
ADD_LIBRARY(boxm2_view_pro ${boxm2_view_pro_sources} )

TARGET_LINK_LIBRARIES(boxm2_view_pro boxm2_view vgui)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

