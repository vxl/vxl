# tbl/vepl2/CMakeLists.txt

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/mul)

SET(vepl2_sources
 vepl2_threshold.cxx            vepl2_threshold.h
 vepl2_add_random_noise.cxx     vepl2_add_random_noise.h
 vepl2_monadic.cxx              vepl2_monadic.h
 vepl2_dyadic.cxx               vepl2_dyadic.h
 vepl2_erode_disk.cxx           vepl2_erode_disk.h
 vepl2_dilate_disk.cxx          vepl2_dilate_disk.h
 vepl2_sobel.cxx                vepl2_sobel.h
 vepl2_gaussian_convolution.cxx vepl2_gaussian_convolution.h
 vepl2_x_gradient.cxx           vepl2_x_gradient.h
 vepl2_y_gradient.cxx           vepl2_y_gradient.h
 vepl2_gradient_mag.cxx         vepl2_gradient_mag.h
 vepl2_gradient_dir.cxx         vepl2_gradient_dir.h
 vepl2_median.cxx               vepl2_median.h
 vepl2_moment.cxx               vepl2_moment.h
 vepl2_histogram.cxx            vepl2_histogram.h

 vepl2_convert.txx              vepl2_convert.h
)

AUX_SOURCE_DIRECTORY(Templates vepl2_sources)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_LIBRARY(vepl2 ${vepl2_sources})
TARGET_LINK_LIBRARIES(vepl2 vipl)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
