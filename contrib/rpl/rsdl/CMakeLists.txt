# ./rpl/rsdl/CMakeLists.txt

ADD_LIBRARY( rsdl rsdl_sources )

SOURCE_FILES( rsdl_sources
  rsdl_bounding_box.cxx
  rsdl_dist.cxx
  rsdl_kd_tree.cxx
  rsdl_point.cxx
)

AUX_SOURCE_DIRECTORY( Templates rsdl_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/rpl/rsdl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
