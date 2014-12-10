# v3p/tiff/CMakeLists.txt

PROJECT( tiff C )

INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)

IF(NOT VXL_USING_NATIVE_TIFF)

  INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
  
  
  INCLUDE( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
  
  IF(ZLIB_FOUND)
    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
    SET( LZW_SUPPORT "1" )
    SET( ZIP_SUPPORT "1" )
  ENDIF(ZLIB_FOUND)
  
  INCLUDE( ${VXL_CMAKE_DIR}/FindJPEG.cmake )
  
  IF(JPEG_FOUND)
    INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
    SET( JPEG_SUPPORT  "1" )
  ENDIF(JPEG_FOUND)
  
  SET( tiff_sources
  # Header files
    tiff.h
    tiffcomp.h
    tiffconf.h
    tiffconf.h.in
    tif_config.h
    tiffio.h
    tiffvers.h
    t4.h
    tif_dir.h
    tif_predict.h
    tiffiop.h
    uvcode.h
    tif_fax3.h
    port.h
  
  # source files
    tif_aux.c
    tif_close.c
    tif_codec.c
    tif_color.c
    tif_compress.c
    tif_dir.c
    tif_dirinfo.c
    tif_dirread.c
    tif_dirwrite.c
    tif_dumpmode.c
    tif_error.c
    tif_extension.c
    tif_fax3.c
    tif_fax3sm.c
    tif_flush.c
    tif_getimage.c
    tif_jpeg.c
    tif_luv.c
    tif_lzw.c
    tif_next.c
    tif_ojpeg.c
    tif_open.c
    tif_packbits.c
    tif_pixarlog.c
    tif_predict.c
    tif_print.c
    tif_read.c
    tif_strip.c
    tif_swab.c
    tif_thunder.c
    tif_tile.c
    tif_version.c
    tif_warning.c
    tif_write.c
    tif_zip.c

    # C++ stream
    tif_stream.cxx
  )

  # put config header files in build tree      
  # For tif_config.h
  SET( HAVE_ASSERT_H    ${VCL_HAS_HEADER_ASSERT_H} )
  SET( HAVE_DLFCN_H     ${VCL_HAS_DLFCN_H} )
  SET( HAVE_FCNTL_H     ${VCL_HAS_FCNTL_H} )
  SET( HAVE_INTTYPES_H  ${VCL_HAS_INTTYPES_H} )
  SET( HAVE_LIMITS_H    ${VCL_HAS_LIMITS_H} )
  SET( HAVE_MALLOC_H    ${VCL_HAS_MALLOC_H} )
  SET( HAVE_MEMORY_H    ${VCL_HAS_MEMORY_H} )
  SET( HAVE_PTHREAD     ${VXL_HAS_PTHREAD_H} )
  SET( HAVE_SEARCH_H    ${VCL_HAS_SEARCH_H} )
  SET( HAVE_STDINT_H    ${VCL_HAS_STDINT_H} )
  SET( HAVE_STDLIB_H    ${VCL_HAS_STDLIB_H} )
  SET( HAVE_STRINGS_H   ${VCL_HAS_STRINGS_H} )
  SET( HAVE_STRING_H    ${VCL_HAS_STRING_H} )
  SET( HAVE_SYS_STAT_H  ${VCL_HAS_SYS_STAT_H} )
  SET( HAVE_SYS_TIME_H  ${VCL_HAS_SYS_TIME_H} )
  SET( HAVE_SYS_TYPES_H ${VCL_HAS_SYS_TYPES_H} )
  SET( HAVE_UNISTD_H    ${VCL_HAS_UNISTD_H} )
  SET( HAVE_WINDOWS_H   ${VCL_CXX_HAS_WINDOWS_H} )
  
  IF(WIN32)
    #SET(HOST_FILLORDER "FILLORDER_LSB2MSB")
    IF( NOT CYGWIN )
      SET( USE_WIN32_FILEIO 1 )
      
      # Suppress warnings of using unsecure CRT functions
      IF(MSVC)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
      ENDIF(MSVC)
    ENDIF( NOT CYGWIN )
  ELSE(WIN32)
    #TODO
    # SET(HOST_FILLORDER "FILLORDER_LSB2MSB")
  ENDIF(WIN32)

  CONFIGURE_FILE(tif_config.h.in  ${CMAKE_CURRENT_BINARY_DIR}/tif_config.h )
  CONFIGURE_FILE(tiffconf.h.in    ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h )
  INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

  IF(UNIX)
    SET( tiff_sources ${tiff_sources}
      tif_unix.c
    )
  ELSE(UNIX)
    IF(WIN32)
  
      # Disable warnings on using unsecured lib functions
      IF(MSVC)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
      ENDIF(MSVC)
  
  
      ## Copy the header files
      #FILE(COPY tif_config.vc.h  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
      #FILE(RENAME ${CMAKE_CURRENT_BINARY_DIR}/tif_config.vc.h ${CMAKE_CURRENT_BINARY_DIR}/tif_config.h)
      #FILE(COPY tiffconf.vc.h    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
      #FILE(RENAME ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.vc.h  ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h)
  
      # TODO  
      # Copy WinCE files for Wince build
      # JBIG Compression is disabled by default


      # Extra options from nmake.opt files
      
      
      # Use console/terminal output for error output.
      # Without this def it will use MessageBoxA() instead.
      ADD_DEFINITIONS( -DTIF_PLATFORM_CONSOLE)
      SET( tiff_sources ${tiff_sources}
        tif_win32.c
       #fax3sm_winnt.c
      )
    ENDIF(WIN32)
  ENDIF(UNIX)
  
  
  ADD_LIBRARY( tiff ${tiff_sources} )
  INSTALL_TARGETS( /lib tiff )
  TARGET_LINK_LIBRARIES( tiff ${ZLIB_LIBRARIES} ${JPEG_LIBRARIES})
  
  IF( BUILD_TESTING)
    SUBDIRS(test)
  ENDIF( BUILD_TESTING)

ENDIF(NOT VXL_USING_NATIVE_TIFF)
