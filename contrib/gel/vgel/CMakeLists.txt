# ./gel/vgel/CMakeLists.txt

ADD_LIBRARY(vgel vgel_sources)

SOURCE_FILES(vgel_sources
  vgel_kl.cxx
  vgel_kl_params.cxx
# vgel_multi_view_data.txx
)

AUX_SOURCE_DIRECTORY(Templates vgel_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vgel/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

SUBDIRS(kl)
IF( BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF( BUILD_EXAMPLES)
