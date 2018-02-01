include_directories( ${VXLCORE_INCLUDE_DIR} )

set(baml_sources
    baml_birchfield_tomasi.h         baml_birchfield_tomasi.cxx
    baml_census.h                    baml_census.cxx
    baml_detect_change.h             baml_detect_change.cxx
    baml_utilities.h                 baml_utilities.cxx
    baml_warp.h                      baml_warp.cxx
   )
   
aux_source_directory(Templates baml_sources)

vxl_add_library(LIBRARY_NAME baml LIBRARY_SOURCES ${baml_sources})

target_link_libraries( baml ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}bsta)

add_subdirectory( app )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
