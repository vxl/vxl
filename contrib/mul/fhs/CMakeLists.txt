# mul/fhs/CMakeLists.txt

doxygen_add_library(contrib/mul/fhs
  DEPENDS contrib/mul/vimt contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Feature matching using Felzenszwalb and Huttenlocher's method"
  )

set(fhs_sources
  fhs_arc.cxx       fhs_arc.h
  fhs_searcher.cxx  fhs_searcher.h
)
aux_source_directory(Templates fhs_sources)

add_library(fhs ${fhs_sources})
target_link_libraries(fhs vimt vnl vil_algo vgl vsl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()
