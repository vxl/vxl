# v3p/png/CMakeLists.txt

# Do nothing here, unless there is no native PNG
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativePNG.cmake)
IF(NOT HAS_NATIVE_PNG)

ADD_LIBRARY( png png_sources )

SOURCE_FILES( png_sources
  pngasmrd.h
  pngconf.h

  png.c png.h
  pngset.c
  pngget.c
  pngrutil.c
  pngtrans.c
  pngwutil.c
  pngread.c
  pngrio.c
  pngwio.c
  pngwrite.c
  pngrtran.c
  pngwtran.c
  pngmem.c
  pngerror.c
  pngpread.c
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/v3p/png/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF (CYGWIN)
  # in Cygwin a define is needed by any file including vxl/v3p/png/png.h
  IF(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DPNG_BUILD_DLL)
  ELSE(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DPNG_STATIC)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF (CYGWIN)

INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindZLIB.cmake )

ENDIF(NOT HAS_NATIVE_PNG)
