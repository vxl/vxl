# vxl/vbl/io/CMakeLists.txt

ADD_LIBRARY(vbl_io vbl_io_sources)

SOURCE_FILES(vbl_io_sources
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

AUX_SOURCE_DIRECTORY(Templates vbl_io_sources)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
