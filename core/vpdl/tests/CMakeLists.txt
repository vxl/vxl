# core/vpdl/tests/CMakeLists.txt
# Tests for vpdl library

add_executable( vpdl_test_all
  test_driver.cxx
  test_eigen_sym_matrix.cxx
  test_gaussian_sphere.cxx
  test_gaussian_indep.cxx
  test_gaussian.cxx
  test_update_gaussian.cxx
  test_mixture.cxx
  test_mixture_of.cxx
  test_update_mog.cxx
  test_kernel_gaussian_sfbw.cxx
)

target_link_libraries( vpdl_test_all ${VXL_LIB_PREFIX}vpdl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib)

add_test( NAME vpdl_test_eigen_sym_matrix COMMAND $<TARGET_FILE:vpdl_test_all> test_eigen_sym_matrix )
add_test( NAME vpdl_test_gaussian_sphere COMMAND $<TARGET_FILE:vpdl_test_all> test_gaussian_sphere )
add_test( NAME vpdl_test_gaussian_indep COMMAND $<TARGET_FILE:vpdl_test_all> test_gaussian_indep )
add_test( NAME vpdl_test_gaussian COMMAND $<TARGET_FILE:vpdl_test_all> test_gaussian )
add_test( NAME vpdl_test_update_gaussian COMMAND $<TARGET_FILE:vpdl_test_all> test_update_gaussian )
add_test( NAME vpdl_test_mixture COMMAND $<TARGET_FILE:vpdl_test_all> test_mixture )
add_test( NAME vpdl_test_mixture_of COMMAND $<TARGET_FILE:vpdl_test_all> test_mixture_of )
add_test( NAME vpdl_test_update_mog COMMAND $<TARGET_FILE:vpdl_test_all> test_update_mog )
add_test( NAME vpdl_test_kernel_gaussian_sfbw COMMAND $<TARGET_FILE:vpdl_test_all> test_kernel_gaussian_sfbw )

add_executable( vpdl_test_include test_include.cxx )
target_link_libraries( vpdl_test_include ${VXL_LIB_PREFIX}vpdl )
add_executable( vpdl_test_template_include test_template_include.cxx )
target_link_libraries( vpdl_test_template_include ${VXL_LIB_PREFIX}vpdl )
