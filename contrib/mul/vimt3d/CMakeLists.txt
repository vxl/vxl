# mul/vimt3d/CMakeLists.txt

SET(vimt3d_sources
    vimt3d_transform_3d.cxx vimt3d_transform_3d.h
    vimt3d_image_3d_of.txx vimt3d_image_3d_of.h
    vimt3d_image_3d.h vimt3d_image_3d.cxx
    vimt3d_sample_grid_trilin.h          vimt3d_sample_grid_trilin.txx
    vimt3d_gaussian_pyramid_builder_3d.h vimt3d_gaussian_pyramid_builder_3d.txx
    vimt3d_sample_profile_trilin.h
    vimt3d_trilin_interp.h
    vimt3d_slice.h
    vimt3d_load.h                        vimt3d_load.cxx
)
AUX_SOURCE_DIRECTORY(Templates vimt3d_sources)

ADD_LIBRARY(vimt3d ${vimt3d_sources})
TARGET_LINK_LIBRARIES(vimt3d vil3d vimt)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
