# prip/vpyr/CMakeLists.txt

set(vpyr_sources

    # 2-pyramid

   vpyr_2_pyramid.txx            vpyr_2_pyramid.h
   vpyr_2_pyramid_base.cxx       vpyr_2_pyramid_base.h

   vpyr_2_pyramid_level_elts.h
   vpyr_2_pyramid_level.txx      vpyr_2_pyramid_level.h

    # 2-topological pyramid

   vpyr_2_tpyramid.txx           vpyr_2_tpyramid.h
   vpyr_2_tpyramid_base.cxx      vpyr_2_tpyramid_base.h

   vpyr_2_tpyramid_builder.h
   vpyr_2_tpyramid_level_elts.h
   vpyr_2_tpyramid_level.txx     vpyr_2_tpyramid_level.h

   vpyr_top_tmap.txx             vpyr_top_tmap.h
)

vxl_add_library( LIBNAME vpyr LIBSRCS ${vpyr_sources})
target_link_libraries(vpyr vmap)

#if(BUILD_EXAMPLES)
  add_subdirectory(examples/simple_segmentation)
#endif()

#if(BUILD_TESTING)
  add_subdirectory(tests)
#endif()
