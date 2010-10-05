# brl/bseg/boxm/basic/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_basic_sources
    boxm_block_vis_graph_iterator.h     boxm_block_vis_graph_iterator.txx
    boxm_block_vis_graph_node.h
    boxm_cell_vis_graph_iterator.h      boxm_cell_vis_graph_iterator.txx
    boxm_raytrace_function.h
    boxm_seg_length_functor.h
    boxm_num_rays_functor.h
    boxm_util_data_types.h boxm_util_data_types.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm_basic_sources)

ADD_LIBRARY(boxm_basic ${boxm_basic_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm_basic boxm boxm_util boxm_sample boxm_sample_algo boct vpgl vpgl_algo vgl_io vgl vil vul vbl_io vbl vcl)
