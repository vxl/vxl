#contrib/brl/bbas/volm/conf/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/ )

add_executable( volm_conf_create_land_map_indexer
                volm_conf_create_land_map_indexer.cxx
              )

add_executable( volm_conf_create_2d_index
                volm_conf_create_2d_index.cxx
              )

add_executable( volm_conf_visualize_index
                volm_conf_visualize_index.cxx
              )

target_link_libraries( volm_conf_create_land_map_indexer volm volm_conf )
target_link_libraries( volm_conf_create_2d_index volm volm_conf )
target_link_libraries( volm_conf_visualize_index volm volm_conf )
