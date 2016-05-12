# volm/conf/CMakeLists.txt
# 2d/3d configuration matching and base library

include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(volm_conf_sources
  volm_conf_object.h              volm_conf_object.cxx
  volm_conf_query.h               volm_conf_query.cxx
  volm_conf_land_map_indexer.h    volm_conf_land_map_indexer.cxx
  volm_conf_buffer.h              volm_conf_buffer.hxx
  volm_conf_indexer.h             volm_conf_indexer.cxx
  volm_conf_2d_indexer.h          volm_conf_2d_indexer.cxx
  volm_conf_score.h               volm_conf_score.cxx
)

aux_source_directory(Templates volm_conf_sources)

vxl_add_library(LIBRARY_NAME volm_conf LIBRARY_SOURCES ${volm_conf_sources})

target_link_libraries(volm_conf volm ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vbl_io bkml bvgl bpgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io vsol ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vnl bvrml depth_map vdgl sdet vtol bsol bvgl_algo)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
