ADD_LIBRARY( vtol vtol_sources )

SOURCE_FILES( vtol_sources
  vtol_topology_object
  vtol_topology_cache
  vtol_list_functions
  vtol_chain
  vtol_two_chain
  vtol_one_chain
  vtol_zero_chain
  vtol_edge
  vtol_block
  vtol_face
  vtol_vertex
  vtol_edge_2d
  vtol_face_2d
  vtol_vertex_2d
  vtol_topology_io
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

AUX_SOURCE_DIRECTORY( Templates vtol_sources )

SUBDIRS( tests examples )
