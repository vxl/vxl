# 3D application
# bbas/bmsh3d/vis/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

include_directories( ${BRL_INCLUDE_DIR} )

if(HAS_BGUI3D)

set(bmsh3d_vis_sources
    bmsh3d_vis_utils.cxx        bmsh3d_vis_utils.h
    bmsh3d_vis_backpt.cxx       bmsh3d_vis_backpt.h
    bmsh3d_vis_vertex.cxx       bmsh3d_vis_vertex.h
    bmsh3d_vis_edge.cxx         bmsh3d_vis_edge.h
    bmsh3d_vis_face.cxx         bmsh3d_vis_face.h
    bmsh3d_vis_mesh.cxx         bmsh3d_vis_mesh.h
   )

vxl_add_library(LIBRARY_NAME bmsh3d_vis LIBRARY_SOURCES ${bmsh3d_vis_sources})
target_link_libraries( bmsh3d_vis bmsh3d bmsh3d_algo bmsh3d_pro ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul )

add_subdirectory(tests)

endif()
