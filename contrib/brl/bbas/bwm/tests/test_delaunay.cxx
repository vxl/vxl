//:
// \file
// \brief   testing the delaunay triangulation method
// \author  Gamze Tunali
// \date    2007-06-10
//
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <testlib/testlib_test.h>

#include <bwm/algo/bwm_delaunay_tri.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

void test_delaunay()
{
  vcl_vector<vgl_point_3d<double> > pxyz;
  vcl_vector<vgl_point_2d<int> > edges;
  vcl_vector<vgl_point_3d<int> > v;
  int ntri;

  pxyz.push_back(vgl_point_3d<double>(0,0,0));
  pxyz.push_back(vgl_point_3d<double>(0,10,0));
  pxyz.push_back(vgl_point_3d<double>(5,5,0));
  pxyz.push_back(vgl_point_3d<double>(10,0,0));
  pxyz.push_back(vgl_point_3d<double>(10,10,0));

  bwm_delaunay_tri tri;
  tri.triangulate(pxyz, v, ntri);

  vcl_cout << "Number of Triangles:" << ntri << vcl_endl;
  for (int i=0; i<ntri; i++) {
    vcl_cout << i << "==>" << v[i].x() << ' ' << v[i].y() << ' ' << v[i].z() << vcl_endl;
  }

  vcl_cout << "Number of Edges:" << edges.size() << vcl_endl;
  for (unsigned int i=0; i<edges.size(); i++) {
    vcl_cout << i << "==>" << edges[i] << vcl_endl;
  }
}

TESTMAIN(test_delaunay);
