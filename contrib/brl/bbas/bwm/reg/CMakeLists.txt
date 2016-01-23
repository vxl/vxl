# world modeling application
# brl/bbas/bwm/reg/CMakeLists.txt


set(bwm_reg_sources
   bwm_reg_utils.cxx              bwm_reg_utils.h
   bwm_reg_edge_champher.cxx      bwm_reg_edge_champher.h
   bwm_reg_matcher.cxx            bwm_reg_matcher.h
   bwm_reg_processor.cxx          bwm_reg_processor.h
   bwm_reg_image.cxx              bwm_reg_image.h
                                  bwm_reg_image_sptr.h
)

aux_source_directory(Templates bwm_reg_sources)

add_library(bwm_reg ${bwm_reg_sources})

target_link_libraries( bwm_reg brip sdet vil vbl vgl vnl bsta bsol vsol vpgl vpgl_algo )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

#install the .h .txx and libs
install_targets(/lib bwm_reg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/reg/ ${bwm_reg_sources})



