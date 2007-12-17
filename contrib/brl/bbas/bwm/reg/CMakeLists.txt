# world modeling application
# contrib/bwm/reg/CMakeLists.txt


SET(bwm_reg_sources
   bwm_reg_utils.cxx              bwm_reg_utils.h
   bwm_reg_edge_champher.cxx      bwm_reg_edge_champher.h
   bwm_reg_matcher.cxx            bwm_reg_matcher.h
   bwm_reg_processor.cxx          bwm_reg_processor.h
   bwm_reg_image.cxx              bwm_reg_image.h
                                  bwm_reg_image_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_reg_sources)

ADD_LIBRARY(bwm_reg ${bwm_reg_sources})

TARGET_LINK_LIBRARIES( bwm_reg brip sdet vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgeo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)




