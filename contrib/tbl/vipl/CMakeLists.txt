# ./tbl/vipl/CMakeLists.txt

ADD_LIBRARY(vipl vipl_sources)

SOURCE_FILES(vipl_sources
  filter/vipl_filter_abs.cxx
)

AUX_SOURCE_DIRECTORY(Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_vbl_array_2d/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_vnl_matrix/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_section/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_mil/Templates vipl_sources)
# and for the latter, we need mul:
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/mul)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/tbl/vipl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
