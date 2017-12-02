# This is contrib/brl/bseg/bvpl/util/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl )

set(bvpl_util_sources
    bvpl_corner_pair_finder.h    bvpl_corner_pair_finder.cxx
    bvpl_corner_search_functor.h bvpl_corner_search_functor.cxx
    bvpl_bundler_features_2d.h   bvpl_bundler_features_2d.cxx
   )
aux_source_directory(Templates bvpl_util_sources)

vxl_add_library(LIBRARY_NAME bvpl_util LIBRARY_SOURCES ${bvpl_util_sources})
target_link_libraries(bvpl_util bvpl brdb ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

add_subdirectory(io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
