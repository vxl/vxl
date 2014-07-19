# contrib/brl/bseg/bstm/util/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_util_sources
    bstm_cams_and_box_to_scene.h   bstm_cams_and_box_to_scene.cxx 
   )
  
ADD_LIBRARY(bstm_util  ${bstm_util_sources})
TARGET_LINK_LIBRARIES(bstm_util bstm expatpp vpgl baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)