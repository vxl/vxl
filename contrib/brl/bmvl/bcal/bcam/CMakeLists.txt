# multi-view camera library

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(cam_sources
 camera.h                   camera.cxx
 camera_node.h              camera_node.cxx
 euclidean_transformation.h euclidean_transformation.cxx
 camera_graph.h
 calibrate_object.h         calibrate_object.cxx
 calibrate_plane.h          calibrate_plane.cxx
 zhang_camera_node.h        zhang_camera_node.cxx
 zhang_linear_calibrate.h   zhang_linear_calibrate.cxx
)

AUX_SOURCE_DIRECTORY(Templates cam_sources)

ADD_LIBRARY(bcal ${cam_sources})
TARGET_LINK_LIBRARIES(bcal vsol vnl vgl vgl_algo vbl vcsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
