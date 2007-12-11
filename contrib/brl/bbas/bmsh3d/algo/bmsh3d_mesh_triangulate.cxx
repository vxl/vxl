//:
// \file
// \date Sep 25, 2005
// \author MingChing Chang

#include "bmsh3d_mesh_tri.h"
#include "bmsh3d_mesh_triangulate.h"
#include <vcl_cassert.h>

//: Triangulate the input mesh face F into a set of triangular faces.
//  The mesh face can be any contour/polygon without holes.
//  Return true if success.
//  Results:
//    tri_faces: a vector of triangular faces specified in their
//               ids of the original face F, e.g.,
//               f0: (v0, v1, v2), f1: (v0, v1, v2), ..., etc.
//
bool bmsh3d_triangulate_face (const bmsh3d_face* F, vcl_vector<vcl_vector<int> >& tri_faces)
{
  //triangulate the face
  Vector2dVector face_polygon;

  vcl_vector<bmsh3d_vertex*> vertices;
  if (F->vertices().size() != 0) {
    //Face in IFS
    for (unsigned int i=0; i<F->vertices().size(); i++)
      vertices.push_back ((bmsh3d_vertex*) F->vertices(i));
  }
  else { //Face in MHE
    F->get_ordered_Vs (vertices);
  }
  assert (vertices.size() > 3);

  //Project the 3d coord of each vertex onto a reference plane
  //to get its 2d coord.
  vgl_vector_3d<double> normal = compute_normal_ifs (vertices);

  //Use V[0] as origin O, use V[1]-V[0] as e1, e2 = N * e1
  vgl_point_3d<double> o = vertices[0]->pt();
  vgl_vector_3d<double> e1 = vertices[1]->pt() - o;
  vgl_vector_3d<double> e2 = cross_product (normal, e1);

  //Normalize e1 and e2
  e1 = normalize (e1);
  e2 = normalize (e2);

  //Any given point P on the 3D polygon can be projected to the plane
  //  OP = u e1 + v e2
  for (unsigned int j=0; j<vertices.size(); j++) {
    const bmsh3d_vertex* V = vertices[j];

    //For any given point P on the 3D polygon, can project to the plane
    //  OP = u e1 + v e2
    vgl_vector_3d<double> op = V->pt() - o;
    float u = (float) dot_product (op, e1);
    float v = (float) dot_product (op, e2);

    //Use the (u, v) on the 2D plane.
    //Should compute normal and project to the polygon plane!
    face_polygon.push_back (Vector2d(u, v, j)); ///V->id()));
  }

  //Allocate an STL vector to hold the answer.
  Vector2dVector result;

  //Triangulate this polygon.
  Triangulate::Process (face_polygon, result);

  unsigned int tcount = result.size()/3;
  //if any numerical arises, the following is not true.
  //assert (vertices.size() == tcount+2);
  if (vertices.size() != tcount+2)
    return false;

  for (int i=0; i<int(tcount); i++) {
    const Vector2d &p1 = result[i*3+0];
    const Vector2d &p2 = result[i*3+1];
    const Vector2d &p3 = result[i*3+2];

    ///vul_printf (vcl_cerr, "Triangle %d => (%f, %f) (%f, %f) (%f, %f)\n",
    //              i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
    ///vul_printf (vcl_cerr, "Triangle %d => v[%d], v[%d], v[%d]\n", i+1, p1.id_, p2.id_, p3.id_);

    //For each triangle, add as a new face
    vcl_vector<int> tri;
    tri.clear();
    tri.push_back (p1.id_);
    tri.push_back (p2.id_);
    tri.push_back (p3.id_);
    tri_faces.push_back (tri);
  }

  return true;
}

bmsh3d_mesh* generate_tri_mesh (bmsh3d_mesh* M)
{
  bmsh3d_mesh* triM = new bmsh3d_mesh;

  //Put all existing vertices into the new triM
  vcl_map<int, bmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;

    bmsh3d_vertex* newV = triM->_new_vertex ();
    newV->set_id (V->id());
    newV->get_pt().set (V->pt().x(), V->pt().y(), V->pt().z());
    triM->_add_vertex (newV);
  }

  //Triangulate each face of the mesh
  vcl_map<int, bmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    bmsh3d_face* F = (*it).second;

    //If the input face is already a triangle, make a new triangular face.
    vcl_vector<bmsh3d_vertex*> vertices;
    F->get_ordered_Vs (vertices);
    if (vertices.size() < 4) {
      assert (vertices.size() == 3);
      //make a new triangular face
      bmsh3d_face* newF = triM->_new_face ();

      for (unsigned int i=0; i<vertices.size(); i++) {
        bmsh3d_vertex* V = vertices[i];
        int id = V->id();
        bmsh3d_vertex* newV = triM->vertexmap (id);
        newF->_ifs_add_vertex (newV);
      }

      triM->_add_face (newF);
    }
    else {
      //triangulate the polygonal face F
      vcl_vector<vcl_vector<int> > tri_faces;
      bmsh3d_triangulate_face (F, tri_faces);

      //For each resulting triangle, add a new face.
      for (unsigned int i=0; i<tri_faces.size(); i++) {
        bmsh3d_face* newF = triM->_new_face ();

        bmsh3d_vertex* V0 = triM->vertexmap (tri_faces[i][0]);
        newF->_ifs_add_vertex (V0);
        bmsh3d_vertex* V1 = triM->vertexmap (tri_faces[i][1]);
        newF->_ifs_add_vertex (V1);
        bmsh3d_vertex* V2 = triM->vertexmap (tri_faces[i][2]);
        newF->_ifs_add_vertex (V2);

        triM->_add_face (newF);
      }
    }
  }

  return triM;
}
