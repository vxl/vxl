# This is bbas/bmsh3d/algo/CMakeLists.txt
# Brown 3D Mesh Algorithm Library

include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${VTHREEP_INCLUDE_DIR}/rply )

set( bmsh3d_algo_sources
  bmsh3d_fileio.cxx           bmsh3d_fileio.h
  bmsh3d_fileio_ply.cxx
  bmsh3d_mesh_triangulate.cxx bmsh3d_mesh_triangulate.h
  bmsh3d_mesh_tri.cxx         bmsh3d_mesh_tri.h
  bmsh3d_mesh_bnd.cxx         bmsh3d_mesh_bnd.h
)

vxl_add_library(LIBRARY_NAME bmsh3d_algo LIBRARY_SOURCES  ${bmsh3d_algo_sources})
target_link_libraries( bmsh3d_algo bmsh3d rply ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#install the .h .hxx and libs
