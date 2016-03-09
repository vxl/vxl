# brl/bpro/core/ihog_pro/CMakeLists.txt
# Brown ihog processes

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )

set( ihog_pro_sources
  ihog_processes.h
  ihog_register.h                          ihog_register.cxx
)
aux_source_directory(Templates ihog_pro_sources)
aux_source_directory(processes ihog_pro_sources)

vxl_add_library(LIBRARY_NAME ihog_pro LIBRARY_SOURCES ${ihog_pro_sources})
target_link_libraries(ihog_pro bprb ihog ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil vil_pro ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
