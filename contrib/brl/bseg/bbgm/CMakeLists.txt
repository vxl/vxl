# This is brl/bseg/bbgm/CMakeLists.txt
# Brown background modeling Library

doxygen_add_library(contrib/brl/bseg/bbgm
  DEPENDS contrib/brl/bbas/bsta core/vil core/vsl
  PACKAGE contrib/brl
  )

SET(bbgm_sources

  bbgm_planes_to_sample.h
  bbgm_update.h
  bbgm_apply.h
  bbgm_detect.h
  bbgm_image_of.h         bbgm_image_of.cxx      bbgm_image_of.txx  bbgm_image_sptr.h
  bbgm_viewer.h           bbgm_viewer.cxx        bbgm_viewer_sptr.h
  bbgm_view_maker.h                              bbgm_view_maker_sptr.h
  bbgm_loader.h           bbgm_loader.cxx
  bbgm_measure.h
  bbgm_feature_image.h    bbgm_feature_image.cxx bbgm_feature_image.txx
  bbgm_feature_image_sptr.h
  bbgm_features.h         bbgm_features.cxx
)

AUX_SOURCE_DIRECTORY(Templates bbgm_sources)

ADD_LIBRARY(bbgm  ${bbgm_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES(bbgm bsta bsta_algo brip vnl_io vbl_io vsl vnl vil_algo vil vgl_algo vgl vbl)

SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
