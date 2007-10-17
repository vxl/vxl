# bmsh3d - Brown 3D Mesh Library
# brl/bbas/bmsh3d/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

SET(bmsh3d_sources
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
)

ADD_LIBRARY( bmsh3d  ${bmsh3d_sources})

TARGET_LINK_LIBRARIES( bmsh3d vsol vil)

SUBDIRS( algo )
SUBDIRS( pro )
SUBDIRS( vis )
SUBDIRS( tests )
SUBDIRS( examples )



