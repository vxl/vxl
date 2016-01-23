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

add_library( bmsh3d_algo  ${bmsh3d_algo_sources})
target_link_libraries( bmsh3d_algo bmsh3d rply vgl vnl vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

#install the .h .txx and libs
install_targets(/lib bmsh3d_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bmsh3d/algo/ ${bmsh3d_algo_sources})
