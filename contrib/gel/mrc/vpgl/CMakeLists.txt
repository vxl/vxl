# gel/mrc/vpgl/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)

SET(vpgl_sources
 vpgl_basic_camera.cxx       vpgl_basic_camera.h       vpgl_basic_camera_sptr.h
 vpgl_matrix_camera.cxx      vpgl_matrix_camera.h      vpgl_matrix_camera_sptr.h
 vpgl_perspective_camera.cxx vpgl_perspective_camera.h vpgl_perspective_camera_sptr.h
 vpgl_rational_camera.cxx vpgl_rational_camera.h
)

AUX_SOURCE_DIRECTORY(Templates vpgl_sources)

ADD_LIBRARY(vpgl ${vpgl_sources})
TARGET_LINK_LIBRARIES(vpgl vcsl vnl_algo vbl vcl)
