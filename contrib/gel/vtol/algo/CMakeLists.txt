# vtol/algo/CMakeLists.txt

SET( vtol_algo_sources
  vtol_extract_topology.h               vtol_extract_topology.cxx
)

ADD_LIBRARY( vtol_algo ${vtol_algo_sources} )
TARGET_LINK_LIBRARIES( vtol_algo vtol vil vbl )
