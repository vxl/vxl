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

vxl_add_library(LIBRARY_NAME btol LIBRARY_SOURCES ${btol_sources})

target_link_libraries(btol bsol vtol vdgl vsol ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
