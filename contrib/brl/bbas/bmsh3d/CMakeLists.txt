# bmsh3d - Brown 3D Mesh Library
# brl/bbas/bmsh3d/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${GEL_INCLUDE_DIR} )

set(bmsh3d_sources
                                    bmsh3d_utils.h
                                    bmsh3d_ptr_list.h

   bmsh3d_vertex.cxx                bmsh3d_vertex.h

   bmsh3d_halfedge.cxx              bmsh3d_halfedge.h
   bmsh3d_edge.cxx                  bmsh3d_edge.h
   bmsh3d_face.cxx                  bmsh3d_face.h
   bmsh3d_face_mc.cxx               bmsh3d_face_mc.h
   bmsh3d_textured_face_mc.cxx      bmsh3d_textured_face_mc.h

   bmsh3d_pt_set.cxx                bmsh3d_pt_set.h
   bmsh3d_mesh.cxx                  bmsh3d_mesh.h
   bmsh3d_mesh_mc.cxx               bmsh3d_mesh_mc.h
   bmsh3d_textured_mesh_mc.cxx      bmsh3d_textured_mesh_mc.h

   bmsh3d_he_mesh.cxx               bmsh3d_he_mesh.h

   bmsh3d_mesh_conn_recov.cxx

                                    bmsh3d_fuzzy_boolean.h
                                    bmsh3d_triangle.h
                                    bmsh3d_dist.h
                                    bmsh3d_graph.h
)

vxl_add_library(LIBRARY_NAME bmsh3d LIBRARY_SOURCES  ${bmsh3d_sources})

target_link_libraries( bmsh3d ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul)

add_subdirectory(algo)
add_subdirectory(pro)
add_subdirectory(vis)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

#install the .h .hxx and libs
