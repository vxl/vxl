# contrib/brl/bbas/bil/algo/tests/CMakeLists.txt

ADD_EXECUTABLE( bil_algo_test_all
  test_driver.cxx
  test_bil_scale_image.cxx
  test_bil_wshed2d.cxx
  test_bil_edt.cxx
  test_bil_cedt.cxx
  test_bil_finite_differences.cxx
  test_bil_finite_second_differences.cxx
  test_bil_detect_ridges.cxx

)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/c.20.tif ${CMAKE_CURRENT_BINARY_DIR}/c.20.tif)

TARGET_LINK_LIBRARIES( bil_algo_test_all bil_algo vil_algo vil vnl testlib )
ADD_TEST( bil_algo_test_bil_wshed2d       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_wshed2d )
ADD_TEST( bil_algo_test_bil_scale_image   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_scale_image )
ADD_TEST( bil_algo_test_bil_edt           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_edt )
ADD_TEST( bil_algo_test_bil_cedt          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_cedt )
ADD_TEST( bil_algo_test_bil_finite_differences          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_finite_differences )
ADD_TEST( bil_algo_test_bil_finite_second_differences   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_finite_second_differences )
ADD_TEST( bil_algo_test_detect_ridges     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bil_algo_test_all test_bil_detect_ridges ${CMAKE_CURRENT_SOURCE_DIR} )


ADD_EXECUTABLE( bil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bil_algo_test_include bil_algo )
ADD_EXECUTABLE( bil_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bil_algo_test_template_include bil_algo )
