# ./gel/jvid/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)

# Build here only if OpenGL (and thus vgui) is available:
IF(HAS_OPENGL)

# Uses the vidl_avicodec, and thus will only build under Windows
IF(WIN32)

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

ENDIF(WIN32)
ENDIF(HAS_OPENGL)
