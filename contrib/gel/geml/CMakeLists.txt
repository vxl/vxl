# gel/geml/CMakeLists.txt

SET(geml_sources
 geml_matcher.cxx             geml_matcher.h
 geml_matcher_correlation.cxx geml_matcher_correlation.h
)

ADD_LIBRARY(geml ${geml_sources})
INSTALL_TARGETS(/lib geml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/geml ${geml_sources})
TARGET_LINK_LIBRARIES(geml vil1 vnl)
