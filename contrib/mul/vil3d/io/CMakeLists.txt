# mul/vil3d/io/CMakeLists.txt

set(vil3d_io_sources
    vil3d_io_image_view.h
    # force building of a library
    vil3d_io_dummy.cxx
)

vxl_add_library(LIBRARY_NAME vil3d_io LIBRARY_SOURCES ${vil3d_io_sources})
target_link_libraries( vil3d_io vil3d ${VXL_LIB_PREFIX}vil_io )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
