# ./vxl/vgl/CMakeLists.txt

ADD_LIBRARY(vgl vgl_sources)

SOURCE_FILES( vgl_sources
  vgl_clip.cxx
  vgl_distance.cxx
  vgl_polygon.cxx

  vgl_polygon_test.cxx
  vgl_lineseg_test.cxx
  vgl_triangle_test.cxx

  vgl_region_scan_iterator.cxx
  vgl_polygon_scan_iterator.cxx
  vgl_triangle_scan_iterator.cxx
  vgl_ellipse_scan_iterator.cxx

  vgl_test.cxx
  vgl_window_scan_iterator.cxx
  vgl_1d_basis.cxx
  vgl_homg.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SUBDIRS( algo )
SUBDIRS( io )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
