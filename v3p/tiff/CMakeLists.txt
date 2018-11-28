# v3p/tiff/CMakeLists.txt

project( tiff C )

include(${VXL_CMAKE_DIR}/FindTIFF.cmake)

if(NOT VXL_USING_NATIVE_TIFF)

  include( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )


  include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )

  if(ZLIB_FOUND)
    include_directories( ${ZLIB_INCLUDE_DIR} )
    set( LZW_SUPPORT 1 )
    set( ZIP_SUPPORT 1 )
  endif()

  include( ${VXL_CMAKE_DIR}/FindJPEG.cmake )

  if(JPEG_FOUND)
    include_directories( ${JPEG_INCLUDE_DIR} )
    set( JPEG_SUPPORT  1 )
  endif()

  set( tiff_sources
  # Header files
    tiff.h
    tiffcomp.h
    tiffio.h
    tiffvers.h
    t4.h
    tif_dir.h
    tif_predict.h
    tiffiop.h
    uvcode.h
    tif_fax3.h

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
  set( HAVE_ASSERT_H    ${VCL_HAS_HEADER_ASSERT_H} )
  set( HAVE_DLFCN_H     ${VCL_HAS_DLFCN_H} )
  set( HAVE_FCNTL_H     ${VCL_HAS_FCNTL_H} )
  set( HAVE_INTTYPES_H  ${VCL_HAS_INTTYPES_H} )
  set( HAVE_LIMITS_H    ${VCL_HAS_LIMITS_H} )
  set( HAVE_MALLOC_H    ${VCL_HAS_MALLOC_H} )
  set( HAVE_MEMORY_H    ${VCL_HAS_MEMORY_H} )
  set( HAVE_PTHREAD     ${VXL_HAS_PTHREAD_H} )
  set( HAVE_SEARCH_H    ${VCL_HAS_SEARCH_H} )
  set( HAVE_STDINT_H    ${VCL_HAS_STDINT_H} )
  set( HAVE_STDLIB_H    ${VCL_HAS_STDLIB_H} )
  set( HAVE_STRINGS_H   ${VCL_HAS_STRINGS_H} )
  set( HAVE_STRING_H    ${VCL_HAS_STRING_H} )
  set( HAVE_SYS_STAT_H  ${VCL_HAS_SYS_STAT_H} )
  set( HAVE_SYS_TIME_H  ${VCL_HAS_SYS_TIME_H} )
  set( HAVE_SYS_TYPES_H ${VCL_HAS_SYS_TYPES_H} )
  set( HAVE_UNISTD_H    ${VCL_HAS_UNISTD_H} )
  set( HAVE_WINDOWS_H   ${VCL_CXX_HAS_WINDOWS_H} )
  set( HOST_BIGENDIAN   ${VXL_BIG_ENDIAN} )
  set( HAVE_IEEEFP      ${VXL_HAS_IEEEFP_H} )
  set( PIXARLOG_SUPPORT ${LZW_SUPPORT} )

  if(WIN32)
    #set(HOST_FILLORDER "FILLORDER_LSB2MSB")
    if( NOT CYGWIN )
      set( USE_WIN32_FILEIO 1 )

    endif()
  else()
    #TODO
    # set(HOST_FILLORDER "FILLORDER_LSB2MSB")
  endif()

  # Figure out the format prefix for 64-bit integer
  # Info from http://www.viva64.com/en/k/0046/
  if(VXL_INT_64_IS_LONGLONG)
    set(_FORMAT_64BIT_PREFIX "ll")
  else()
    set(_FORMAT_64BIT_PREFIX "l")
  endif()
  set(_FORMAT_SSIZE_PREFIX "z")
  if(WIN32)
    set(_FORMAT_64BIT_PREFIX "I64")
    set(_FORMAT_SSIZE_PREFIX "I")
  endif()
  set(TIFF_INT64_FORMAT "%${_FORMAT_64BIT_PREFIX}d")
  set(TIFF_UINT64_FORMAT "%${_FORMAT_64BIT_PREFIX}u")
  set(TIFF_SSIZE_FORMAT "%${_FORMAT_SSIZE_PREFIX}u")
  set(TIFF_PTRDIFF_FORMAT "%${_FORMAT_SSIZE_PREFIX}d")

  vxl_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tif_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/tif_config.h v3p/tiff )
  vxl_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tiffconf.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h v3p/tiff )

  include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

  if(UNIX)
    set( tiff_sources ${tiff_sources}
      tif_unix.c
    )
  else()
    if(WIN32)

      # Disable warnings on using unsecured CRT lib functions
      if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      endif()

      # TODO
      # Copy WinCE files for Wince build
      # JBIG Compression is disabled by default


      # Extra options from nmake.opt files


      # Use console/terminal output for error output.
      # Without this def it will use MessageBoxA() instead.
      add_definitions( -DTIF_PLATFORM_CONSOLE)
      set( tiff_sources ${tiff_sources}
        tif_win32.c
      )
    endif()
  endif()
  vxl_add_library(LIBRARY_NAME tiff LIBRARY_SOURCES ${tiff_sources} )
  target_link_libraries( tiff ${ZLIB_LIBRARIES} ${JPEG_LIBRARIES})

  if( BUILD_TESTING)
    add_subdirectory(test)
  endif()

endif()
