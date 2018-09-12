# vxl/vbl/io/CMakeLists.txt

set(vbl_io_sources
  vbl_io_smart_ptr.hxx vbl_io_smart_ptr.h
  vbl_io_array_1d.hxx vbl_io_array_1d.h
  vbl_io_array_2d.hxx vbl_io_array_2d.h
  vbl_io_array_3d.hxx vbl_io_array_3d.h
  vbl_io_sparse_array_base.hxx vbl_io_sparse_array_base.h
  vbl_io_sparse_array_1d.h
  vbl_io_sparse_array_2d.h
  vbl_io_sparse_array_3d.h
  vbl_io_bounding_box.hxx vbl_io_bounding_box.h
  vbl_io_triple.hxx vbl_io_triple.h
  vbl_io_quadruple.hxx vbl_io_quadruple.h
)

aux_source_directory(Templates vbl_io_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vbl_io LIBRARY_SOURCES ${vbl_io_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vbl_io-obj OBJECT ${vbl_io_sources})
endif()
