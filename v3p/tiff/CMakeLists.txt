ADD_LIBRARY( tiff tiff_sources )

SOURCE_FILES( tiff_sources
tif_aux
tif_close
tif_codec
tif_compress
tif_dir
tif_dirinfo
tif_dirread
tif_dirwrite
tif_dumpmode
tif_error
tif_fax3
tif_fax3sm
tif_flush
tif_getimage
tif_jpeg
tif_luv
tif_lzw
tif_next
tif_open
tif_packbits
tif_pixarlog
tif_predict
tif_print
tif_read
tif_strip
tif_swab
tif_thunder
tif_tile
tif_version
tif_warning
tif_write
tif_zip
)

IF(WIN32)
  ADD_DEFINITIONS( -DTIF_PLATFORM_CONSOLE)

  SOURCE_FILES( tiff_sources
    tif_win32
  )
ELSE(WIN32)
  SOURCE_FILES( tiff_sources
    tif_unix
  )
ENDIF(WIN32)
