# ./gel/vtol/CMakeLists.txt

ADD_LIBRARY( vtol vtol_sources )

SOURCE_FILES( vtol_sources
  vtol_topology_object.cxx
  vtol_topology_cache.cxx
  vtol_list_functions.cxx
  vtol_chain.cxx
  vtol_two_chain.cxx
  vtol_one_chain.cxx
  vtol_zero_chain.cxx
  vtol_edge.cxx
  vtol_block.cxx
  vtol_face.cxx
  vtol_vertex.cxx
  vtol_edge_2d.cxx
  vtol_face_2d.cxx
  vtol_vertex_2d.cxx
  vtol_topology_io.cxx
  vtol_cycle_processor.cxx
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

AUX_SOURCE_DIRECTORY( Templates vtol_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
