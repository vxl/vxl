#
# ./mul/mil/CMakeLists.txt
#
#   Ian Scott
#

ADD_LIBRARY(mil mil_sources )

SOURCE_FILES(mil_sources
mil_gauss_reduce_2d
mil_gaussian_pyramid_builder_2d
mil_image
mil_image_2d_of
mil_image_io
mil_image_pyramid
mil_image_pyramid_builder
mil_transform_2d
mil_byte_image_2d_io
mil_convert_image_2d.cxx
)

AUX_SOURCE_DIRECTORY(Templates mil_sources )

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
