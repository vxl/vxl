# v3p/tiff/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindNativeTIFF.cmake)

IF(NOT HAS_NATIVE_TIFF)

ADD_LIBRARY( tiff tiff_sources )

SOURCE_FILES( tiff_sources
  format.h
  machdep.h
  port.h
  prototypes.h
  t4.h
  tiff.h
  tiffcomp.h
  tiffcompat.h
  tiffconf.h
  tiffio.h
  tiffiop.h
  uvcode.h
  version.h

  tif_aux.c
  tif_close.c
  tif_codec.c
  tif_compress.c
  tif_dir.c tif_dir.h
  tif_dirinfo.c
  tif_dirread.c
  tif_dirwrite.c
  tif_dumpmode.c
  tif_error.c
  tif_fax3.c tif_fax3.h
  tif_fax3sm.c
  tif_flush.c
  tif_getimage.c
  tif_jpeg.c
  tif_luv.c
  tif_lzw.c
  tif_next.c
  tif_open.c
  tif_packbits.c
  tif_pixarlog.c
  tif_predict.c tif_predict.h
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
)

IF(UNIX)
  SOURCE_FILES( tiff_sources
    tif_unix.c
  )
ELSE(UNIX)
  ADD_DEFINITIONS( -DTIF_PLATFORM_CONSOLE)

  SOURCE_FILES( tiff_sources
    tif_win32.c
  )
ENDIF(UNIX)

ENDIF(NOT HAS_NATIVE_TIFF)
