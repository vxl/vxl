#
# ./mul/mil/CMakeLists.txt
#
#   Ian Scott
#

ADD_LIBRARY(mil mil_sources )

SOURCE_FILES(mil_sources
mil_gauss_reduce_2d.cxx
mil_gaussian_pyramid_builder_2d.cxx
mil_gaussian_pyramid_builder_2d_general.cxx
mil_image.cxx
mil_image_2d_of.cxx
mil_image_data.cxx
mil_image_io.cxx
mil_image_pyramid.cxx
mil_image_pyramid_builder.cxx
mil_transform_2d.cxx
mil_byte_image_2d_io.cxx
mil_convert_image_2d.cxx
)

AUX_SOURCE_DIRECTORY(Templates mil_sources )

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
