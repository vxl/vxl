# prip/vpyr/CMakeLists.txt

SET(vpyr_sources

    # 2-pyramid

   vpyr_2_pyramid.txx            vpyr_2_pyramid.h
   vpyr_2_pyramid_base.cxx       vpyr_2_pyramid_base.h

   vpyr_2_pyramid_level_elts.cxx vpyr_2_pyramid_level_elts.h
   vpyr_2_pyramid_level.txx      vpyr_2_pyramid_level.h

    # 2-topological pyramid

   vpyr_2_tpyramid.txx           vpyr_2_tpyramid.h
   vpyr_2_tpyramid_base.cxx      vpyr_2_tpyramid_base.h

   vpyr_2_tpyramid_builder.h
   vpyr_2_tpyramid_level_elts.h
   vpyr_2_tpyramid_level.txx     vpyr_2_tpyramid_level.h

   vpyr_top_tmap.txx             vpyr_top_tmap.h
)

ADD_LIBRARY(vpyr ${vpyr_sources})

TARGET_LINK_LIBRARIES(vpyr vmap)

#IF (BUILD_EXAMPLES)
  SUBDIRS(examples/simple_segmentation)
#ENDIF (BUILD_EXAMPLES)

#IF (BUILD_TESTING)
  SUBDIRS(tests)
#ENDIF (BUILD_TESTING)
