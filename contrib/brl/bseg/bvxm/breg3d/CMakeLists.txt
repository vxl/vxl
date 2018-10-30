# This is bseg/bvxm/breg3d/CMakeLists.txt
# 3-d video registration using the bvxm library
if(VXL_BUILD_MUL)
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${BRL_INCLUDE_DIR}/bbas/bpgl )
  include_directories( ${MUL_INCLUDE_DIR})

  set(breg3d_sources
      breg3d_ekf_camera_optimizer.h            breg3d_ekf_camera_optimizer.cxx
      breg3d_ekf_camera_optimizer_state.h      breg3d_ekf_camera_optimizer_state.cxx
      breg3d_homography_generator.h
      breg3d_lm_direct_homography_generator.h  breg3d_lm_direct_homography_generator.cxx
      breg3d_gdbicp_homography_generator.h     breg3d_gdbicp_homography_generator.cxx
     )

  #aux_source_directory(Templates breg3d_sources)

  vxl_add_library(LIBRARY_NAME breg3d LIBRARY_SOURCES ${breg3d_sources})

  target_link_libraries(breg3d bvxm ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ihog vsol brip brdb ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vul)

  add_subdirectory(pro)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
