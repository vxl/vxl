# tbl/vipl/CMakeLists.txt

doxygen_add_library(contrib/tbl/vipl
  PACKAGE contrib/tbl
  DESCRIPTION "Templated Image Processing Library"
  )

set(vipl_sources
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
if(BUILD_CORE_IMAGING)
  set(vipl_sources ${vipl_sources}
      section/vipl_filterable_section_container_generator_vil_image_view.txx
      accessors/vipl_accessors_vil_image_view.txx accessors/vipl_accessors_vil_image_view.h
     )
endif()
if(BUILD_CORE_UTILITIES)
  set(vipl_sources ${vipl_sources}
      vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.txx
      vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.txx vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h
     )
endif()
if(BUILD_CORE_NUMERICS)
  set(vipl_sources ${vipl_sources}
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.txx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_vector.txx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.txx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h
     )
endif()

add_definitions(-DINSTANTIATE_TEMPLATES)

add_library(vipl ${vipl_sources})
install_targets(/lib vipl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/tbl/vipl ${vipl_sources})

# No unconditional library dependencies:
if(BUILD_CORE_IMAGING)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vil)
endif()
if(BUILD_CORE_UTILITIES)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vbl)
endif()
if(BUILD_CORE_NUMERICS)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vnl)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
