# ./oxl/oxltest/CMakeLists.txt

SOURCE_FILES( oxltest_sources oxltest.cpp )

ADD_EXECUTABLE( oxltest oxltest_sources )
ADD_TEST(       oxltest oxltest         )

IF(WIN32)
  IF (NOT CYGWIN)
    SOURCE_FILES( oxltest_sources StdAfx.cpp )
    SOURCE_FILES( oxltest_sources StdAfx.h )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
