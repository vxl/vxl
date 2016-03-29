# bsta's Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bsta_pro_sources
    bsta_processes.h
    bsta_register.h   bsta_register.cxx
)

aux_source_directory(Templates bsta_pro_sources)
aux_source_directory(processes bsta_pro_sources)

vxl_add_library(LIBRARY_NAME bsta_pro LIBRARY_SOURCES ${bsta_pro_sources} )

target_link_libraries(bsta_pro bsta_algo bsta_io bsta bprb brdb brip ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()