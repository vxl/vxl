# temporary include
include_directories(${OXL_INCLUDE_DIR})
set(GEOTIFF_VALID FALSE)
include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
if(TIFF_FOUND)
  include_directories(${TIFF_INCLUDE_DIR})
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})
    set(GEOTIFF_VALID TRUE)
 endif()
endif()
if(NOT GEOTIFF_VALID)
add_executable( vpgl_algo_test_all
  test_driver.cxx
  test_optimize_camera.cxx
  test_fm_compute.cxx
  test_camera_compute.cxx
  test_camera_convert.cxx
  test_lens_warp_mapper.cxx
  test_backproject.cxx
  test_ray.cxx
  test_ray_intersect.cxx
  test_ortho_procrustes.cxx
  test_rational_adjust_onept.cxx
  test_rational_adjust.cxx
  test_em_compute_5_point.cxx
  test_triangulate.cxx
  test_ba_fixed_k_lsqr.cxx
  test_ba_shared_k_lsqr.cxx
  test_bundle_adjust.cxx
  test_affine_rectification.cxx
)
target_link_libraries( vpgl_algo_test_all ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )
endif()

if(GEOTIFF_VALID)
add_executable( vpgl_algo_test_all
  test_driver.cxx
  test_optimize_camera.cxx
  test_fm_compute.cxx
  test_camera_compute.cxx
  test_camera_convert.cxx
  test_lens_warp_mapper.cxx
  test_backproject.cxx
  test_ray.cxx
  test_ray_intersect.cxx
  test_ortho_procrustes.cxx
  test_rational_adjust_onept.cxx
  test_rational_adjust.cxx
  test_em_compute_5_point.cxx
  test_triangulate.cxx
  test_ba_fixed_k_lsqr.cxx
  test_ba_shared_k_lsqr.cxx
  test_bundle_adjust.cxx
  test_affine_rectification.cxx
  test_backproject_dem.cxx
)
target_link_libraries( vpgl_algo_test_all ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_file_formats ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )
add_test( NAME vpgl_algo_test_backproject_dem COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_backproject_dem )
endif()

add_test( NAME vpgl_algo_test_camera_compute COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_camera_compute )
add_test( NAME vpgl_algo_test_camera_convert COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_camera_convert )
add_test( NAME vpgl_algo_test_optimize_camera COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_optimize_camera )
add_test( NAME vpgl_algo_test_fm_compute COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_fm_compute )
add_test( NAME vpgl_algo_test_rational_adjust_onept COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_rational_adjust_onept )
add_test( NAME vpgl_algo_test_rational_adjust COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_rational_adjust )
add_test( NAME vpgl_algo_test_lens_warp_mapper COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_lens_warp_mapper )
add_test( NAME vpgl_algo_test_backproject COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_backproject )
add_test( NAME vpgl_algo_test_ray COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_ray )
add_test( NAME vpgl_algo_test_ray_intersect COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_ray_intersect )
add_test( NAME vpgl_algo_test_ortho_procrustes COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_ortho_procrustes )
add_test( NAME vpgl_algo_test_em_compute_5_point COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_em_compute_5_point )
add_test( NAME vpgl_algo_test_triangulate COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_triangulate )
add_test( NAME vpgl_algo_test_bundle_adjust COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_bundle_adjust )
add_test( NAME vpgl_algo_test_ba_fixed_k_lsqr COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_ba_fixed_k_lsqr )
add_test( NAME vpgl_algo_test_ba_shared_k_lsqr COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_ba_shared_k_lsqr )
add_test( NAME vpgl_algo_test_affine_rect COMMAND $<TARGET_FILE:vpgl_algo_test_all> test_affine_rect )

add_executable( vpgl_algo_test_include test_include.cxx )
target_link_libraries( vpgl_algo_test_include ${VXL_LIB_PREFIX}vpgl_algo )
add_executable( vpgl_algo_test_template_include test_template_include.cxx )
target_link_libraries( vpgl_algo_test_template_include ${VXL_LIB_PREFIX}vpgl_algo )
