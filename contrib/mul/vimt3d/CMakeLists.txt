# mul/vimt3d/CMakeLists.txt

SET(vimt3d_sources
  vimt3d_transform_3d.cxx                vimt3d_transform_3d.h
  vimt3d_image_3d_of.txx                 vimt3d_image_3d_of.h
  vimt3d_image_3d.cxx                    vimt3d_image_3d.h
  vimt3d_sample_grid_trilin.txx          vimt3d_sample_grid_trilin.h
  vimt3d_reconstruct_from_grid.txx       vimt3d_reconstruct_from_grid.h
                                         vimt3d_sample_profile_trilin.h
                                         vimt3d_trilin_interp.h
                                         vimt3d_slice.h
  vimt3d_load.cxx                        vimt3d_load.h

  vimt3d_gaussian_pyramid_builder_3d.txx vimt3d_gaussian_pyramid_builder_3d.h
                                         vimt3d_crop.h
  vimt3d_vil3d_v3i.cxx                   vimt3d_vil3d_v3i.h
  vimt3d_add_all_loaders.cxx             vimt3d_add_all_loaders.h
                                         vimt3d_from_image_2d.h
)
AUX_SOURCE_DIRECTORY(Templates vimt3d_sources)

ADD_LIBRARY(vimt3d ${vimt3d_sources})
TARGET_LINK_LIBRARIES(vimt3d vil3d vil3d_algo vimt vnl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
