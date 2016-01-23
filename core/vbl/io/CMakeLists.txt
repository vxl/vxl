# vxl/vbl/io/CMakeLists.txt

set(vbl_io_sources
  vbl_io_smart_ptr.txx vbl_io_smart_ptr.h
  vbl_io_array_1d.txx vbl_io_array_1d.h
  vbl_io_array_2d.txx vbl_io_array_2d.h
  vbl_io_array_3d.txx vbl_io_array_3d.h
  vbl_io_sparse_array_base.txx vbl_io_sparse_array_base.h
  vbl_io_sparse_array_1d.h
  vbl_io_sparse_array_2d.h
  vbl_io_sparse_array_3d.h
  vbl_io_bounding_box.txx vbl_io_bounding_box.h
  vbl_io_triple.txx vbl_io_triple.h
  vbl_io_quadruple.txx vbl_io_quadruple.h
)

aux_source_directory(Templates vbl_io_sources)

add_library(vbl_io ${vbl_io_sources})
target_link_libraries( vbl_io vbl vsl )

install_targets(/lib vbl_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vbl/io ${vbl_io_sources})

if( BUILD_TESTING)
  add_subdirectory(tests)
endif( BUILD_TESTING)
