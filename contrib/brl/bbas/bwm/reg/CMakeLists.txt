# world modeling application
# brl/bbas/bwm/reg/CMakeLists.txt


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

TARGET_LINK_LIBRARIES( bwm_reg brip sdet vil vbl vgl vnl bsta bsol vsol vpgl vpgl_algo )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

#install the .h .txx and libs
INSTALL_TARGETS(/lib bwm_reg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/reg/ ${bwm_reg_sources})



