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

vxl_add_library(LIBRARY_NAME bwm_reg LIBRARY_SOURCES ${bwm_reg_sources})

target_link_libraries( bwm_reg brip sdet ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl bsta bsol vsol ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

#install the .h .hxx and libs



