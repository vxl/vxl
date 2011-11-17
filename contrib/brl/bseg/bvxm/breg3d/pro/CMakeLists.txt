# This is contrib/dec/breg3d/pro/CMakeLists.txt
# 3-d registration processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET(breg3d_pro_sources
    breg3d_ekf_camera_optimize_process.h  breg3d_ekf_camera_optimize_process.cxx
    breg3d_init_ekf_camera_optimizer_process.h  breg3d_init_ekf_camera_optimizer_process.cxx
    breg3d_set_occupancy_plane_process.h   breg3d_set_occupancy_plane_process.cxx
    breg3d_zero_occupancy_borders_process.h   breg3d_zero_occupancy_borders_process.cxx
    breg3d_init_ekf_camera_optimizer_planar_process.h   breg3d_init_ekf_camera_optimizer_planar_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates breg3d_pro_sources)

ADD_LIBRARY(breg3d_pro ${breg3d_pro_sources})

TARGET_LINK_LIBRARIES(breg3d_pro breg3d bprb vpgl_pro vpgl vpgl_io vil vil_io vnl vnl_io  vgl vgl_algo vsol brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
