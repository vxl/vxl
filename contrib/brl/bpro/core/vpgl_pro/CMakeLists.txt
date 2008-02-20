# contrib/brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

IF(BUILD_EXPAT)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )


SET( vpgl_bpro_sources
 vpgl_load_proj_camera_process.h vpgl_load_proj_camera_process.cxx
 vpgl_load_rational_camera_process.h  vpgl_load_rational_camera_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_bpro_sources)

ADD_LIBRARY(vpgl_bpro ${vpgl_bpro_sources})
TARGET_LINK_LIBRARIES( vpgl_bpro bprb vpgl vpgl_io vnl)
ENDIF(BUILD_EXPAT)
