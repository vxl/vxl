# This is contrib/brl/bbas/bxml/bsvg/pro/CMakeLists.txt
# Brown bsvg processes

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bsvg_pro_sources
    bsvg_register.h      bsvg_register.cxx
    bsvg_processes.h
   )

aux_source_directory(Templates bsvg_pro_sources)
aux_source_directory(processes bsvg_pro_sources)

vxl_add_library(LIBRARY_NAME bsvg_pro LIBRARY_SOURCES ${bsvg_pro_sources})
target_link_libraries(bsvg_pro bbas_pro bsvg bprb brdb ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
