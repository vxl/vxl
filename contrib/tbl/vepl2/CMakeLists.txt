# tbl/vepl2/CMakeLists.txt

INCLUDE_DIRECTORIES(${MUL_INCLUDE_DIR})

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

 accessors/vipl_accessors_vil_image_view_base.h
 accessors/vipl_accessors_vil_image_view_base.txx
 section/vipl_filterable_section_container_generator_vil_image_view_base.txx
)

AUX_SOURCE_DIRECTORY(Templates vepl2_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(vepl2_gaussian_convolution.cxx PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(vepl2_threshold.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_add_random_noise.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_dyadic.cxx PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(vepl2_dilate_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_erode_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_median.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_moment.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(vepl2_histogram.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl2_convert+uchar-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl2_convert+short-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl2_convert+float-.cxx PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(Templates/vepl2_convert+vil1_rgb+uchar--.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_LIBRARY(vepl2 ${vepl2_sources})
TARGET_LINK_LIBRARIES(vepl2 vipl vil ${CMAKE_THREAD_LIBS})

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
