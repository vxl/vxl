// This is brl/bbas/imesh/imesh_operations.h
#ifndef imesh_operations_h_
#define imesh_operations_h_
//:
// \file
// \brief Operations on meshes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 5, 2008
//
// \verbatim
//  Modifications
//   2013-03-05 P.Vanroose - signature of contains_point made consistent w. .cxx
// \endverbatim


#include <iostream>
#include <set>
#include "imesh_mesh.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>

//: Subdivide mesh faces into triangle
std::unique_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_face_array_base& faces);

//: Subdivide quadrilaterals into triangle
std::unique_ptr<imesh_regular_face_array<3> >
imesh_triangulate(const imesh_regular_face_array<4>& faces);


//: Triangulate the faces of the mesh (in place)
void
imesh_triangulate(imesh_mesh& mesh);


//: Subdivide faces into quadrilaterals (in place)
//  Add a vertex at the center of each edge
//  And a vertex at the center of each face
void
imesh_quad_subdivide(imesh_mesh& mesh);

//: Subdivide faces into quadrilaterals (in place)
//  Add a vertex at the center of each edge
//  And a vertex at the center of each face
//  Only subdivide the selected faces
void
imesh_quad_subdivide(imesh_mesh& mesh, const std::set<unsigned int>& sel_faces);


//: Extract a sub-mesh containing only the faces listed in sel_faces
imesh_mesh
imesh_submesh_from_faces(const imesh_mesh& mesh, const std::set<unsigned int>& sel_faces);


//: Flip the orientation of all mesh faces
void imesh_flip_faces( imesh_mesh& mesh );

//: Flip the orientation of the selected faces
void imesh_flip_faces( imesh_mesh& mesh, const std::set<unsigned int>& sel_faces);


//: Compute the dual mesh using face centroids for vertices
imesh_mesh dual_mesh(const imesh_mesh& mesh);

bool contains_point( imesh_mesh const& mesh,vgl_point_3d<double> p);

#endif // imesh_operations_h_
