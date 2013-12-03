# temporary include
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

ADD_EXECUTABLE( vpgl_algo_test_all
  test_driver.cxx
  test_optimize_camera.cxx
  test_fm_compute.cxx
  test_camera_compute.cxx
  test_camera_convert.cxx
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

TARGET_LINK_LIBRARIES( vpgl_algo_test_all vpgl_algo vgl_algo vgl vnl_algo vnl vil vul testlib )

ADD_TEST( vpgl_algo_test_camera_compute              ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_compute )
ADD_TEST( vpgl_algo_test_camera_convert              ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_convert )
ADD_TEST( vpgl_algo_test_optimize_camera             ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_optimize_camera )
ADD_TEST( vpgl_algo_test_fm_compute                  ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_fm_compute )
ADD_TEST( vpgl_algo_test_rational_adjust_onept ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_rational_adjust_onept )
ADD_TEST( vpgl_algo_test_rational_adjust         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_rational_adjust )
ADD_TEST( vpgl_algo_test_backproject                 ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_backproject )
ADD_TEST( vpgl_algo_test_ray                         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ray )
ADD_TEST( vpgl_algo_test_ray_intersect                         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ray_intersect )
ADD_TEST( vpgl_algo_test_ortho_procrustes            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ortho_procrustes )
ADD_TEST( vpgl_algo_test_em_compute_5_point          ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_em_compute_5_point )
ADD_TEST( vpgl_algo_test_triangulate          ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_triangulate )
ADD_TEST( vpgl_algo_test_bundle_adjust          ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_bundle_adjust )
ADD_TEST( vpgl_algo_test_ba_fixed_k_lsqr             ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ba_fixed_k_lsqr )
ADD_TEST( vpgl_algo_test_ba_shared_k_lsqr            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ba_shared_k_lsqr )
ADD_TEST( vpgl_algo_test_affine_rect            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_affine_rect )

ADD_EXECUTABLE( vpgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_algo_test_include vpgl_algo )
ADD_EXECUTABLE( vpgl_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_algo_test_template_include vpgl_algo )
