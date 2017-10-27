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
    bbas_1d_array_byte.h        bbas_1d_array_byte.cxx
   )

aux_source_directory(Templates bbas_pro_sources)
aux_source_directory(processes bbas_pro_sources)

vxl_add_library(LIBRARY_NAME bbas_pro LIBRARY_SOURCES ${bbas_pro_sources})
target_link_libraries(bbas_pro bprb brdb brad brad_io bsta_io bsta ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl imesh depth_map)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
