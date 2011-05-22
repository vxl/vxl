# v3p/tiff/CMakeLists.txt

PROJECT( tiff C )

INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)

IF(NOT VXL_USING_NATIVE_TIFF)

INCLUDE( ${VXL_CMAKE_DIR}/FindZLIB.cmake )

IF(ZLIB_FOUND)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )

INCLUDE( ${VXL_CMAKE_DIR}/FindJPEG.cmake )

IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )

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
  tif_stream.cxx
  tif_swab.c
  tif_thunder.c
  tif_tile.c
  tif_version.c
  tif_warning.c
  tif_write.c
  tif_zip.c
)

IF(UNIX)
  SET( tiff_sources ${tiff_sources}
    tif_unix.c
  )
ELSE(UNIX)
  ADD_DEFINITIONS( -DTIF_PLATFORM_CONSOLE)

  SET( tiff_sources ${tiff_sources}
    tif_win32.c
   #fax3sm_winnt.c
  )
ENDIF(UNIX)


ADD_LIBRARY( tiff ${tiff_sources} )
INSTALL_TARGETS( /lib tiff )
TARGET_LINK_LIBRARIES( tiff ${ZLIB_LIBRARIES} ${JPEG_LIBRARIES})
ENDIF(JPEG_FOUND)
ENDIF(ZLIB_FOUND)
ENDIF(NOT VXL_USING_NATIVE_TIFF)
