# basic/brad/io/CMakeLists.txt
# Brown radiometry io Library.

set(brad_io_sources
  brad_io_image_metadata.h  brad_io_image_metadata.cxx
  brad_io_atmospheric_parameters.h brad_io_atmospheric_parameters.cxx
)
aux_source_directory(Templates brad_io_sources)

vxl_add_library(LIBRARY_NAME brad_io LIBRARY_SOURCES  ${brad_io_sources})
target_link_libraries(brad_io brad ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()