# segmentation detection library
# brl/bbas/btol/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/btol
  DEPENDS contrib/brl/bbas/bsol contrib/gel/vtol core/vbl
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR} )

set(btol_sources
    dll.h

    btol_edge_algs.cxx     btol_edge_algs.h
    btol_vertex_algs.cxx   btol_vertex_algs.h
    btol_face_algs.cxx     btol_face_algs.h
)

add_library(btol ${btol_sources})
install_targets(/lib btol)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/btol ${btol_sources})

target_link_libraries(btol bsol vtol vdgl vsol vgl vnl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
