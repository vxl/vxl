# brl/bbas/bdpg/CMakeLists.txt
# writen by J.L. Mundy for basic dynamic programming classes
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bdpg_sources
bdpg_node.cxx                     bdpg_node.h                  bdpg_node_sptr.h
bdpg_array_dynamic_prg.cxx    bdpg_array_dynamic_prg.h 
)
AUX_SOURCE_DIRECTORY(Templates bdpg_sources)

ADD_LIBRARY(bdpg ${bdpg_sources})

TARGET_LINK_LIBRARIES(bdpg vnl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
