include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTHREEP_INCLUDE_DIR} )
include_directories( ${MUL_INCLUDE_DIR} )

set(boxm2_cpp_pro_sources
    boxm2_cpp_register.h                              boxm2_cpp_register.cxx
    boxm2_cpp_processes.h
   )
aux_source_directory(processes boxm2_cpp_pro_sources)
add_library(boxm2_cpp_pro  ${boxm2_cpp_pro_sources})

target_link_libraries(boxm2_cpp_pro boxm2_cpp_algo brad bbas_pro bbgm_pro bprb brdb vbl_io expatpp vpgl vil vgl_xio vgl vul vbl vsl vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
