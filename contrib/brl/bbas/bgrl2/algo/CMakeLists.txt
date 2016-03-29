# bbas/bgrl2/algo/CMakeLists.txt

set( bgrl2_algo_sources
  bgrl2_algs.cxx              bgrl2_algs.h
)

vxl_add_library(LIBRARY_NAME bgrl2_algo LIBRARY_SOURCES  ${bgrl2_algo_sources})

target_link_libraries( bgrl2_algo bgrl2 )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
