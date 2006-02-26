# tbl/vepl2/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vepl2_test_all
 test_driver.cxx test_driver.h

 vepl2_test_dilate_disk.cxx
 vepl2_test_dyadic.cxx
 vepl2_test_erode_disk.cxx
 vepl2_test_gaussian_convolution.cxx
 vepl2_test_gradient_dir.cxx
 vepl2_test_gradient_mag.cxx
 vepl2_test_median.cxx
 vepl2_test_moment.cxx
 vepl2_test_monadic.cxx
 vepl2_test_sobel.cxx
 vepl2_test_threshold.cxx
 vepl2_test_x_gradient.cxx
 vepl2_test_y_gradient.cxx
)
TARGET_LINK_LIBRARIES( vepl2_test_all vepl2 vil testlib )

ADD_TEST(vepl2_test_dilate_disk          vepl2_test_all vepl2_test_dilate_disk)
ADD_TEST(vepl2_test_dyadic               vepl2_test_all vepl2_test_dyadic)
ADD_TEST(vepl2_test_erode_disk           vepl2_test_all vepl2_test_erode_disk)
ADD_TEST(vepl2_test_gaussian_convolution vepl2_test_all vepl2_test_gaussian_convolution)
ADD_TEST(vepl2_test_gradient_dir         vepl2_test_all vepl2_test_gradient_dir)
ADD_TEST(vepl2_test_gradient_mag         vepl2_test_all vepl2_test_gradient_mag)
ADD_TEST(vepl2_test_median               vepl2_test_all vepl2_test_median)
ADD_TEST(vepl2_test_moment               vepl2_test_all vepl2_test_moment)
ADD_TEST(vepl2_test_monadic              vepl2_test_all vepl2_test_monadic)
ADD_TEST(vepl2_test_sobel                vepl2_test_all vepl2_test_sobel)
ADD_TEST(vepl2_test_threshold            vepl2_test_all vepl2_test_threshold)
ADD_TEST(vepl2_test_x_gradient           vepl2_test_all vepl2_test_x_gradient)
ADD_TEST(vepl2_test_y_gradient           vepl2_test_all vepl2_test_y_gradient)

ADD_EXECUTABLE( vepl2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vepl2_test_include vepl2 )
