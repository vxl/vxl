ADD_LIBRARY( rsdl rsdl_sources )

SOURCE_FILES( rsdl_sources
  rsdl_bounding_box
  rsdl_dist
  rsdl_kd_tree
  rsdl_point
)

AUX_SOURCE_DIRECTORY( Templates rsdl_sources )

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vcl )

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
