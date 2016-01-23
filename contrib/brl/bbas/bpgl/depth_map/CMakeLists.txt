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

add_library(depth_map  ${depth_map_sources})

target_link_libraries(depth_map bpgl vpgl vpgl_io vpgl_algo vsol vnl_algo vnl vil vgl_io vgl_algo vgl vil_algo vbl_io vsol vbl vsl vul)



if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
