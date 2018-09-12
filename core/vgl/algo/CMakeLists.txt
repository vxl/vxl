# vxl/vgl/algo/CMakeLists.txt

project(vgl_algo)


set( vgl_algo_sources
  vgl_algo_fwd.h
  vgl_rtree.hxx                            vgl_rtree.h
  vgl_orient_box_3d.hxx                    vgl_orient_box_3d.h
#  vgl_ellipsoid_3d.hxx                     vgl_ellipsoid_3d.h
  vgl_homg_operators_1d.hxx                vgl_homg_operators_1d.h
  vgl_homg_operators_2d.hxx                vgl_homg_operators_2d.h
  vgl_homg_operators_3d.hxx                vgl_homg_operators_3d.h
  vgl_h_matrix_1d.hxx                      vgl_h_matrix_1d.h
  vgl_h_matrix_2d.hxx                      vgl_h_matrix_2d.h
  vgl_h_matrix_3d.hxx                      vgl_h_matrix_3d.h
  vgl_p_matrix.hxx                         vgl_p_matrix.h
  vgl_norm_trans_2d.hxx                    vgl_norm_trans_2d.h
  vgl_norm_trans_3d.hxx                    vgl_norm_trans_3d.h
  vgl_compute_similarity_3d.hxx            vgl_compute_similarity_3d.h
  vgl_compute_rigid_3d.hxx            vgl_compute_rigid_3d.h
                                           vgl_h_matrix_1d_compute.h
  vgl_h_matrix_1d_compute_linear.cxx       vgl_h_matrix_1d_compute_linear.h
  vgl_h_matrix_1d_compute_3point.cxx       vgl_h_matrix_1d_compute_3point.h
  vgl_h_matrix_1d_compute_optimize.cxx     vgl_h_matrix_1d_compute_optimize.h
                                           vgl_h_matrix_2d_compute.h
  vgl_h_matrix_2d_compute_linear.cxx       vgl_h_matrix_2d_compute_linear.h
  vgl_h_matrix_2d_compute_4point.cxx       vgl_h_matrix_2d_compute_4point.h
  vgl_h_matrix_2d_compute_rigid_body.cxx   vgl_h_matrix_2d_compute_rigid_body.h
  vgl_line_2d_regression.hxx               vgl_line_2d_regression.h
  vgl_fit_lines_2d.hxx                     vgl_fit_lines_2d.h
  vgl_convex_hull_2d.hxx                   vgl_convex_hull_2d.h
                                           vgl_h_matrix_2d_optimize.h
  vgl_h_matrix_2d_optimize_lmq.cxx         vgl_h_matrix_2d_optimize_lmq.h
  vgl_conic_2d_regression.hxx              vgl_conic_2d_regression.h
  vgl_fit_conics_2d.hxx                    vgl_fit_conics_2d.h
  vgl_fit_plane_3d.hxx                     vgl_fit_plane_3d.h
  vgl_fit_sphere_3d.hxx                    vgl_fit_sphere_3d.h
  vgl_intersection.hxx                     vgl_intersection.h
                                           vgl_rotation_3d.h
                                           vgl_rtree_c.h
  vgl_orient_box_3d_operators.hxx          vgl_orient_box_3d_operators.h
                                           vgl_h_matrix_3d_compute.h
  vgl_h_matrix_3d_compute_linear.cxx       vgl_h_matrix_3d_compute_linear.h
  vgl_h_matrix_3d_compute_affine.cxx       vgl_h_matrix_3d_compute_affine.h
)
aux_source_directory(Templates vgl_algo_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vgl_algo LIBRARY_SOURCES ${vgl_algo_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vgl_algo-obj OBJECT ${vgl_algo_sources})
endif()
