# ./gel/mrc/vpgl/CMakeLists.txt

ADD_LIBRARY( vpgl vpgl_sources )

SOURCE_FILES( vpgl_sources
  vpgl_basic_camera.cxx
  vpgl_matrix_camera.cxx
  vpgl_perspective_camera.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/mrc/vpgl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

AUX_SOURCE_DIRECTORY( Templates vpgl_sources )

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel/mrc )
