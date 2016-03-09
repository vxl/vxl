# vtol/algo/CMakeLists.txt

set( vtol_algo_sources
  vtol_extract_topology.h  vtol_extract_topology.cxx  vtol_extract_topology.hxx
# vtol_extract_topology_internals.h     vtol_extract_topology_internals.cxx
)

aux_source_directory( Templates vtol_algo_sources )

vxl_add_library(LIBRARY_NAME vtol_algo LIBRARY_SOURCES ${vtol_algo_sources} )
target_link_libraries( vtol_algo vtol vsol vdgl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
