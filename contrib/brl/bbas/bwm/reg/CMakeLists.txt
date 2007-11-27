# world modeling application
# contrib/bwm/reg/CMakeLists.txt


SET(bwm_reg_sources
   bwm_reg_utils.cxx              bwm_reg_utils.h
   bwm_reg_edge_champher.cxx      bwm_reg_edge_champher.h
   bwm_reg_matcher.cxx            bwm_reg_matcher.h
)

ADD_LIBRARY(bwm_reg ${bwm_reg_sources})

TARGET_LINK_LIBRARIES( bwm_reg sdet vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgeo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)




