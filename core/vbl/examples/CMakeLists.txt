ADD_LIBRARY( vbl_example_templates vbl_example_templates_sources )
AUX_SOURCE_DIRECTORY( Templates vbl_example_templates_sources )

LINK_LIBRARIES( vbl_example_templates )

ADD_EXECUTABLE( vbl_smart_ptr_example vbl_smart_ptr_example.cxx )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vbl/examples/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
