USES += contrib/tbl core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vepl2_test_threshold.cxx
TESTS += vepl2_test_erode_disk.cxx
TESTS += vepl2_test_dyadic.cxx
TESTS += vepl2_test_dilate_disk.cxx
TESTS += vepl2_test_gaussian_convolution.cxx
TESTS += vepl2_test_gradient_dir.cxx
TESTS += vepl2_test_gradient_mag.cxx
TESTS += vepl2_test_median.cxx
TESTS += vepl2_test_moment.cxx
TESTS += vepl2_test_monadic.cxx
TESTS += vepl2_test_sobel.cxx
TESTS += vepl2_test_x_gradient.cxx
TESTS += vepl2_test_y_gradient.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
