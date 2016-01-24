# brl/bpro/core/bbas_pro/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${GEL_INCLUDE_DIR}/mrc)

set(bbas_pro_sources
    bbas_processes.h
    bbas_register.h             bbas_register.cxx
    bbas_1d_array_float.h       bbas_1d_array_float.cxx
    bbas_1d_array_double.h      bbas_1d_array_double.cxx
    bbas_1d_array_int.h         bbas_1d_array_int.cxx
    bbas_1d_array_unsigned.h    bbas_1d_array_unsigned.cxx
    bbas_1d_array_string.h      bbas_1d_array_string.cxx
   )

aux_source_directory(Templates bbas_pro_sources)
aux_source_directory(processes bbas_pro_sources)

add_library(bbas_pro ${bbas_pro_sources})
target_link_libraries(bbas_pro bprb brdb brad brad_io bsta_io bsta vpgl vpgl_algo vil_io vil vnl_io vnl_algo vnl vgl vbl_io vbl vsl imesh depth_map)

install_targets(/lib bbas_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/bbas_pro ${bbas_pro_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
