#
# ./v3p/Qv/CMakeLists.txt
#

ADD_LIBRARY( Qv Qv_sources )

SOURCE_FILES( Qv_sources
  vecutil
  QvLib
  wrltraverse
  wrlbuild
  QvVisitor
  QvVrmlFile
)

AUX_SOURCE_DIRECTORY( Templates Qv_sources )

IF(BUILD_TESTING)
  ADD_EXECUTABLE(test-compute-vrml-centroid test-compute-vrml-centroid.cxx)
  INCLUDE( ${allvxl_SOURCE_DIR}/v3p/Qv/CMakeListsLink.txt )
ENDIF(BUILD_TESTING)
