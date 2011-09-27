# This is bbas/bmsh3d/algo/CMakeLists.txt
# Brown 3D Mesh Algorithm Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )

SET( bmsh3d_algo_sources
  bmsh3d_fileio.cxx           bmsh3d_fileio.h
  bmsh3d_fileio_ply.cxx
  bmsh3d_mesh_triangulate.cxx bmsh3d_mesh_triangulate.h
  bmsh3d_mesh_tri.cxx         bmsh3d_mesh_tri.h
  bmsh3d_mesh_bnd.cxx         bmsh3d_mesh_bnd.h
)

ADD_LIBRARY( bmsh3d_algo  ${bmsh3d_algo_sources})
TARGET_LINK_LIBRARIES( bmsh3d_algo bmsh3d rply vgl vnl vul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#install the .h .txx and libs
INSTALL_TARGETS(/lib bmsh3d_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bmsh3d/algo/ ${bmsh3d_algo_sources})
