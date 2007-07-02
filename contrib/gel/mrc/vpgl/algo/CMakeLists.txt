# gel/mrc/vpgl/algo/CMakeLists.txt
# Photogrammetry Algorithms Library.

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${RPL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR}/mrc)

SET( vpgl_algo_sources
  vpgl_construct_cameras.h         vpgl_construct_cameras.cxx
  vpgl_optimize_camera.h           vpgl_optimize_camera.cxx
  vpgl_project.h                   vpgl_project.cxx
  vpgl_camera_compute.h            vpgl_camera_compute.cxx
  vpgl_fm_compute_7_point.h        vpgl_fm_compute_7_point.cxx
  vpgl_fm_compute_8_point.h        vpgl_fm_compute_8_point.cxx
  vpgl_fm_compute_2_point.h        vpgl_fm_compute_2_point.cxx
  vpgl_fm_compute_ransac.h         vpgl_fm_compute_ransac.cxx
  vpgl_fm_compute_reg_ransac.h     vpgl_fm_compute_reg_ransac.cxx
  vpgl_fm_compute_affine_ransac.h  vpgl_fm_compute_affine_ransac.cxx
  vpgl_lens_warp_mapper.h
  vpgl_vsol_lens_warp.h            vpgl_vsol_lens_warp.cxx
  vpgl_list.h                      vpgl_list.txx
  vpgl_invmap_cost_function.h      vpgl_invmap_cost_function.cxx
  vpgl_backproject.h               vpgl_backproject.cxx
  vpgl_ortho_procrustes.h	   vpgl_ortho_procrustes.cxx
)

AUX_SOURCE_DIRECTORY(Templates vpgl_algo_sources)

ADD_LIBRARY(vpgl_algo ${vpgl_algo_sources})
TARGET_LINK_LIBRARIES( vpgl_algo vpgl rrel vgl_algo vsol vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
