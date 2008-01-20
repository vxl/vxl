# This is brl/bseg/bbgm/CMakeLists.txt
# Brown background modeling Library

SET(bbgm_sources

  bbgm_planes_to_sample.h
  bbgm_update.h
  bbgm_apply.h
  bbgm_detect.h
  bbgm_image_of.h      bbgm_image_of.cxx   bbgm_image_of.txx  bbgm_image_sptr.h
  bbgm_viewer.h        bbgm_viewer.cxx     bbgm_viewer_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates bbgm_sources)

ADD_LIBRARY(bbgm  ${bbgm_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES(bbgm bsta bsta_io bsta_algo vbl_io vsl vnl vil vbl)


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
