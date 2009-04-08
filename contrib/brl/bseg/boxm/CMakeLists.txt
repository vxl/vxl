# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )
#INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expat )
SET(boxm_sources
    boxm_block.txx                    boxm_block.h           
    boxm_scene.txx                    boxm_scene.h
    boxm_sample.txx                   boxm_sample.h        
    boxm_apm_traits.h
    boxm_mog_grey_processor.cxx       boxm_mog_grey_processor.h  
    boxm_block_vis_graph_node.h
    boxm_block_vis_graph_iterator.txx boxm_block_vis_graph_iterator.h
    boxm_cell_vis_graph_node.h
    boxm_cell_vis_graph_iterator.h
    boxm_neighb_graph_node.h
    boxm_utils.cxx                    boxm_utils.h
)

AUX_SOURCE_DIRECTORY(Templates boxm_sources)

ADD_LIBRARY(boxm  ${boxm_sources})

TARGET_LINK_LIBRARIES(boxm boct boxm_io vcl vgl vgl_io vgl_xio vbl vsl vpgl bgeo bsta)


#processes 
SUBDIRS(pro)

SUBDIRS(io)

#tests
SUBDIRS(tests)

ENDIF(EXPAT_FOUND)