#
# Find a PNG library
#

IF(NOT HAS_PNG)
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativePNG.cmake )
  
  IF(NOT HAS_NATIVE_PNG)
  
    #
    # At some point, in a "release" version, it is possible that someone
    # will not have the v3p png library, so make sure the headers
    # exist.
    #
  
    FIND_PATH( PNG_INCLUDE_PATH png.h
      ${allvxl_SOURCE_DIR}/v3p/png
    )
  
    IF(PNG_INCLUDE_PATH)
  
      SET( HAS_PNG "YES" )
      ADD_DEFINITIONS( -DHAS_PNG )
  
      INCLUDE( ${allvxl_SOURCE_DIR}/v3p/png/CMakeListsLink.txt )

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
      # NB: make sure this logic is consistent with v3p/png/CMakeLists.txt

      IF (CYGWIN)
        # in Cygwin a define is needed by any file including
        # vxl/v3p/png/png.h (which in turn includes pngconf.h)
        IF(BUILD_SHARED_LIBS)
          ADD_DEFINITIONS(-DPNG_BUILD_DLL)
        ELSE(BUILD_SHARED_LIBS)
          ADD_DEFINITIONS(-DPNG_STATIC)
        ENDIF(BUILD_SHARED_LIBS)
      ENDIF (CYGWIN)

    ENDIF(PNG_INCLUDE_PATH)
  
  ELSE(NOT HAS_NATIVE_PNG)
  
    SET( HAS_PNG "YES" )
    ADD_DEFINITIONS( -DHAS_PNG )
  
  ENDIF(NOT HAS_NATIVE_PNG)

  # libpng uses zlib for compressed PNG files
  INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindZLIB.cmake )
ENDIF(NOT HAS_PNG)
