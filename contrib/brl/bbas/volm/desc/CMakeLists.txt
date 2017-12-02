# volm/desc/CMakeLists.txt
# Existence matching and base matching library

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/volm )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bsta )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )

set(volm_desc_sources
  volm_desc.h                             volm_desc.cxx
  volm_desc_ex.h                          volm_desc_ex.cxx
  volm_desc_ex_2d.h                       volm_desc_ex_2d.cxx
  volm_desc_ex_land_only.h                volm_desc_ex_land_only.cxx
  volm_desc_land.h                        volm_desc_land.cxx
  volm_desc_matcher.h                     volm_desc_matcher.cxx
  volm_desc_indexer.h                     volm_desc_indexer.cxx
  volm_desc_land_matcher.h                volm_desc_land_matcher.cxx
  volm_desc_land_indexer.h                volm_desc_land_indexer.cxx
  volm_desc_ex_2d_indexer.h               volm_desc_ex_2d_indexer.cxx
  volm_desc_ex_2d_matcher.h               volm_desc_ex_2d_matcher.cxx
)

aux_source_directory(Templates volm_desc_sources)

vxl_add_library(LIBRARY_NAME volm_desc LIBRARY_SOURCES ${volm_desc_sources})

target_link_libraries(volm_desc volm bsta bsta_algo bsta_vis bsvg bvgl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul depth_map)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(exe)
