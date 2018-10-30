# tbl/vepl/CMakeLists.txt

doxygen_add_library(contrib/tbl/vepl
  DEPENDS contrib/tbl/vipl core/vil
  PACKAGE contrib/tbl
  DESCRIPTION "Easy Image Processing Library using vil"
  )

include_directories(${MUL_INCLUDE_DIR})

set(vepl_sources
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

 vepl_convert.hxx              vepl_convert.h

 accessors/vipl_accessors_vil_image_view_base.h
 accessors/vipl_accessors_vil_image_view_base.hxx
 section/vipl_filterable_section_container_generator_vil_image_view_base.hxx
)

add_definitions(-DINSTANTIATE_TEMPLATES)
# [only when INSTANTIATE_TEMPLATES not defined:]
# aux_source_directory(Templates vepl_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(vepl_gaussian_convolution.cxx PROPERTIES COMPILE_FLAGS -O0)
  set_source_files_properties(vepl_threshold.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_add_random_noise.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_dyadic.cxx PROPERTIES COMPILE_FLAGS -O0)
  set_source_files_properties(vepl_dilate_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_erode_disk.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_median.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_moment.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(vepl_histogram.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(Templates/vepl_convert+uchar-.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(Templates/vepl_convert+short-.cxx PROPERTIES COMPILE_FLAGS -O1)
  set_source_files_properties(Templates/vepl_convert+float-.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()

vxl_add_library(LIBRARY_NAME vepl LIBRARY_SOURCES ${vepl_sources})
target_link_libraries(vepl vipl ${VXL_LIB_PREFIX}vil ${CMAKE_THREAD_LIBS})

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
