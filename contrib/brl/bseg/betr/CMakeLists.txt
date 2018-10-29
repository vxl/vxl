# Event triggers
# contrib/brl/bseg/betr/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR})
include_directories( ${VCSL_INCLUDE_DIR})

set(betr_sources
 betr_geo_box_3d.h   betr_geo_box_3d.cxx
 betr_geo_object_3d.h   betr_geo_object_3d.cxx
 betr_kml_utils.h    betr_kml_utils.cxx
 vsol_mesh_3d.h      vsol_mesh_3d.cxx #defined here to avoid gel/brl dependency
 betr_site.h         betr_site.cxx
 betr_edgel_factory.h  betr_edgel_factory.cxx
 betr_edgel_factory_params.h  betr_edgel_factory_params.cxx
 betr_event_trigger.h  betr_event_trigger.cxx
 betr_io.h             betr_io.cxx
 betr_algorithm.h      betr_algorithm.cxx betr_algorithm_sptr.h
 betr_edgel_change_detection.h  betr_edgel_change_detection.cxx
 betr_edgel_change_detection_params.h  betr_edgel_change_detection_params.cxx
 betr_edgel_reference_cd.h  betr_edgel_reference_cd.cxx
 betr_edgel_reference_cd_params.h    betr_edgel_reference_cd_params.cxx
 betr_pixelwise_change_detection.h  betr_pixelwise_change_detection.cxx
 betr_pixelwise_change_detection_params.h  betr_pixelwise_change_detection_params.cxx
 betr_params.h              betr_params.cxx  betr_params_sptr.h
 betr_gridded_geo_polygon_3d.h   betr_gridded_geo_polygon_3d.cxx
)
aux_source_directory(Templates betr_sources)

vxl_add_library(LIBRARY_NAME betr LIBRARY_SOURCES  ${betr_sources})

target_link_libraries(betr bbas_pro ${VXL_LIB_PREFIX}vsph ${VXL_LIB_PREFIX}vsol ${VXL_LIB_PREFIX}bmsh3d ${VXL_LIB_PREFIX}bmsh3d_algo ${VXL_LIB_PREFIX}sdet ${VXL_LIB_PREFIX}brip ${VXL_LIB_PREFIX}vdgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vcsl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}baml ${VXL_LIB_PREFIX}bjson ${VXL_LIB_PREFIX}vpl )

add_subdirectory(pro)

add_subdirectory( app )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()


