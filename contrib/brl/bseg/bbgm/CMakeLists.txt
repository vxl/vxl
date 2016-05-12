# This is brl/bseg/bbgm/CMakeLists.txt
# Brown background modeling Library

doxygen_add_library(contrib/brl/bseg/bbgm
  DEPENDS contrib/brl/bbas/bsta core/vil core/vsl
  PACKAGE contrib/brl
  )

set(bbgm_sources

  bbgm_planes_to_sample.h
  bbgm_update.h
  bbgm_apply.h
  bbgm_detect.h
  bbgm_image_of.h         bbgm_image_of.cxx      bbgm_image_of.hxx  bbgm_image_sptr.h
  bbgm_viewer.h           bbgm_viewer.cxx        bbgm_viewer_sptr.h
  bbgm_view_maker.h                              bbgm_view_maker_sptr.h
  bbgm_loader.h           bbgm_loader.cxx
  bbgm_measure.h
  bbgm_feature_image.h    bbgm_feature_image.cxx bbgm_feature_image.hxx
  bbgm_feature_image_sptr.h
  bbgm_features.h         bbgm_features.cxx
)

aux_source_directory(Templates bbgm_sources)

vxl_add_library(LIBRARY_NAME bbgm LIBRARY_SOURCES  ${bbgm_sources})

# add the required libraries into this list
target_link_libraries(bbgm bsta bsta_algo brip ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl)

add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
