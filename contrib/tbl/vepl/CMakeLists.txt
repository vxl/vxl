# tbl/vepl/CMakeLists.txt

SET(vepl_sources
 vepl_threshold.cxx            vepl_threshold.h
 vepl_add_random_noise.cxx     vepl_add_random_noise.h
 vepl_monadic.cxx              vepl_monadic.h
 vepl_dyadic.cxx               vepl_dyadic.h
 vepl_erode_disk.cxx           vepl_erode_disk.h
 vepl_dilate_disk.cxx          vepl_dilate_disk.h
 vepl_sobel.cxx                vepl_sobel.h
 vepl_gaussian_convolution.cxx vepl_gaussian_convolution.h
 vepl_x_gradient.cxx           vepl_x_gradient.h
 vepl_y_gradient.cxx           vepl_y_gradient.h
 vepl_gradient_mag.cxx         vepl_gradient_mag.h
 vepl_gradient_dir.cxx         vepl_gradient_dir.h
 vepl_median.cxx               vepl_median.h
 vepl_moment.cxx               vepl_moment.h
 vepl_histogram.cxx            vepl_histogram.h

 vepl_convert.txx              vepl_convert.h
)

AUX_SOURCE_DIRECTORY(Templates vepl_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(vepl_threshold.cxx PROPERTIES COMPILE_FLAGS -O2)
  SET_SOURCE_FILES_PROPERTIES(vepl_add_random_noise.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl_dyadic.cxx PROPERTIES COMPILE_FLAGS -O3)
  SET_SOURCE_FILES_PROPERTIES(vepl_dilate_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl_erode_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl_median.cxx PROPERTIES COMPILE_FLAGS -O3)
  SET_SOURCE_FILES_PROPERTIES(vepl_moment.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl_histogram.cxx PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl_convert+uchar-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl_convert+short-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl_convert+float-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl_convert+vil1_rgb+uchar--.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_LIBRARY(vepl ${vepl_sources})
TARGET_LINK_LIBRARIES(vepl vipl ${CMAKE_THREAD_LIBS})

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
