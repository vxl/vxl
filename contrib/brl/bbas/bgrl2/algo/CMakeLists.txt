# bbas/bgrl2/algo/CMakeLists.txt

set( bgrl2_algo_sources
  bgrl2_algs.cxx              bgrl2_algs.h
)

add_library( bgrl2_algo  ${bgrl2_algo_sources})

target_link_libraries( bgrl2_algo bgrl2 )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
