# mul/mil/CMakeLists.txt

SET(mil_sources
    mil_all.h                       mil_add_all_loaders.cxx

    mil_gauss_reduce_2d.cxx         mil_gauss_reduce_2d.h
    mil_image.cxx                   mil_image.h
    mil_image_io.cxx                mil_image_io.h
    mil_image_pyramid_builder.cxx   mil_image_pyramid_builder.h
    mil_transform_2d.cxx            mil_transform_2d.h
    mil_byte_image_2d_io.cxx        mil_byte_image_2d_io.h
    mil_byte_image_2d_io_plugin.cxx mil_byte_image_2d_io_plugin.h
    mil_convert_image_2d.cxx        mil_convert_image_2d.h
    mil_normalise_image_2d.cxx      mil_normalise_image_2d.h
    mil_invert_image_2d.cxx         mil_invert_image_2d.h

    mil_image_2d.h
    mil_image_2d_of.txx             mil_image_2d_of.h
    mil_image_data.txx              mil_image_data.h
    mil_image_pyramid.txx           mil_image_pyramid.h        mil_image_pyramid.cxx
    mil_gaussian_pyramid_builder_2d_general.txx mil_gaussian_pyramid_builder_2d_general.h
    mil_gaussian_pyramid_builder_2d.txx         mil_gaussian_pyramid_builder_2d.h
    mil_scale_pyramid_builder_2d.txx            mil_scale_pyramid_builder_2d.h
    mil_sample_profile_2d.txx       mil_sample_profile_2d.h
    mil_sample_grid_2d.txx          mil_sample_grid_2d.h

    mil_bilin_interp_2d.h
    mil_wrappers.h
    mil_convert_vil.h
)
AUX_SOURCE_DIRECTORY(Templates mil_sources)

ADD_LIBRARY(mil ${mil_sources})
TARGET_LINK_LIBRARIES(mil mbl vnl vbl_io vgl vil1)

SUBDIRS(algo)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
