#
# ./gel/vtol/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vtol vtol_sources)

SOURCE_FILES(vtol_sources
vtol_block    vtol_face_2d          vtol_two_chain
vtol_chain    vtol_list_functions   vtol_vertex
vtol_one_chain        vtol_vertex_2d
vtol_edge     vtol_topology_cache   vtol_zero_chain
vtol_edge_2d  
# vtol_topology_io
#   fails under Win32
vtol_face     vtol_topology_object
)

AUX_SOURCE_DIRECTORY(Templates vtol_sources)