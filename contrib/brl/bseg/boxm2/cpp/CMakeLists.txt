INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )
SET(boxm2_cpp_sources

   )
AUX_SOURCE_DIRECTORY(Templates boxm2_cpp_sources)
ADD_LIBRARY(boxm2_cpp  ${boxm2_cpp_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp boxm2 boxm2_basic boxm2_io boct brdb vbl_io expatpp vpgl vil_io vil vgl vgl_xio vbl vsl vcl rply)

# cpp process APIs.
add_subdirectory(pro)

# cpp process APIs.
add_subdirectory(algo)

#tests
IF( BUILD_TESTING )
  add_subdirectory(tests)
ENDIF( BUILD_TESTING )
