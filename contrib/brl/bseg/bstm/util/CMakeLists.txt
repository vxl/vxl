# contrib/brl/bseg/bstm/util/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_util_sources
    bstm_cams_and_box_to_scene.h   bstm_cams_and_box_to_scene.cxx
   )

vxl_add_library(LIBRARY_NAME bstm_util LIBRARY_SOURCES  ${bstm_util_sources})
target_link_libraries(bstm_util bstm expatpp ${VXL_LIB_PREFIX}vpgl baio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)
