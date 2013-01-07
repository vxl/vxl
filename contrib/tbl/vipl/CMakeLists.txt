# tbl/vipl/CMakeLists.txt

doxygen_add_library(contrib/tbl/vipl
  PACKAGE contrib/tbl
  DESCRIPTION "Templated Image Processing Library"
  )

SET(vipl_sources
 # A. Point operators:
 vipl_convert.txx              vipl_convert.h
 vipl_threshold.txx            vipl_threshold.h
 vipl_add_random_noise.txx     vipl_add_random_noise.h
 vipl_monadic.txx              vipl_monadic.h
 vipl_dyadic.txx               vipl_dyadic.h

 # B. Convolutions:
 vipl_erode_disk.txx           vipl_erode_disk.h
 vipl_dilate_disk.txx          vipl_dilate_disk.h
 vipl_sobel.txx                vipl_sobel.h
 vipl_gaussian_convolution.txx vipl_gaussian_convolution.h
 vipl_x_gradient.txx           vipl_x_gradient.h
 vipl_y_gradient.txx           vipl_y_gradient.h
 vipl_gradient_mag.txx         vipl_gradient_mag.h
 vipl_gradient_dir.txx         vipl_gradient_dir.h
 vipl_median.txx               vipl_median.h

 # C. Moments:
 vipl_moment.txx               vipl_moment.h

 # Z. Miscellaneous:
 vipl_histogram.txx            vipl_histogram.h

 # Support classes
 filter/vipl_filter_abs.cxx  filter/vipl_filter_abs.h
                             filter/vipl_filter_helper.h
 filter/vipl_filter.txx      filter/vipl_filter.h
 filter/vipl_filter_2d.txx   filter/vipl_filter_2d.h
                             filter/vipl_trivial_pixeliter.h

 section/vipl_section_container.txx          section/vipl_section_container.h
 section/vipl_section_descriptor.txx         section/vipl_section_descriptor.h
 section/vipl_section_descriptor_2d.txx      section/vipl_section_descriptor_2d.h
 section/vipl_section_iterator.txx           section/vipl_section_iterator.h
 section/vipl_filterable_section_container_generator_vcl_vector.txx

 # Image accessors (getpixel, setpixel)
 accessors/vipl_accessors_vcl_vector.txx     accessors/vipl_accessors_vcl_vector.h
 vipl_with_section/vipl_filterable_section_container_generator_section.txx
 vipl_with_section/accessors/vipl_accessors_section.txx  vipl_with_section/accessors/vipl_accessors_section.h
)
IF (BUILD_CORE_IMAGING)
  SET(vipl_sources ${vipl_sources}
      section/vipl_filterable_section_container_generator_vil_image_view.txx
      accessors/vipl_accessors_vil_image_view.txx accessors/vipl_accessors_vil_image_view.h
     )
ENDIF (BUILD_CORE_IMAGING)
IF (BUILD_CORE_UTILITIES)
  SET(vipl_sources ${vipl_sources}
      vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.txx
      vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.txx vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h
     )
ENDIF (BUILD_CORE_UTILITIES)
IF (BUILD_CORE_NUMERICS)
  SET(vipl_sources ${vipl_sources}
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.txx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_vector.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.txx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h
     )
ENDIF (BUILD_CORE_NUMERICS)
IF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)
  SET(vipl_sources ${vipl_sources}
      section/vipl_filterable_section_container_generator_vil1_image.txx
      accessors/vipl_accessors_vil1_image.txx     accessors/vipl_accessors_vil1_image.h
     )
ENDIF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)
# [only when INSTANTIATE_TEMPLATES not defined:]
# AUX_SOURCE_DIRECTORY(Templates vipl_sources)
# AUX_SOURCE_DIRECTORY(vipl_with_vbl_array_2d/Templates vipl_sources)
# AUX_SOURCE_DIRECTORY(vipl_with_vnl_matrix/Templates vipl_sources)
# AUX_SOURCE_DIRECTORY(vipl_with_section/Templates vipl_sources)

ADD_LIBRARY(vipl ${vipl_sources})
INSTALL_TARGETS(/lib vipl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/tbl/vipl ${vipl_sources})

# No unconditional library dependencies:
IF (BUILD_CORE_IMAGING)
  TARGET_LINK_LIBRARIES(vipl vil)
ENDIF (BUILD_CORE_IMAGING)
IF (BUILD_CORE_UTILITIES)
  TARGET_LINK_LIBRARIES(vipl vbl)
ENDIF (BUILD_CORE_UTILITIES)
IF (BUILD_CORE_NUMERICS)
  TARGET_LINK_LIBRARIES(vipl vnl)
ENDIF (BUILD_CORE_NUMERICS)
IF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)
  TARGET_LINK_LIBRARIES(vipl vil1)
ENDIF (BUILD_CORE_IMAGING AND NOT BUILD_NONDEPRECATED_ONLY)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
