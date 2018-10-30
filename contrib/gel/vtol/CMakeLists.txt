# gel/vtol/CMakeLists.txt

doxygen_add_library(contrib/gel/vtol
  DEPENDS contrib/gel/vdgl core/vil
  PACKAGE contrib/gel
  DESCRIPTION "1, 2 and 3D Topology representations"
  )

set(vtol_sources
 vtol_topology_object.cxx vtol_topology_object.h vtol_topology_object_sptr.h
 vtol_topology_cache.cxx  vtol_topology_cache.h  vtol_topology_cache_sptr.h
 vtol_list_functions.hxx  vtol_list_functions.h
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

aux_source_directory(Templates vtol_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(vtol_two_chain.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()

vxl_add_library(LIBRARY_NAME vtol LIBRARY_SOURCES ${vtol_sources})
target_link_libraries(vtol vdgl vsol ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${CMAKE_THREAD_LIBS})

add_subdirectory(algo)

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
