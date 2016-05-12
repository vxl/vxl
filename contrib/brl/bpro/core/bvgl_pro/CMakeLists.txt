# brl/bpro/bvgl_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR} )
if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    include_directories(${EXPAT_INCLUDE_DIRS} )
    include_directories(${EXPATPP_INCLUDE_DIRS} )
  endif()
endif()

set(bvgl_pro_sources
    bvgl_processes.h
    bvgl_register.h                    bvgl_register.cxx
)

aux_source_directory(processes bvgl_pro_sources)

vxl_add_library(LIBRARY_NAME bvgl_pro LIBRARY_SOURCES ${bvgl_pro_sources})

target_link_libraries(bvgl_pro bprb vil_pro bvgl bvgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
