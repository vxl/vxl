# v3p/geotiff/CMakeLists.txt

if (${VXL_USE_GEOTIFF})

  project( geotiff C )

  include(${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)

  if(NOT VXL_USING_NATIVE_GEOTIFF)
    include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
    #if(TIFF_FOUND)
    #if(EXISTS ${TIFF_INCLUDE_DIR}/tiffio.h)

    include_directories(${TIFF_INCLUDE_DIR} )

    set( geotiff_sources
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


    vxl_add_library(LIBRARY_NAME geotiff LIBRARY_SOURCES ${geotiff_sources} )
    target_link_libraries(geotiff ${TIFF_LIBRARIES})

    #endif()
  endif()
endif()
