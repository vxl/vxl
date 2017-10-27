# contrib/brl/bbas/bil/algo/tests/CMakeLists.txt

add_executable( bil_algo_test_all
  test_driver.cxx
  test_bil_scale_image.cxx
  test_bil_wshed2d.cxx
  test_bil_edt.cxx
  test_bil_cedt.cxx
  test_bil_finite_differences.cxx
  test_bil_finite_second_differences.cxx
  test_bil_detect_ridges.cxx
  test_image_bounds_from_histogram.cxx
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/c.20.tif ${CMAKE_CURRENT_BINARY_DIR}/c.20.tif)

target_link_libraries( bil_algo_test_all bil_algo ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )
add_test( NAME bil_algo_test_bil_wshed2d COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_wshed2d )
add_test( NAME bil_algo_test_bil_scale_image COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_scale_image )
add_test( NAME bil_algo_test_bil_edt COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_edt )
add_test( NAME bil_algo_test_bil_cedt COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_cedt )
add_test( NAME bil_algo_test_bil_finite_differences COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_finite_differences )
add_test( NAME bil_algo_test_bil_finite_second_differences COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_finite_second_differences )
add_test( NAME bil_algo_test_detect_ridges COMMAND $<TARGET_FILE:bil_algo_test_all> test_bil_detect_ridges ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( NAME bil_test_image_bounds_from_histogram COMMAND $<TARGET_FILE:bil_algo_test_all> test_image_bounds_from_histogram ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( bil_algo_test_include test_include.cxx )
target_link_libraries( bil_algo_test_include bil_algo )
add_executable( bil_algo_test_template_include test_template_include.cxx )
target_link_libraries( bil_algo_test_template_include bil_algo )
