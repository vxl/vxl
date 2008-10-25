#include "test_share.h"
#include <testlib/testlib_test.h>
#include <imesh/algo/imesh_transform.h>

void make_cube(imesh_mesh& cube)
{
  imesh_vertex_array<3>* verts = new imesh_vertex_array<3>();
  verts->push_back(imesh_vertex<3>( 1, 1, 1));
  verts->push_back(imesh_vertex<3>(-1, 1, 1));
  verts->push_back(imesh_vertex<3>(-1,-1, 1));
  verts->push_back(imesh_vertex<3>( 1,-1, 1));
  verts->push_back(imesh_vertex<3>( 1, 1,-1));
  verts->push_back(imesh_vertex<3>(-1, 1,-1));
  verts->push_back(imesh_vertex<3>(-1,-1,-1));
  verts->push_back(imesh_vertex<3>( 1,-1,-1));
  vcl_auto_ptr<imesh_vertex_array_base> v(verts);
  cube.set_vertices(v);

  imesh_regular_face_array<4>* faces = new imesh_regular_face_array<4>();
  faces->push_back(imesh_quad(0,1,2,3));
  faces->push_back(imesh_quad(0,4,5,1));
  faces->push_back(imesh_quad(1,5,6,2));
  faces->push_back(imesh_quad(2,6,7,3));
  faces->push_back(imesh_quad(3,7,4,0));
  faces->push_back(imesh_quad(7,6,5,4));
  vcl_auto_ptr<imesh_face_array_base> f(faces);
  cube.set_faces(f);
}

