# vtol/algo/CMakeLists.txt

SET( vtol_algo_sources
  vtol_extract_topology.h  vtol_extract_topology.cxx  vtol_extract_topology.txx
# vtol_extract_topology_internals.h     vtol_extract_topology_internals.cxx
)

AUX_SOURCE_DIRECTORY( Templates vtol_algo_sources )

ADD_LIBRARY( vtol_algo ${vtol_algo_sources} )
TARGET_LINK_LIBRARIES( vtol_algo vtol vdgl vil_algo vgl_algo vgl vil vbl )
