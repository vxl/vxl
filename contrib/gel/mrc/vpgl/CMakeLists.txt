ADD_LIBRARY( vpgl vpgl_sources )

SOURCE_FILES( vpgl_sources
  vpgl_basic_camera
  vpgl_matrix_camera
  vpgl_perspective_camera
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel/mrc )
