# bbas/volm/CMakeLists.txt
# Volumetric Matching and 3-d indexing library

# some classes depend on OPENCL being found...
# shouldn't be needed in a basic library (JLM)
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
ENDIF(OPENCL_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

SET(volm_sources
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
 volm_buffered_index.h					volm_buffered_index.cxx
 volm_candidate_list.h                  volm_candidate_list.cxx
 volm_geo_index2_node_base.h            volm_geo_index2_node_base.cxx
 volm_geo_index2_sptr.h
 volm_geo_index2.h                      volm_geo_index2.cxx
 volm_geo_index2.txx
 volm_osm_parser.h                      volm_osm_parser.cxx
 volm_category_io.h                     volm_category_io.cxx
 volm_osm_objects.h                     volm_osm_objects.cxx
 volm_osm_object_point.h                volm_osm_object_point.cxx
 volm_osm_object_line.h                 volm_osm_object_line.cxx
 volm_osm_object_polygon.h              volm_osm_object_polygon.cxx
 volm_satellite_resources.h             volm_satellite_resources.cxx
 volm_satellite_resources_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates volm_sources)

ADD_LIBRARY(volm  ${volm_sources})

TARGET_LINK_LIBRARIES(volm vsph bpgl vgl_algo vil vil_io vil_algo vbl_io vsol bkml bvgl vbl vsl vul bvrml depth_map )

# Base matcher, Proximity matcher, Visibility matcher related
SUBDIRS(desc)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)

SUBDIRS(pro)
