# ./oul/ouel/CMakeLists.txt

SET(ouel_sources
  kalman_filter.h    kalman_filter.cxx
)

ADD_LIBRARY(ouel ${ouel_sources})
TARGET_LINK_LIBRARIES( ouel vnl_algo )
