# mul/mil3d/CMakeLists.txt

SET(mil3d_sources
 mil3d_image_3d.h
 mil3d_transform_3d.cxx                mil3d_transform_3d.h
 mil3d_gauss_reduce_3d.cxx             mil3d_gauss_reduce_3d.h
 mil3d_image_3d_of.txx                 mil3d_image_3d_of.h
 mil3d_sample_profile_3d.txx           mil3d_sample_profile_3d.h
 mil3d_gaussian_pyramid_builder_3d.txx mil3d_gaussian_pyramid_builder_3d.h
 mil3d_trilin_interp_3d.h
 mil3d_sample_grid_3d.txx mil3d_sample_grid_3d.h
)

AUX_SOURCE_DIRECTORY(Templates mil3d_sources)

ADD_LIBRARY(mil3d ${mil3d_sources})
TARGET_LINK_LIBRARIES(mil3d mil vbl_io vnl_io vil1 vgl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
