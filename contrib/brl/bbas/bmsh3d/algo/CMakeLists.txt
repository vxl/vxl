# This is brcv/shp/dbmsh3d/algo/CMakeLists.txt
# Brown 3D Mesh Algorithm Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

SET( bmsh3d_algo_sources
  bmsh3d_fileio.cxx                bmsh3d_fileio.h
  bmsh3d_fileio_ply.cxx
  bmsh3d_mesh_triangulate.cxx      bmsh3d_mesh_triangulate.h
  bmsh3d_mesh_tri.cxx      bmsh3d_mesh_tri.h
  bmsh3d_mesh_bnd.cxx      bmsh3d_mesh_bnd.h
)


ADD_LIBRARY( bmsh3d_algo  ${bmsh3d_algo_sources})

TARGET_LINK_LIBRARIES( bmsh3d_algo bmsh3d vgl_algo rply)

#SUBDIRS (tests)


