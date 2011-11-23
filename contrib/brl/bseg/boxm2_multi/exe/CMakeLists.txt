# contrib/brl/bseg/boxm2_multi/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2_multi/ )

#exec for render
ADD_EXECUTABLE( boxm2_multi_render_exe
                boxm2_multi_render.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_multi_render_exe boxm2_multi boxm2_multi_algo boxm2 boxm2_ocl_pro vil_pro vpgl_pro vbl) #boxm2_basic boxm2_io boxm2_ocl bocl vsph vil vul

#exec for update
ADD_EXECUTABLE( boxm2_multi_update_exe
                boxm2_multi_update.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_multi_update_exe boxm2_multi boxm2_multi_algo boxm2 boxm2_ocl_pro vil_pro vpgl_pro vbl)
