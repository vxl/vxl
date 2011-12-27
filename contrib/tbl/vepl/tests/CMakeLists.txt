# tbl/vepl/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vepl_test_all
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
TARGET_LINK_LIBRARIES( vepl_test_all vepl vil testlib )

ADD_TEST(vepl_test_dilate_disk          ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_dilate_disk)
ADD_TEST(vepl_test_dyadic               ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_dyadic)
ADD_TEST(vepl_test_erode_disk           ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_erode_disk)
ADD_TEST(vepl_test_gaussian_convolution ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_gaussian_convolution)
ADD_TEST(vepl_test_gradient_dir         ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_gradient_dir)
ADD_TEST(vepl_test_gradient_mag         ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_gradient_mag)
ADD_TEST(vepl_test_median               ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_median)
ADD_TEST(vepl_test_moment               ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_moment)
ADD_TEST(vepl_test_monadic              ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_monadic)
ADD_TEST(vepl_test_sobel                ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_sobel)
ADD_TEST(vepl_test_threshold            ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_threshold)
ADD_TEST(vepl_test_x_gradient           ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_x_gradient)
ADD_TEST(vepl_test_y_gradient           ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_y_gradient)

ADD_EXECUTABLE( vepl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vepl_test_include vepl )
ADD_EXECUTABLE( vepl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vepl_test_template_include vepl )
