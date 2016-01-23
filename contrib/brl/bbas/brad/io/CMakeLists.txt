# basic/brad/io/CMakeLists.txt
# Brown radiometry io Library.

set(brad_io_sources
  brad_io_image_metadata.h  brad_io_image_metadata.cxx
  brad_io_atmospheric_parameters.h brad_io_atmospheric_parameters.cxx
)
aux_source_directory(Templates brad_io_sources)

add_library(brad_io  ${brad_io_sources})
target_link_libraries(brad_io brad vnl_io vbl_io vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

install_targets(/lib brad_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/brad/io ${brad_io_sources})
