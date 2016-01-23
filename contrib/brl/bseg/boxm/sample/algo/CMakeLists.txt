# brl/bseg/boxm/util/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
set(boxm_sample_algo_sources
    boxm_mob_grey_processor.h        boxm_mob_grey_processor.cxx
    boxm_mog_grey_processor.h        boxm_mog_grey_processor.cxx
    boxm_simple_grey_processor.h     boxm_simple_grey_processor.cxx
    boxm_sigma_normalizer.h          boxm_sigma_normalizer.cxx
   )

aux_source_directory(Templates boxm_sample_algo_sources)

add_library(boxm_sample_algo ${boxm_sample_algo_sources})

target_link_libraries(boxm_sample_algo boxm_sample boxm boxm_util bsta bsta_algo vnl vgl_io vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
