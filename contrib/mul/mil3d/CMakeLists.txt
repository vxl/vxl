#
# ./mul/mil3d/CMakeLists.txt
#
#   Graham Vincent
#

ADD_LIBRARY(mil3d mil3d_sources )

SOURCE_FILES(mil3d_sources
   mil3d_transform_3d.cxx
   mil3d_image_3d_of.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil3d/CMakeListsHeaders.txt )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY(Templates mil3d_sources )

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
