# v3p/png/CMakeLists.txt
#   This is the png directory of v3p.  Current version is 1.2.5 of October 2002.

INCLUDE(${MODULE_PATH}/FindPNG.cmake)

IF(NOT VXL_USING_NATIVE_PNG)

  INCLUDE( ${MODULE_PATH}/FindZLIB.cmake )

  IF(ZLIB_FOUND)
    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )

    SET( png_sources
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

    ADD_LIBRARY( png ${png_sources} )

    # With cygwin, the there are multiple user configurations
    # possible. From pngconf.h:
    #     'Cygwin' defines/defaults:
    #       PNG_BUILD_DLL -- building the dll
    #       (no define)   -- building an application, linking to the dll
    #       PNG_STATIC    -- building the static lib, or building an application
    #                        which links to the static lib.
    #
    # Since we are here, there is no external png library. Therefore, we
    # build a static or shared based on the vxl build property, and rely
    # on FindPNG.cmake to propagate the appropriate build flags to user
    # code.
    #
    # NB: make sure this logic is consistent with FindPNG.cmake!

    IF (CYGWIN)
      # in Cygwin a define is needed by any file including
      # vxl/v3p/png/png.h (which in turn includes pngconf.h)
      IF(BUILD_SHARED_LIBS)
        ADD_DEFINITIONS(-DPNG_BUILD_DLL)
      ELSE(BUILD_SHARED_LIBS)
        ADD_DEFINITIONS(-DPNG_STATIC)
      ENDIF(BUILD_SHARED_LIBS)
    ENDIF (CYGWIN)

    TARGET_LINK_LIBRARIES( png ${ZLIB_LIBRARIES} )
    IF (UNIX)
      TARGET_LINK_LIBRARIES( png m )
    ENDIF (UNIX)
  ENDIF(ZLIB_FOUND)

ENDIF(NOT VXL_USING_NATIVE_PNG)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
