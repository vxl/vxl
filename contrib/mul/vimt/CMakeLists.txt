# mul/vimt/CMakeLists.txt

SET(vimt_sources
    vimt_image.cxx                                vimt_image.h
    vimt_transform_2d.cxx                         vimt_transform_2d.h
    vimt_image_2d.h                               vimt_image_2d.cxx
    vimt_image_2d_of.txx                          vimt_image_2d_of.h
    vimt_crop.h
    vimt_dog_pyramid_builder_2d.txx               vimt_dog_pyramid_builder_2d.h
    vimt_image_pyramid.cxx vimt_image_pyramid.txx vimt_image_pyramid.h
    vimt_image_pyramid_builder.cxx                vimt_image_pyramid_builder.h
    vimt_gaussian_pyramid_builder_2d.txx          vimt_gaussian_pyramid_builder_2d.h
    vimt_gaussian_pyramid_builder_2d_general.txx  vimt_gaussian_pyramid_builder_2d_general.h
    vimt_scale_pyramid_builder_2d.txx             vimt_scale_pyramid_builder_2d.h
    vimt_sample_profile_bilin.txx                 vimt_sample_profile_bilin.h
    vimt_sample_grid_bilin.txx                    vimt_sample_grid_bilin.h
    vimt_bilin_interp.h
    vimt_resample_bilin.h
    vimt_add_all_binary_loaders.h                 vimt_add_all_binary_loaders.cxx
)
AUX_SOURCE_DIRECTORY(Templates vimt_sources)

ADD_LIBRARY(vimt ${vimt_sources})
TARGET_LINK_LIBRARIES(vimt vil vil_algo vil_io vnl vgl)

SUBDIRS(algo)
SUBDIRS(examples)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
