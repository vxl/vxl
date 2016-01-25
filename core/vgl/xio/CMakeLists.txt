# vxl/vgl/xio/CMakeLists.txt

set(vgl_xio_sources
  vgl_xio_point_2d.txx vgl_xio_point_2d.h
  vgl_xio_point_3d.txx vgl_xio_point_3d.h
  vgl_xio_box_3d.txx vgl_xio_box_3d.h
  vgl_xio_vector_3d.txx vgl_xio_vector_3d.h
)

aux_source_directory(Templates vgl_xio_sources)

add_library(vgl_xio ${vgl_xio_sources})
if(NOT WIN32)
  set_target_properties(vgl_xio PROPERTIES COMPILE_FLAGS "-fPIC")
endif()
target_link_libraries( vgl_xio vgl vsl )

install_targets(/lib vgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vgl/xio ${vgl_xio_sources})

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
