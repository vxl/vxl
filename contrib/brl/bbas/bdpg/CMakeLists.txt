# brl/bbas/bdpg/CMakeLists.txt
# writen by J.L. Mundy for basic dynamic programming classes
include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )

set(bdpg_sources
bdpg_node.cxx                     bdpg_node.h                  bdpg_node_sptr.h
bdpg_array_dynamic_prg.cxx    bdpg_array_dynamic_prg.h
)
aux_source_directory(Templates bdpg_sources)

add_library(bdpg ${bdpg_sources})
install_targets(/lib bdpg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bdpg ${bdpg_sources})

target_link_libraries(bdpg ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
