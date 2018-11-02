#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <imesh/algo/imesh_generate_mesh.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_generate_mesh()
{
  vgl_point_2d<double> p0(0.0, 0.0);
  vgl_point_2d<double> p1(1.0, 0.0);
  vgl_point_2d<double> p2(1.0, 10.0);
  vgl_point_2d<double> p3(0.0, 10.0);
  vgl_point_2d<double> p4(0.25, 5.0);
  vgl_point_2d<double> p5(0.75, 5.0);
  std::vector<vgl_point_2d<double> > c_hull;
  c_hull.push_back(p0);   c_hull.push_back(p1);
  c_hull.push_back(p2);   c_hull.push_back(p3);
  vgl_line_segment_2d<double> ls(p4, p5);
  std::vector<vgl_line_segment_2d<double> > segs;
  segs.push_back(ls);
  imesh_mesh mesh;
  imesh_generate_mesh_2d(c_hull, segs, mesh);
  TEST("num_verts, num_faces", mesh.num_verts()*mesh.num_faces(), 36);
  const imesh_face_array_base& faces = mesh.faces();
  TEST("face verts", faces(0,0)+faces(0,1)+faces(0,2), 7);
  vgl_point_2d<double> p6(0.25, 1);
  vgl_point_2d<double> p7(0.75, 1);
  vgl_line_segment_2d<double> l2(p5, p7);
  segs.push_back(l2);
  imesh_mesh mesh2;
  imesh_generate_mesh_2d(c_hull, segs,mesh2);
  unsigned nverts = mesh2.num_verts();
  unsigned ntris =  mesh2.num_faces();
  TEST("two segs", nverts*ntris, 56);
#if 0
  const imesh_regular_face_array<3>& tris =
    static_cast<const imesh_regular_face_array<3>&>(mesh2.faces());
  const imesh_vertex_array<2>& verts = mesh2.vertices<2>();
  std::cout << "verts\n";
  for (unsigned i = 0; i< nverts; ++i)
    std::cout << "v[" << i << "](" << verts[i][0] << ' ' << verts[i][1] << ")\n";
  std::cout << "faces\n";
  for (unsigned i = 0; i< ntris; ++i)
    std::cout << "f[" << i << "](" << tris[i][0]<< ' ' << tris[i][1]
             << ' ' << tris[i][2] << ")\n";
#endif
}

TESTMAIN(test_generate_mesh);
