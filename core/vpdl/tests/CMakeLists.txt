# core/vpdl/tests/CMakeLists.txt
# Tests for vpdl library

ADD_EXECUTABLE( vpdl_test_all
  test_driver.cxx
  test_eigen_sym_matrix.cxx
  test_gaussian_sphere.cxx
  test_gaussian_indep.cxx
  test_gaussian.cxx
)

TARGET_LINK_LIBRARIES( vpdl_test_all vpdl testlib)

ADD_TEST( vpdl_test_eigen_sym_matrix    vpdl_test_all test_eigen_sym_matrix )
ADD_TEST( vpdl_test_gaussian_sphere     vpdl_test_all test_gaussian_sphere )
ADD_TEST( vpdl_test_gaussian_indep      vpdl_test_all test_gaussian_indep )
ADD_TEST( vpdl_test_gaussian            vpdl_test_all test_gaussian )

#ADD_EXECUTABLE( vpdl_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( vpdl_test_include vpdl )
