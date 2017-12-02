# This is contrib/dec/breg3d/pro/CMakeLists.txt
# 3-d registration processes

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(breg3d_pro_sources
    breg3d_ekf_camera_optimize_process.h  breg3d_ekf_camera_optimize_process.cxx
    breg3d_init_ekf_camera_optimizer_process.h  breg3d_init_ekf_camera_optimizer_process.cxx
    breg3d_set_occupancy_plane_process.h   breg3d_set_occupancy_plane_process.cxx
    breg3d_zero_occupancy_borders_process.h   breg3d_zero_occupancy_borders_process.cxx
    breg3d_init_ekf_camera_optimizer_planar_process.h   breg3d_init_ekf_camera_optimizer_planar_process.cxx
)

aux_source_directory(Templates breg3d_pro_sources)

vxl_add_library(LIBRARY_NAME breg3d_pro LIBRARY_SOURCES ${breg3d_pro_sources})

target_link_libraries(breg3d_pro breg3d bprb vpgl_pro ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo vsol brip brdb)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
