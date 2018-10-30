# mul/vimt/CMakeLists.txt

doxygen_add_library(contrib/mul/vimt
  DEPENDS core/vil core/vnl core/vgl
  PACKAGE contrib/mul
  DESCRIPTION "Polymorphic image + transform library"
  )

IF (VXL_BUILD_DCMTK)
  ADD_DEFINITIONS(-D_BUILD_DCMTK)
ENDIF (VXL_BUILD_DCMTK)

set(vimt_sources
    vimt_image.cxx                                vimt_image.h
    vimt_transform_2d.cxx                         vimt_transform_2d.h
    vimt_transform_util_2d.cxx                    vimt_transform_util_2d.h
    vimt_image_2d.h                               vimt_image_2d.cxx
    vimt_image_2d_of.hxx                          vimt_image_2d_of.h
    vimt_crop.h
    vimt_convert.h
    vimt_dog_pyramid_builder_2d.hxx               vimt_dog_pyramid_builder_2d.h
    vimt_image_pyramid.cxx vimt_image_pyramid.hxx vimt_image_pyramid.h
    vimt_image_pyramid_builder.cxx                vimt_image_pyramid_builder.h
    vimt_gaussian_pyramid_builder_2d.hxx          vimt_gaussian_pyramid_builder_2d.h
    vimt_gaussian_pyramid_builder_2d_general.hxx  vimt_gaussian_pyramid_builder_2d_general.h
    vimt_scale_pyramid_builder_2d.hxx             vimt_scale_pyramid_builder_2d.h
    vimt_sample_profile_bilin.hxx                 vimt_sample_profile_bilin.h
    vimt_sample_grid_bilin.hxx                    vimt_sample_grid_bilin.h
    vimt_sample_grid_bicub.hxx                    vimt_sample_grid_bicub.h
    vimt_bilin_interp.h
    vimt_reflect.h
    vimt_resample_bilin.h
    vimt_load.h                                   vimt_load.cxx
    vimt_save.h                                   vimt_save.cxx
    vimt_vil_v2i.h                                vimt_vil_v2i.cxx
    vimt_add_all_binary_loaders.h                 vimt_add_all_binary_loaders.cxx
    vimt_fwd.h
)
aux_source_directory(Templates vimt_sources)

vxl_add_library(LIBRARY_NAME vimt LIBRARY_SOURCES ${vimt_sources})
target_link_libraries(vimt mbl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil )

add_subdirectory(algo)

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vimt-obj OBJECT ${vimt_sources})
endif()
