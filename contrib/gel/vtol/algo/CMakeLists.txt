# vtol/algo/CMakeLists.txt

set( vtol_algo_sources
  vtol_extract_topology.h  vtol_extract_topology.cxx  vtol_extract_topology.txx
# vtol_extract_topology_internals.h     vtol_extract_topology_internals.cxx
)

aux_source_directory( Templates vtol_algo_sources )

add_library( vtol_algo ${vtol_algo_sources} )
install_targets(/lib vtol_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/vtol/algo ${vtol_algo_sources})
target_link_libraries( vtol_algo vtol vsol vdgl vil_algo vgl_algo vgl vil vbl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
