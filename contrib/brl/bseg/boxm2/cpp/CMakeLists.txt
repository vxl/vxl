INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_cpp_sources

   )
AUX_SOURCE_DIRECTORY(Templates boxm2_cpp_sources)
ADD_LIBRARY(boxm2_cpp  ${boxm2_cpp_sources})

TARGET_LINK_LIBRARIES(boxm2_cpp boxm2 boxm2_basic boxm2_io boct brdb vbl_io expatpp vpgl vil_io vil vgl vgl_xio vbl vsl vcl)

# cpp process APIs.
SUBDIRS(pro)

# cpp process APIs.
SUBDIRS(algo)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
