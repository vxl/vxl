# ./oul/ouel/CMakeLists.txt

SOURCE_FILES(ouel_sources
  kalman_filter.h    kalman_filter.cxx
)

ADD_LIBRARY(ouel ouel_sources)
TARGET_LINK_LIBRARIES( ouel vnl_algo )
