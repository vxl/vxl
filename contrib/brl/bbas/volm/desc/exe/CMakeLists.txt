# contrib/brl/bbas/volm/desc/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/ )

add_executable( volm_desc_land_exe
                volm_desc_land_exe.cxx
              )

target_link_libraries( volm_desc_land_exe volm_desc )
