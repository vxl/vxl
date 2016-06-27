# bbas/volm/CMakeLists.txt
# Volumetric Matching and 3-d indexing library

# volm depends on bkml and expat

include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(HAS_GEOTIFF)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
    include_directories( ${GEL_INCLUDE_DIR} )
    include_directories(${EXPAT_INCLUDE_DIRS} )
    include_directories(${EXPATPP_INCLUDE_DIRS} )

    set(volm_sources
     volm_export.h
     volm_io.h                              volm_io.cxx
     volm_io_tools.h                        volm_io_tools.cxx
     volm_tile.h                            volm_tile.cxx
     volm_spherical_container.h             volm_spherical_container.cxx
     volm_spherical_container_sptr.h
     volm_spherical_shell_container.h       volm_spherical_shell_container.cxx
     volm_spherical_shell_container_sptr.h
     volm_query.h                           volm_query.cxx
     volm_query_sptr.h
     volm_loc_hyp.h                         volm_loc_hyp.cxx
     volm_loc_hyp_sptr.h
     volm_geo_index.h  volm_geo_index.cxx
     volm_geo_index_sptr.h
     volm_camera_space.h                    volm_camera_space.cxx
     volm_camera_space_sptr.h
     volm_spherical_query_region.h          volm_spherical_query_region.cxx
     volm_spherical_region_query.h          volm_spherical_region_query.cxx
     volm_spherical_region_index.h          volm_spherical_region_index.cxx
     volm_spherical_layers.h                volm_spherical_layers.cxx
     volm_spherical_index_query_matcher.h   volm_spherical_index_query_matcher.cxx
     volm_spherical_region.h                volm_spherical_region.cxx
     volm_vrml_io.h                         volm_vrml_io.cxx
     volm_buffered_index.h                  volm_buffered_index.cxx
     volm_candidate_list.h                  volm_candidate_list.cxx
     volm_geo_index2_node_base.h            volm_geo_index2_node_base.cxx
     volm_geo_index2_sptr.h
     volm_geo_index2.h                      volm_geo_index2.cxx
     volm_geo_index2.hxx
     volm_category_io.h                     volm_category_io.cxx
     volm_satellite_resources.h             volm_satellite_resources.cxx
     volm_satellite_resources_sptr.h
     volm_utils.h                           volm_utils.hxx
     volm_osm_parser.h                      volm_osm_parser.cxx
     volm_osm_objects.h                     volm_osm_objects.cxx
     volm_osm_object_point.h                volm_osm_object_point.cxx
     volm_osm_object_line.h                 volm_osm_object_line.cxx
     volm_osm_object_polygon.h              volm_osm_object_polygon.cxx
     volm_candidate_region_parser.h         volm_candidate_region_parser.cxx
     volm_utils.h                           volm_utils.hxx                   volm_utils.cxx
    )

    aux_source_directory(Templates volm_sources)

    vxl_add_library(LIBRARY_NAME volm LIBRARY_SOURCES  ${volm_sources})

    target_link_libraries(volm vsph bpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vbl_io vsol bkml bvgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul bvrml depth_map brad)
    target_link_libraries(volm ${EXPAT_LIBRARIES})
    if(APPLE)
      target_link_libraries(volm expat)
    endif()
    target_link_libraries(volm ${EXPATPP_LIBRARIES})
    # Base matcher, Proximity matcher, Visibility matcher related
    add_subdirectory(desc)

    # 2-d, 3-d Configurational matcher related
    add_subdirectory(conf)

    if(BUILD_TESTING)
      add_subdirectory(tests)
    endif()

    add_subdirectory(exe)

    add_subdirectory(pro)
  endif()
endif()
else()
  message(STATUS "Skipping contrib/brl/bbas/volm: requires geotiff")
endif()
