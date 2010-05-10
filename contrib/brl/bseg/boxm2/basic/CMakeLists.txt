# brl/bseg/boxm2/basic/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_basic_sources
   boxm_block_vis_graph_iterator.h     boxm_block_vis_graph_iterator.txx
   boxm_block_vis_graph_node.h
   boxm_cell_vis_graph_iterator.h      boxm_cell_vis_graph_iterator.txx
   boxm_raytrace_function.h
   boxm_seg_length_functor.h
   dummy.cxx
   )


AUX_SOURCE_DIRECTORY(Templates boxm2_basic_sources)

ADD_LIBRARY(boxm2_basic ${boxm2_basic_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm2_basic boxm2 boxm2_util boxm2_sample boxm2_sample_algo boct vgl vpgl vpgl_algo vgl vul vcl)

