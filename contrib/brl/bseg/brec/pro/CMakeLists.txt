# This is contrib/brl/bseg/brec/pro/CMakeLists.txt
# Brown Recognition Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )

set(brec_pro_sources
    brec_register.h      brec_register.cxx
    brec_processes.h
   )

aux_source_directory(Templates brec_pro_sources)

aux_source_directory(processes brec_pro_sources)

vxl_add_library(LIBRARY_NAME brec_pro LIBRARY_SOURCES ${brec_pro_sources})

target_link_libraries(brec_pro brec bvxm_grid_io bbgm_pro bprb brip brdb sdet ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_file_formats vpgl_pro vsol vil_pro ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
