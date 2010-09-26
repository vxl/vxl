# gel/geml/CMakeLists.txt

doxygen_add_library(contrib/gel/geml
  DEPENDS core/vil1 core/vnl
  PACKAGE contrib/gel
  DESCRIPTION "Match marked points in second image"
  )

SET(geml_sources
 geml_matcher.cxx             geml_matcher.h
 geml_matcher_correlation.cxx geml_matcher_correlation.h
)

ADD_LIBRARY(geml ${geml_sources})
INSTALL_TARGETS(/lib geml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/geml ${geml_sources})
TARGET_LINK_LIBRARIES(geml vil1 vnl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
