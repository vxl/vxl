include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTHREEP_INCLUDE_DIR}/rply )
set(boxm2_cpp_sources

   )
aux_source_directory(Templates boxm2_cpp_sources)
vxl_add_library(LIBRARY_NAME boxm2_cpp LIBRARY_SOURCES  ${boxm2_cpp_sources})

target_link_libraries(boxm2_cpp boxm2 boxm2_basic boxm2_io boct brdb ${VXL_LIB_PREFIX}vbl_io expatpp ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl rply)

# cpp process APIs.
add_subdirectory(pro)

# cpp process APIs.
add_subdirectory(algo)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
