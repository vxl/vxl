# bpgl/depth_map/CMakeLists.txt
# image homography registration library

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
include_directories( ${GEL_INCLUDE_DIR} )


set(depth_map_sources
 depth_map_scene.h   depth_map_scene.cxx  depth_map_scene_sptr.h
 depth_map_region.h  depth_map_region.cxx depth_map_region_sptr.h
)

aux_source_directory(Templates depth_map_sources)

vxl_add_library(LIBRARY_NAME depth_map LIBRARY_SOURCES  ${depth_map_sources})

target_link_libraries(depth_map bpgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_algo vsol ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vbl_io vsol ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)



if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
