//:
// \file
// \date Sep 25, 2005
// \author MingChing Chang

#ifndef bmsh3d_mesh_triangulate_h_
#define bmsh3d_mesh_triangulate_h_

#include <bmsh3d/bmsh3d_mesh.h>
#include "bmsh3d_mesh_triangulate.h"

//: Triangulate the input mesh face into a set of triangular faces.
//  The mesh face can be any contour/polygon without holes.
//  Return true if success.
//  Results:
//    tri_faces: a vector of triangular faces specified in their
//               ids of the original face F, e.g.,
//               f0: (v0, v1, v2), f1: (v0, v1, v2), ..., etc.
//
bool bmsh3d_triangulate_face (const bmsh3d_face* F, vcl_vector<vcl_vector<int> >& tri_faces);

bmsh3d_mesh* generate_tri_mesh (bmsh3d_mesh* M);


#endif


