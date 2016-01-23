# volm/conf/CMakeLists.txt
# 2d/3d configuration matching and base library

include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(volm_conf_sources
  volm_conf_object.h              volm_conf_object.cxx
  volm_conf_query.h               volm_conf_query.cxx
  volm_conf_land_map_indexer.h    volm_conf_land_map_indexer.cxx
  volm_conf_buffer.h              volm_conf_buffer.txx
  volm_conf_indexer.h             volm_conf_indexer.cxx
  volm_conf_2d_indexer.h          volm_conf_2d_indexer.cxx
  volm_conf_score.h               volm_conf_score.cxx
)

aux_source_directory(Templates volm_conf_sources)

add_library(volm_conf ${volm_conf_sources})

target_link_libraries(volm_conf volm vgl_io vil vil_io vbl_io bkml bvgl bpgl vpgl vpgl_io vsol vbl vsl vul vnl bvrml depth_map vdgl sdet vtol bsol bvgl_algo)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

add_subdirectory(exe)
