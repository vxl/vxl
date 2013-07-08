# contrib/brl/bbas/volm/desc/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/ )

ADD_EXECUTABLE( volm_desc_land_exe
                volm_desc_land_exe.cxx
              )
              
TARGET_LINK_LIBRARIES( volm_desc_land_exe volm_desc )