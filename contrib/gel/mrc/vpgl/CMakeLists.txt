# gel/mrc/vpgl/CMakeLists.txt

ADD_LIBRARY(vpgl vpgl_sources)

SOURCE_FILES(vpgl_sources
 vpgl_basic_camera.cxx       vpgl_basic_camera.h       vpgl_basic_camera_sptr.h
 vpgl_matrix_camera.cxx      vpgl_matrix_camera.h      vpgl_matrix_camera_sptr.h
 vpgl_perspective_camera.cxx vpgl_perspective_camera.h vpgl_perspective_camera_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates vpgl_sources)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/gel/mrc)
