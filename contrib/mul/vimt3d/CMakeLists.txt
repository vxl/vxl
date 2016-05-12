# mul/vimt3d/CMakeLists.txt

doxygen_add_library(contrib/mul/vimt3d
  DEPENDS contrib/mul/vil3d contrib/mul/vimt core/vnl
  PACKAGE contrib/mul
  DESCRIPTION "Polymorphic 3D image + transform library"
 )

set(vimt3d_sources
    vimt3d_transform_3d.h                 vimt3d_transform_3d.cxx
    vimt3d_image_3d_of.h                  vimt3d_image_3d_of.hxx
    vimt3d_image_3d.h                     vimt3d_image_3d.cxx
    vimt3d_sample_grid_trilin.h           vimt3d_sample_grid_trilin.hxx
    vimt3d_reconstruct_from_grid.h        vimt3d_reconstruct_from_grid.hxx
    vimt3d_sample_profile_trilin.h
    vimt3d_trilin_interp.h
    vimt3d_slice.h
    vimt3d_load.h                         vimt3d_load.cxx
    vimt3d_save.h                         vimt3d_save.cxx
    vimt3d_reflect.h
    vimt3d_gaussian_pyramid_builder_3d.h  vimt3d_gaussian_pyramid_builder_3d.hxx
    vimt3d_crop.h
    vimt3d_vil3d_v3i.h                    vimt3d_vil3d_v3i.cxx
    vimt3d_vil3d_v3m.cxx                  vimt3d_vil3d_v3m.h
    vimt3d_add_all_loaders.h              vimt3d_add_all_loaders.cxx
    vimt3d_from_image_2d.h
    vimt3d_fwd.h
    vimt3d_resample_trilinear.h
    vimt3d_gauss_reduce.h
    vimt3d_resample_tricubic.h
    vimt3d_sample_transformed_blob.h
   )

aux_source_directory(Templates vimt3d_sources)

vxl_add_library(LIBRARY_NAME vimt3d LIBRARY_SOURCES ${vimt3d_sources})
target_link_libraries(vimt3d vil3d_algo vil3d_io vil3d vimt mbl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(tools)
