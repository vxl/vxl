# tbl/vipl/CMakeLists.txt

SET(vipl_sources
 filter/vipl_filter_abs.cxx  filter/vipl_filter_abs.h
                             filter/vipl_filter_helper.h
 filter/vipl_filter.txx      filter/vipl_filter.h
 filter/vipl_filter_2d.txx   filter/vipl_filter_2d.h
                             filter/vipl_trivial_pixeliter.h

 vipl_add_random_noise.txx     vipl_add_random_noise.h
 vipl_convert.txx              vipl_convert.h
 vipl_dilate_disk.txx          vipl_dilate_disk.h
 vipl_dyadic.txx               vipl_dyadic.h

 vipl_erode_disk.txx           vipl_erode_disk.h
 vipl_gaussian_convolution.txx vipl_gaussian_convolution.h
 vipl_gradient_dir.txx         vipl_gradient_dir.h
 vipl_gradient_mag.txx         vipl_gradient_mag.h
 vipl_histogram.txx            vipl_histogram.h
 vipl_median.txx               vipl_median.h
 vipl_moment.txx               vipl_moment.h
 vipl_monadic.txx              vipl_monadic.h
 vipl_sobel.txx                vipl_sobel.h
 vipl_threshold.txx            vipl_threshold.h
 vipl_x_gradient.txx           vipl_x_gradient.h
 vipl_y_gradient.txx           vipl_y_gradient.h

 section/vipl_section_container.txx      section/vipl_section_container.h
 section/vipl_section_descriptor.txx     section/vipl_section_descriptor.h
 section/vipl_section_descriptor_2d.txx  section/vipl_section_descriptor_2d.h
 section/vipl_section_iterator.txx       section/vipl_section_iterator.h
 section/vipl_filterable_section_container_generator_vil1_image.txx
 section/vipl_filterable_section_container_generator_vcl_vector.txx
 accessors/vipl_accessors_vil1_image.txx  accessors/vipl_accessors_vil1_image.h
 accessors/vipl_accessors_vcl_vector.txx accessors/vipl_accessors_vcl_vector.h

 vipl_with_section/vipl_filterable_section_container_generator_section.txx
 vipl_with_section/accessors/vipl_accessors_section.txx
      vipl_with_section/accessors/vipl_accessors_section.h
 vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.txx
 vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h
 vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_vector.txx
 vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h
 vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.txx
 vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.txx
      vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h
)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

AUX_SOURCE_DIRECTORY(Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_vbl_array_2d/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_vnl_matrix/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_section/Templates vipl_sources)
# AUX_SOURCE_DIRECTORY(vipl_with_mil/Templates vipl_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
 SET_SOURCE_FILES_PROPERTIES(
  vipl_with_section/Templates/vipl_filter_2d+section+vil1_rgb+uchar-.2-.section+vil1_rgb+uchar-.2-.vil1_rgb+uchar-.vil1_rgb+uchar--.cxx
   PROPERTIES
   COMPILE_FLAGS -O1
 )
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# and for the latter, we need mul:
# INCLUDE_DIRECTORIES(${MUL_INCLUDE_DIR})

ADD_LIBRARY(vipl ${vipl_sources})
TARGET_LINK_LIBRARIES(vipl vil1 vnl)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
