# vxl/vgl/algo/CMakeLists.txt

SET( vgl_algo_sources
  vgl_algo_fwd.h
  vgl_rtree.txx                        vgl_rtree.h
  vgl_homg_operators_1d.txx            vgl_homg_operators_1d.h
  vgl_homg_operators_2d.txx            vgl_homg_operators_2d.h
  vgl_homg_operators_3d.txx            vgl_homg_operators_3d.h
  vgl_h_matrix_1d.txx                  vgl_h_matrix_1d.h
  vgl_h_matrix_2d.txx                  vgl_h_matrix_2d.h
  vgl_h_matrix_3d.txx                  vgl_h_matrix_3d.h
  vgl_p_matrix.txx                     vgl_p_matrix.h
  vgl_norm_trans_2d.txx                vgl_norm_trans_2d.h
  vgl_norm_trans_3d.txx                vgl_norm_trans_3d.h
                                       vgl_h_matrix_1d_compute.h
  vgl_h_matrix_1d_compute_linear.cxx   vgl_h_matrix_1d_compute_linear.h
  vgl_h_matrix_1d_compute_3point.cxx   vgl_h_matrix_1d_compute_3point.h
  vgl_h_matrix_1d_compute_optimize.cxx vgl_h_matrix_1d_compute_optimize.h
                                       vgl_h_matrix_2d_compute.h
  vgl_h_matrix_2d_compute_linear.cxx   vgl_h_matrix_2d_compute_linear.h
  vgl_h_matrix_2d_compute_4point.cxx   vgl_h_matrix_2d_compute_4point.h
  vgl_line_2d_regression.txx           vgl_line_2d_regression.h
  vgl_fit_lines_2d.txx                 vgl_fit_lines_2d.h
  vgl_convex_hull_2d.txx               vgl_convex_hull_2d.h
)
AUX_SOURCE_DIRECTORY(Templates vgl_algo_sources)

ADD_LIBRARY(vgl_algo ${vgl_algo_sources})
TARGET_LINK_LIBRARIES( vgl_algo vgl vnl_algo vnl )
