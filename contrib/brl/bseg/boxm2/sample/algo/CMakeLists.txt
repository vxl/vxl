# brl/bseg/boxm2/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_sample_algo_sources
   boxm_mob_grey_processor.h        boxm_mob_grey_processor.cxx
   boxm_mog_grey_processor.h        boxm_mog_grey_processor.cxx
   boxm_simple_grey_processor.h     boxm_simple_grey_processor.cxx
   )

ADD_LIBRARY(boxm2_sample_algo ${boxm2_sample_algo_sources})

TARGET_LINK_LIBRARIES(boxm2_sample_algo boxm2 boxm2_util bsta bsta_algo vnl )

