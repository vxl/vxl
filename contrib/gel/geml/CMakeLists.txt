# gel/geml/CMakeLists.txt

ADD_LIBRARY(geml geml_sources)

SOURCE_FILES(geml_sources
 geml_matcher.cxx             geml_matcher.h
 geml_matcher_correlation.cxx geml_matcher_correlation.h
)
