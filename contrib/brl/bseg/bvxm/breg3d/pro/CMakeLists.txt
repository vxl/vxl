# This is contrib/dec/breg3d/pro/CMakeLists.txt
# 3-d registration processes

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${VXLCORE_INCLUDE_DIR} )

set(breg3d_pro_sources
    breg3d_ekf_camera_optimize_process.h  breg3d_ekf_camera_optimize_process.cxx
    breg3d_init_ekf_camera_optimizer_process.h  breg3d_init_ekf_camera_optimizer_process.cxx
    breg3d_set_occupancy_plane_process.h   breg3d_set_occupancy_plane_process.cxx
    breg3d_zero_occupancy_borders_process.h   breg3d_zero_occupancy_borders_process.cxx
    breg3d_init_ekf_camera_optimizer_planar_process.h   breg3d_init_ekf_camera_optimizer_planar_process.cxx
)

aux_source_directory(Templates breg3d_pro_sources)

add_library(breg3d_pro ${breg3d_pro_sources})

target_link_libraries(breg3d_pro breg3d bprb vpgl_pro vpgl vpgl_io vil vil_io vnl vnl_io  vgl vgl_algo vsol brip brdb)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
