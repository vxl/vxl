# gel/geml/CMakeLists.txt

doxygen_add_library(contrib/gel/geml
  DEPENDS core/vil1 core/vnl
  PACKAGE contrib/gel
  DESCRIPTION "Match marked points in second image"
  )

set(geml_sources
 geml_matcher.cxx             geml_matcher.h
 geml_matcher_correlation.cxx geml_matcher_correlation.h
)

vxl_add_library(LIBRARY_NAME geml LIBRARY_SOURCES ${geml_sources})
target_link_libraries(geml ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
