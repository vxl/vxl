# Kd tree and k nearest neighbor library adapted from nabo to use vnl
# nabo was written by Stephane Magnenat
# brl/bbas/bvgl/bnabo/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bvgl/bnabo
  DEPENDS core/vnl
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(bnabo_sources
 bnabo_index_heap.h bnabo.h bnabo_private.h
 bnabo_kdd_tree.cxx bnabo.cxx bnabo_brute_force.cxx
)

#Create NABO port to vnl library
VXL_ADD_LIBRARY(LIBRARY_NAME bnabo LIBRARY_SOURCES ${bnabo_sources})

TARGET_LINK_LIBRARIES(${VXL_LIB_PREFIX}bnabo ${VXL_LIB_PREFIX}vnl)
