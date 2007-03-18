# v3p/geotiff/CMakeLists.txt

PROJECT( geotiff C )

IF(VXL_FORCE_V3P_TIFF)
INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
#IF(TIFF_FOUND)
#IF (EXISTS ${TIFF_INCLUDE_DIR}/tiffio.h)

 INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR} )

 SET( geotiff_sources
  cpl_serv.h
  defs.h
  geo_config.h
  geo_keyp.h
  geo_normalize.h
  geo_tiffp.h
  geokeys.h
  geonames.h
  geotiff.h
  geotiffio.h
  geovalues.h
  xtiffio.h

  ##  source files
  cpl_csv.c
  cpl_serv.c
  geo_extra.c
  geo_free.c
  geo_get.c
  geo_names.c
  geo_new.c
  geo_normalize.c
  geo_print.c
  geo_set.c
  geo_tiffp.c
  geo_trans.c
  geo_write.c
  geotiff_proj4.c
  xtiff.c
 )


 ADD_LIBRARY( geotiff ${geotiff_sources} )
 INSTALL_TARGETS(/lib geotiff)
 TARGET_LINK_LIBRARIES(geotiff ${TIFF_LIBRARIES})

#ENDIF (EXISTS ${TIFF_INCLUDE_DIR}/tiffio.h)
#ENDIF(TIFF_FOUND)
ENDIF(VXL_FORCE_V3P_TIFF)
