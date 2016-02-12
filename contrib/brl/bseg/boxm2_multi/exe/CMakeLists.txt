# contrib/brl/bseg/boxm2_multi/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2_multi/ )

#exec for render
add_executable( boxm2_multi_render_exe
                boxm2_multi_render.cxx
              )
target_link_libraries( boxm2_multi_render_exe boxm2_multi boxm2_multi_algo boxm2 boxm2_ocl_pro vil_pro vpgl_pro ${VXL_LIB_PREFIX}vbl) #boxm2_basic boxm2_io boxm2_ocl bocl vsph ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul

#exec for update
add_executable( boxm2_multi_update_exe
                boxm2_multi_update.cxx
              )
target_link_libraries( boxm2_multi_update_exe boxm2_multi boxm2_multi_algo boxm2 boxm2_ocl_pro vil_pro vpgl_pro ${VXL_LIB_PREFIX}vbl)
