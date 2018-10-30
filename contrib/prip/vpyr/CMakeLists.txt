# prip/vpyr/CMakeLists.txt

set(vpyr_sources

    # 2-pyramid

   vpyr_2_pyramid.hxx            vpyr_2_pyramid.h
   vpyr_2_pyramid_base.cxx       vpyr_2_pyramid_base.h

   vpyr_2_pyramid_level_elts.h
   vpyr_2_pyramid_level.hxx      vpyr_2_pyramid_level.h

    # 2-topological pyramid

   vpyr_2_tpyramid.hxx           vpyr_2_tpyramid.h
   vpyr_2_tpyramid_base.cxx      vpyr_2_tpyramid_base.h

   vpyr_2_tpyramid_builder.h
   vpyr_2_tpyramid_level_elts.h
   vpyr_2_tpyramid_level.hxx     vpyr_2_tpyramid_level.h

   vpyr_top_tmap.hxx             vpyr_top_tmap.h
)

vxl_add_library(LIBRARY_NAME vpyr LIBRARY_SOURCES ${vpyr_sources})
target_link_libraries(vpyr vmap)

#if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples/simple_segmentation)
#endif()

#if(BUILD_TESTING)
  add_subdirectory(tests)
#endif()
