# This is contrib/brl/bseg/bapl/pro/CMakeLists.txt
# Brown Affine Patch Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )

set(bapl_pro_sources
    bapl_register.h      bapl_register.cxx
    bapl_processes.h
   )

aux_source_directory(Templates bapl_pro_sources)
aux_source_directory(processes bapl_pro_sources)

add_library(bapl_pro ${bapl_pro_sources})
target_link_libraries(bapl_pro bapl bapl_io bprb brdb vil_pro vil_io vil vnl vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
