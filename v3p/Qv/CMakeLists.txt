# ./v3p/Qv/CMakeLists.txt

ADD_LIBRARY( Qv Qv_sources )

SOURCE_FILES( Qv_sources
  vecutil.cxx
  QvLib.cxx
  wrltraverse.cxx
  wrlbuild.cxx
  QvVisitor.cxx
  QvVrmlFile.cxx
)

AUX_SOURCE_DIRECTORY( Templates Qv_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/v3p/Qv/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

#INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vcl )

IF(BUILD_TESTING)
  ADD_EXECUTABLE(test-compute-vrml-centroid test-compute-vrml-centroid.cxx)
  INCLUDE( ${allvxl_SOURCE_DIR}/v3p/Qv/CMakeListsLink.txt )
ENDIF(BUILD_TESTING)
