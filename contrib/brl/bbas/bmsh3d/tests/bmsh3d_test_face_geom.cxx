// This is brl/bbas/bmsh3d/tests/bmsh3d_test_face_geom.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Ming-Ching Chang
// \date Nov 28, 2006.
//  To debug in Visual studio, put argument
//  all D:\Projects\vxl\brl\bbas\bmsh3d\tests\data

#include <vnl/vnl_math.h>

#include <bmsh3d/bmsh3d_mesh.h>


MAIN (bmsh3d_test_face_geom)
{
  testlib_test_start("bmsh3d_test_face_geom.");

  //Create the test mesh.
  bmsh3d_mesh* M = new bmsh3d_mesh;

  bmsh3d_vertex* v[3];

  v[0] = M->_new_vertex ();
  v[0]->get_pt().set (1.234, 234, 64); //(0, 0, 0);
  M->_add_vertex (v[0]);

  v[1] = M->_new_vertex ();
  v[1]->get_pt().set (657, 23.2, -34); //(0, 1, 0);
  M->_add_vertex (v[1]);

  v[2] = M->_new_vertex ();
  v[2]->get_pt().set (0.54345, -238, -34.23); //(2, 1, 0);
  M->_add_vertex (v[2]);

  bmsh3d_edge* e01 = M->_new_edge (v[0], v[1]);
  M->add_edge_incidence (e01);

  bmsh3d_edge* e12 = M->_new_edge (v[1], v[2]);
  M->add_edge_incidence (e12);

  bmsh3d_edge* e02 = M->_new_edge (v[0], v[2]);
  M->add_edge_incidence (e02);

  bmsh3d_face* f0 = M->_new_face ();
  f0->connect_E_to_end (e01);
  f0->connect_E_to_end (e12);
  f0->connect_E_to_end (e02);
  M->_add_face (f0);

  //Test the 2D polygon of the face f0
  vcl_vector<double> xs, ys;
  vcl_vector<bmsh3d_vertex*> vertices;
  f0->get_ordered_Vs (vertices);
  get_2d_polygon (vertices, xs, ys);

  //Validate if the 2D polygon has the same vertex-distance as the 3D polygon.
  int size = (int) vertices.size();
  for (int i=0; i<size; i++) {
    int j = (i+1) % size;

    double d2 = vnl_math_hypot (xs[i]-xs[j], ys[i]-ys[j]);
    bmsh3d_vertex* vi = vertices[i];
    bmsh3d_vertex* vj = vertices[j];
    double d3 = vgl_distance (vi->pt(), vj->pt());

    TEST_NEAR ("d2, d3 of v[i], v[j]", d2, d3, 1E-10);
  }


  delete M;

  return testlib_test_summary();
}
