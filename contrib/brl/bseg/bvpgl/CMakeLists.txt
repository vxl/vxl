# gel/mrc/vpgl/algo/CMakeLists.txt
# Photogrammetry Algorithms Library.


INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )

SET( bvpgl_sources
  bvpgl_camera_estimator.h          bvpgl_camera_estimator.txx
  bvpgl_camera_estimator_amoeba.h
)

#AUX_SOURCE_DIRECTORY(Templates bvpgl_sources)

ADD_LIBRARY(bvpgl ${bvpgl_sources})
TARGET_LINK_LIBRARIES( bvpgl vpgl brip vnl vgl vil vul vbl vcl)


