include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTHREEP_INCLUDE_DIR}/rply )
set(boxm2_cpp_sources

   )
aux_source_directory(Templates boxm2_cpp_sources)
add_library(boxm2_cpp  ${boxm2_cpp_sources})

target_link_libraries(boxm2_cpp boxm2 boxm2_basic boxm2_io boct brdb vbl_io expatpp vpgl vil_io vil vgl vgl_xio vbl vsl vcl rply)

# cpp process APIs.
add_subdirectory(pro)

# cpp process APIs.
add_subdirectory(algo)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
