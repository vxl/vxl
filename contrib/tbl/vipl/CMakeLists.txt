# tbl/vipl/CMakeLists.txt

doxygen_add_library(contrib/tbl/vipl
  PACKAGE contrib/tbl
  DESCRIPTION "Templated Image Processing Library"
  )

set(vipl_sources
 # A. Point operators:
 vipl_convert.hxx              vipl_convert.h
 vipl_threshold.hxx            vipl_threshold.h
 vipl_add_random_noise.hxx     vipl_add_random_noise.h
 vipl_monadic.hxx              vipl_monadic.h
 vipl_dyadic.hxx               vipl_dyadic.h

 # B. Convolutions:
 vipl_erode_disk.hxx           vipl_erode_disk.h
 vipl_dilate_disk.hxx          vipl_dilate_disk.h
 vipl_sobel.hxx                vipl_sobel.h
 vipl_gaussian_convolution.hxx vipl_gaussian_convolution.h
 vipl_x_gradient.hxx           vipl_x_gradient.h
 vipl_y_gradient.hxx           vipl_y_gradient.h
 vipl_gradient_mag.hxx         vipl_gradient_mag.h
 vipl_gradient_dir.hxx         vipl_gradient_dir.h
 vipl_median.hxx               vipl_median.h

 # C. Moments:
 vipl_moment.hxx               vipl_moment.h

 # Z. Miscellaneous:
 vipl_histogram.hxx            vipl_histogram.h

 # Support classes
 filter/vipl_filter_abs.cxx  filter/vipl_filter_abs.h
                             filter/vipl_filter_helper.h
 filter/vipl_filter.hxx      filter/vipl_filter.h
 filter/vipl_filter_2d.hxx   filter/vipl_filter_2d.h
                             filter/vipl_trivial_pixeliter.h

 section/vipl_section_container.hxx          section/vipl_section_container.h
 section/vipl_section_descriptor.hxx         section/vipl_section_descriptor.h
 section/vipl_section_descriptor_2d.hxx      section/vipl_section_descriptor_2d.h
 section/vipl_section_iterator.hxx           section/vipl_section_iterator.h
 section/vipl_filterable_section_container_generator_vcl_vector.hxx

 # Image accessors (getpixel, setpixel)
 accessors/vipl_accessors_vcl_vector.h     accessors/vipl_accessors_vcl_vector.h
 vipl_with_section/vipl_filterable_section_container_generator_section.hxx
 vipl_with_section/accessors/vipl_accessors_section.hxx  vipl_with_section/accessors/vipl_accessors_section.h
)
if(VXL_BUILD_CORE_IMAGING)
  set(vipl_sources ${vipl_sources}
      section/vipl_filterable_section_container_generator_vil_image_view.hxx
      accessors/vipl_accessors_vil_image_view.hxx accessors/vipl_accessors_vil_image_view.h
     )
endif()
if(VXL_BUILD_CORE_UTILITIES)
  set(vipl_sources ${vipl_sources}
      vipl_with_vbl_array_2d/vipl_filterable_section_container_generator_vbl_array_2d.hxx
      vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.hxx vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h
     )
endif()
if(VXL_BUILD_CORE_NUMERICS)
  set(vipl_sources ${vipl_sources}
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_matrix.hxx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.hxx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h
      vipl_with_vnl_matrix/vipl_filterable_section_container_generator_vnl_vector.hxx
      vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.hxx     vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h
     )
endif()

add_definitions(-DINSTANTIATE_TEMPLATES)

vxl_add_library(LIBRARY_NAME vipl LIBRARY_SOURCES ${vipl_sources})

# No unconditional library dependencies:
if(VXL_BUILD_CORE_IMAGING)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vil)
endif()
if(VXL_BUILD_CORE_UTILITIES)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vbl)
endif()
if(VXL_BUILD_CORE_NUMERICS)
  target_link_libraries(vipl ${VXL_LIB_PREFIX}vnl)
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
