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

add_library(brec_pro ${brec_pro_sources})

target_link_libraries(brec_pro brec bvxm_grid_io bbgm_pro bprb brip brdb sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil_pro vil_algo vil vil_io vnl_io vnl vgl_algo vgl vbl_io vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
