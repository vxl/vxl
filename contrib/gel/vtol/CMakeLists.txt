# gel/vtol/CMakeLists.txt

SET(vtol_sources
 vtol_topology_object.cxx vtol_topology_object.h vtol_topology_object_sptr.h
 vtol_topology_cache.cxx  vtol_topology_cache.h  vtol_topology_cache_sptr.h
 vtol_list_functions.txx  vtol_list_functions.h
 vtol_chain.cxx           vtol_chain.h           vtol_chain_sptr.h
 vtol_two_chain.cxx       vtol_two_chain.h       vtol_two_chain_sptr.h
 vtol_one_chain.cxx       vtol_one_chain.h       vtol_one_chain_sptr.h
 vtol_zero_chain.cxx      vtol_zero_chain.h      vtol_zero_chain_sptr.h
 vtol_edge.cxx            vtol_edge.h            vtol_edge_sptr.h
 vtol_block.cxx           vtol_block.h           vtol_block_sptr.h
 vtol_face.cxx            vtol_face.h            vtol_face_sptr.h
 vtol_vertex.cxx          vtol_vertex.h          vtol_vertex_sptr.h
 vtol_edge_2d.cxx         vtol_edge_2d.h         vtol_edge_2d_sptr.h
 vtol_face_2d.cxx         vtol_face_2d.h         vtol_face_2d_sptr.h
 vtol_vertex_2d.cxx       vtol_vertex_2d.h       vtol_vertex_2d_sptr.h
 vtol_topology_io.cxx     vtol_topology_io.h
 vtol_cycle_processor.cxx vtol_cycle_processor.h
 vtol_intensity_face.cxx  vtol_intensity_face.h  vtol_intensity_face_sptr.h
 vtol_dtd.h
 vtol_macros.h
)

AUX_SOURCE_DIRECTORY(Templates vtol_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(vtol_two_chain.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(vtol ${vtol_sources})
TARGET_LINK_LIBRARIES(vtol vdgl vsol vbl vul ${CMAKE_THREAD_LIBS})

SUBDIRS( algo )

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
