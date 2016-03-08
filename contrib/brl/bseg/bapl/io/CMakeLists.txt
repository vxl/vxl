#brl/bbas/bapl/io/CMakeLists.txt

set( bapl_io_sources
     bapl_io_dsift.h      bapl_io_dsift.cxx
   )
aux_source_directory( Templates bapl_io_sources )

include_directories( ${BRL_INCLUDE_DIR}/bseg)

vxl_add_library(LIBRARY_NAME bapl_io LIBRARY_SOURCES ${bapl_io_sources} )
target_link_libraries( bapl_io bapl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
