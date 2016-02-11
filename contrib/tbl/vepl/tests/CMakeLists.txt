# tbl/vepl/tests/CMakeLists.txt

add_definitions(-DINSTANTIATE_TEMPLATES)

add_executable( vepl_test_all
 test_driver.cxx test_driver.h

 vepl_test_dilate_disk.cxx
 vepl_test_dyadic.cxx
 vepl_test_erode_disk.cxx
 vepl_test_gaussian_convolution.cxx
 vepl_test_gradient_dir.cxx
 vepl_test_gradient_mag.cxx
 vepl_test_median.cxx
 vepl_test_moment.cxx
 vepl_test_monadic.cxx
 vepl_test_sobel.cxx
 vepl_test_threshold.cxx
 vepl_test_x_gradient.cxx
 vepl_test_y_gradient.cxx
)
target_link_libraries( vepl_test_all vepl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib )

add_test( NAME vepl_test_dilate_disk COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_dilate_disk)
add_test( NAME vepl_test_dyadic COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_dyadic)
add_test( NAME vepl_test_erode_disk COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_erode_disk)
add_test( NAME vepl_test_gaussian_convolution COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_gaussian_convolution)
add_test( NAME vepl_test_gradient_dir COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_gradient_dir)
add_test( NAME vepl_test_gradient_mag COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_gradient_mag)
add_test( NAME vepl_test_median COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_median)
add_test( NAME vepl_test_moment COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_moment)
add_test( NAME vepl_test_monadic COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_monadic)
add_test( NAME vepl_test_sobel COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_sobel)
add_test( NAME vepl_test_threshold COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_threshold)
add_test( NAME vepl_test_x_gradient COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_x_gradient)
add_test( NAME vepl_test_y_gradient COMMAND $<TARGET_FILE:vepl_test_all> vepl_test_y_gradient)

add_executable( vepl_test_include test_include.cxx )
target_link_libraries( vepl_test_include vepl )
add_executable( vepl_test_template_include test_template_include.cxx )
target_link_libraries( vepl_test_template_include vepl )
