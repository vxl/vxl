# brl/bpro/core/vpgl_pro/CMakeLists.txt
# Brown vpgl processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET( vpgl_pro_sources
  vpgl_load_proj_camera_process.h          vpgl_load_proj_camera_process.cxx
  vpgl_load_rational_camera_process.h      vpgl_load_rational_camera_process.cxx
  vpgl_load_rational_camera_nitf_process.h vpgl_load_rational_camera_nitf_process.cxx
  vpgl_load_perspective_camera_process.h   vpgl_load_perspective_camera_process.cxx
  vpgl_save_perspective_camera_process.h   vpgl_save_perspective_camera_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_pro_sources)

ADD_LIBRARY(vpgl_pro ${vpgl_pro_sources})
TARGET_LINK_LIBRARIES(vpgl_pro bprb brdb vpgl_file_formats vpgl_io vpgl vnl vil)

