# brl/bpro/core/brip_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories(${GEL_INCLUDE_DIR})

set(brip_pro_sources
    brip_processes.h
    brip_register.h             brip_register.cxx
)

aux_source_directory(Templates brip_pro_sources)
aux_source_directory(processes brip_pro_sources)

vxl_add_library(LIBRARY_NAME brip_pro LIBRARY_SOURCES ${brip_pro_sources})
target_link_libraries(brip_pro bprb brdb brip vil_pro ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vsl bil_algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
