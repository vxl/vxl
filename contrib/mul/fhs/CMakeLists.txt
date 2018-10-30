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

vxl_add_library(LIBRARY_NAME fhs LIBRARY_SOURCES ${fhs_sources})
target_link_libraries(fhs vimt ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(VXL_BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()
