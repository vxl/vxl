# brl/bpro/core/icam_pro/CMakeLists.txt
# Brown vpgl processes

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set( icam_pro_sources
  icam_processes.h
  icam_register.h      icam_register.cxx
)
aux_source_directory(Templates icam_pro_sources)
aux_source_directory(processes icam_pro_sources)

vxl_add_library(LIBRARY_NAME icam_pro LIBRARY_SOURCES ${icam_pro_sources})
target_link_libraries(icam_pro bprb icam ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
