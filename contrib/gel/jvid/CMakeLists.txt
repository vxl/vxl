# ./gel/jvid/CMakeLists.txt

SUBDIRS(bin)

ADD_LIBRARY(jvid jvid_sources)

SOURCE_FILES( jvid_sources
jvx_manager.cxx
)

AUX_SOURCE_DIRECTORY(Templates jvid_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/jvid/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)


