# world modeling application
# contrib/bwm/reg/CMakeLists.txt


SET(bwm_reg_sources
   bwm_reg_utils.cxx      bwm_reg_utils.h
)

ADD_LIBRARY(bwm_reg ${bwm_reg_sources})

TARGET_LINK_LIBRARIES( bwm_reg sdet vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgeo)




