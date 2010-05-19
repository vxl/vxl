# brl/bseg/boxm/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_sample_algo_sources
   boxm_mob_grey_processor.h        boxm_mob_grey_processor.cxx
   boxm_mog_grey_processor.h        boxm_mog_grey_processor.cxx
   boxm_simple_grey_processor.h     boxm_simple_grey_processor.cxx
   )

ADD_LIBRARY(boxm_sample_algo ${boxm_sample_algo_sources})

TARGET_LINK_LIBRARIES(boxm_sample_algo boxm boxm_util bsta bsta_algo vnl )

