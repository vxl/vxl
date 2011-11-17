# This is bseg/bvxm/breg3d/CMakeLists.txt
# 3-d video registration using the bvxm library
IF(BUILD_MUL)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

  SET(breg3d_sources
      breg3d_ekf_camera_optimizer.h            breg3d_ekf_camera_optimizer.cxx
      breg3d_ekf_camera_optimizer_state.h      breg3d_ekf_camera_optimizer_state.cxx
      breg3d_homography_generator.h
      breg3d_lm_direct_homography_generator.h  breg3d_lm_direct_homography_generator.cxx
      breg3d_gdbicp_homography_generator.h     breg3d_gdbicp_homography_generator.cxx
     )

  #AUX_SOURCE_DIRECTORY(Templates breg3d_sources)

  ADD_LIBRARY(breg3d ${breg3d_sources})

  TARGET_LINK_LIBRARIES(breg3d bvxm vpgl vpgl_algo ihog vsol brip brdb vil vnl vgl_algo vgl vnl_algo vnl_io vul)

  SUBDIRS(pro)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(BUILD_MUL)
