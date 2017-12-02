include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTHREEP_INCLUDE_DIR} )
include_directories( ${MUL_INCLUDE_DIR} )

set(boxm2_cpp_pro_sources
    boxm2_cpp_register.h                              boxm2_cpp_register.cxx
    boxm2_cpp_processes.h
   )
aux_source_directory(processes boxm2_cpp_pro_sources)
vxl_add_library(LIBRARY_NAME boxm2_cpp_pro LIBRARY_SOURCES  ${boxm2_cpp_pro_sources})

target_link_libraries(boxm2_cpp_pro boxm2_cpp_algo brad bbas_pro bbgm_pro bprb brdb ${VXL_LIB_PREFIX}vbl_io expatpp ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
