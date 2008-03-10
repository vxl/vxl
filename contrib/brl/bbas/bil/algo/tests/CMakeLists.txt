# contrib/brl/bbas/bil/algo/tests/CMakeLists.txt


ADD_EXECUTABLE( bil_algo_test_all
  test_driver.cxx
#  test_bil_scale_image.cxx
  test_bil_wshed2d.cxx
  test_bil_edt.cxx
  test_bil_cedt.cxx
  test_bil_finite_differences.cxx
  test_bil_finite_second_differences.cxx
  test_bil_detect_ridges.cxx
)

TARGET_LINK_LIBRARIES( bil_algo_test_all bil_algo vil_algo testlib )
ADD_TEST( bil_algo_test_bil_scale_image   bil_algo_test_all test_bil_scale_image )
ADD_TEST( bil_algo_test_bil_wshed2d       bil_algo_test_all test_bil_wshed2d )
ADD_TEST( bil_algo_test_bil_edt           bil_algo_test_all test_bil_edt )
ADD_TEST( bil_algo_test_bil_cedt          bil_algo_test_all test_bil_cedt )
ADD_TEST( bil_algo_test_bil_finite_differences          bil_algo_test_all test_bil_finite_differences )
ADD_TEST( bil_algo_test_bil_finite_second_differences          bil_algo_test_all test_bil_finite_second_differences )
ADD_TEST( bil_algo_test_detect_ridges          bil_algo_test_all test_bil_detect_ridges  ${CMAKE_CURRENT_SOURCE_DIR}/c.20.tif )
ADD_EXECUTABLE( bil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bil_algo_test_include bil_algo  )
