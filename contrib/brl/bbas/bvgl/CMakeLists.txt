# Brown basic geometry library
# brl/bbas/bvgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bvgl_sources
    bvgl_h_matrix_1d.h                   bvgl_h_matrix_1d.txx
    bvgl_h_matrix_2d.h                   bvgl_h_matrix_2d.txx
    bvgl_h_matrix_3d.h                   bvgl_h_matrix_3d.txx
    bvgl_norm_trans_2d.h                 bvgl_norm_trans_2d.txx
    bvgl_h_matrix_1d_compute.h           bvgl_h_matrix_1d_compute.cxx
    bvgl_h_matrix_1d_compute_linear.h    bvgl_h_matrix_1d_compute_linear.cxx
    bvgl_h_matrix_1d_compute_3point.h    bvgl_h_matrix_1d_compute_3point.cxx
    bvgl_h_matrix_1d_compute_optimize1.h bvgl_h_matrix_1d_compute_optimize1.cxx
    bvgl_h_matrix_2d_compute.h           bvgl_h_matrix_2d_compute.cxx
    bvgl_h_matrix_2d_compute_linear.h    bvgl_h_matrix_2d_compute_linear.cxx
    bvgl_h_matrix_2d_compute_4point.h    bvgl_h_matrix_2d_compute_4point.cxx
)
AUX_SOURCE_DIRECTORY(Templates bvgl_sources)

ADD_LIBRARY(bvgl ${bvgl_sources})

TARGET_LINK_LIBRARIES(bvgl vgl vnl_algo vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
