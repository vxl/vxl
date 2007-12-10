# 3D application
# contrib/dbmsh3d/vis/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/UseBGUI3D.cmake )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

IF (HAS_BGUI3D)

SET(bmsh3d_vis_sources
   bmsh3d_vis_utils.cxx        bmsh3d_vis_utils.h
   bmsh3d_vis_backpt.cxx       bmsh3d_vis_backpt.h
   bmsh3d_vis_vertex.cxx       bmsh3d_vis_vertex.h
   bmsh3d_vis_edge.cxx         bmsh3d_vis_edge.h
   bmsh3d_vis_face.cxx         bmsh3d_vis_face.h
   bmsh3d_vis_mesh.cxx         bmsh3d_vis_mesh.h
)

ADD_LIBRARY(bmsh3d_vis ${bmsh3d_vis_sources})

TARGET_LINK_LIBRARIES( bmsh3d_vis bmsh3d bmsh3d_algo bmsh3d_pro bgui3d )

ENDIF (HAS_BGUI3D)

