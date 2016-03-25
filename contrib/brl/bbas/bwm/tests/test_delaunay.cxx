//:
// \file
// \brief   testing the delaunay triangulation method
// \author  Gamze Tunali
// \date    2007-06-10
//
#include <iostream>
#include <vector>
#include <vcl_compiler.h>
#include <testlib/testlib_test.h>

#include <bwm/algo/bwm_delaunay_tri.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

void test_delaunay()
{
  std::vector<vgl_point_3d<double> > pxyz;
  std::vector<vgl_point_2d<int> > edges;
  std::vector<vgl_point_3d<int> > v;
  int ntri;

  pxyz.push_back(vgl_point_3d<double>(0,0,0));
  pxyz.push_back(vgl_point_3d<double>(0,10,0));
  pxyz.push_back(vgl_point_3d<double>(5,5,0));
  pxyz.push_back(vgl_point_3d<double>(10,0,0));
  pxyz.push_back(vgl_point_3d<double>(10,10,0));

  bwm_delaunay_tri tri;
  tri.triangulate(pxyz, v, ntri);

  std::cout << "Number of Triangles:" << ntri << std::endl;
  for (int i=0; i<ntri; i++) {
    std::cout << i << "==>" << v[i].x() << ' ' << v[i].y() << ' ' << v[i].z() << std::endl;
  }

  std::cout << "Number of Edges:" << edges.size() << std::endl;
  for (unsigned int i=0; i<edges.size(); i++) {
    std::cout << i << "==>" << edges[i] << std::endl;
  }
}

TESTMAIN(test_delaunay);
