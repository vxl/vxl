# contrib/brl/bseg/bstm/util/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_util_sources
    bstm_cams_and_box_to_scene.h   bstm_cams_and_box_to_scene.cxx
   )

add_library(bstm_util  ${bstm_util_sources})
target_link_libraries(bstm_util bstm expatpp vpgl baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)
