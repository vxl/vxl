# segmentation detection library
# brl/bbas/btol/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
SET(btol_sources
    btol_edge_algs.cxx     btol_edge_algs.h
    btol_vertex_algs.cxx   btol_vertex_algs.h
    btol_face_algs.cxx     btol_face_algs.h
)
AUX_SOURCE_DIRECTORY(Templates btol_sources)

ADD_LIBRARY(btol ${btol_sources})

TARGET_LINK_LIBRARIES(btol vtol vsol vbl vul)
