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

add_library(geml ${geml_sources})
install_targets(/lib geml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/geml ${geml_sources})
target_link_libraries(geml vil1 vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
