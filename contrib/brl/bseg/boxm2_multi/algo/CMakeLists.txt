# contrib/brl/bseg/boxm2/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2_multi/ )

SET(boxm2_multi_algo_sources
    boxm2_multi_render.h       boxm2_multi_render.cxx
    boxm2_multi_update.h       boxm2_multi_update.cxx
    
    boxm2_multi_store_aux.h    boxm2_multi_store_aux.cxx
    boxm2_multi_pre_vis_inf.h  boxm2_multi_pre_vis_inf.cxx
    boxm2_multi_update_cell.h  boxm2_multi_update_cell.cxx
    boxm2_multi_refine.h       boxm2_multi_refine.cxx
   )

ADD_LIBRARY(boxm2_multi_algo  ${boxm2_multi_algo_sources})
TARGET_LINK_LIBRARIES(boxm2_multi_algo boxm2_multi boxm2_ocl_algo)# boxm2_ocl boxm2_ocl_pro boct brdb bsta vbl_io vpgl_algo vpgl vgl_xio vgl vnl_algo vnl vbl vil vul vsl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

