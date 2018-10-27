// This is brl/bbas/imesh/algo/imesh_transform.cxx

//:
// \file


#include "imesh_transform.h"


//: Translate the vertices in place
void imesh_transform_inplace(imesh_mesh& mesh,
                             const vgl_vector_3d<double>& t)
{
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  for(auto & v : verts)
  {
    v[0] += t.x();
    v[1] += t.y();
    v[2] += t.z();
  }
}


//: Rotate the vertices in place
void imesh_transform_inplace(imesh_mesh& mesh,
                             const vgl_rotation_3d<double>& rotation)
{
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  vnl_matrix_fixed<double,3,3> R = rotation.as_matrix();
  for(auto & v : verts)
  {
    v = imesh_vertex<3>(R[0][0]*v[0] + R[0][1]*v[1] + R[0][2]*v[2],
                        R[1][0]*v[0] + R[1][1]*v[1] + R[1][2]*v[2],
                        R[2][0]*v[0] + R[2][1]*v[1] + R[2][2]*v[2]);
  }

}


//: Rotate and translate the vertices in place
void imesh_transform_inplace(imesh_mesh& mesh,
                             const vgl_rotation_3d<double>& rotation,
                             const vgl_vector_3d<double>& t)
{
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  vnl_matrix_fixed<double,3,3> R = rotation.as_matrix();
  for(auto & v : verts)
  {
    v = imesh_vertex<3>(R[0][0]*v[0] + R[0][1]*v[1] + R[0][2]*v[2] + t.x(),
                        R[1][0]*v[0] + R[1][1]*v[1] + R[1][2]*v[2] + t.y(),
                        R[2][0]*v[0] + R[2][1]*v[1] + R[2][2]*v[2] + t.z());
  }

}
