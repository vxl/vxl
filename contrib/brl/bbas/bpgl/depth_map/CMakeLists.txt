# bpgl/depth_map/CMakeLists.txt
# image homography registration library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )


SET(depth_map_sources
 depth_map_scene.h   depth_map_scene.cxx  depth_map_scene_sptr.h
 depth_map_region.h  depth_map_region.cxx depth_map_region_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates depth_map_sources)

ADD_LIBRARY(depth_map  ${depth_map_sources})

TARGET_LINK_LIBRARIES(depth_map bpgl vpgl vpgl_io vpgl_algo vsol vnl_algo vnl vil vgl_io vgl_algo vgl vil_algo vbl_io vsol vbl vsl vul)



IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
