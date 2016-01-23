# mul/vil3d/io/CMakeLists.txt

set(vil3d_io_sources
    vil3d_io_image_view.h
    # force building of a library
    vil3d_io_dummy.cxx
)

add_library(vil3d_io ${vil3d_io_sources})
install_targets(/lib vil3d_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/mul/vil3d/io ${vil3d_io_sources})
target_link_libraries( vil3d_io vil3d vil_io )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif( BUILD_TESTING)
