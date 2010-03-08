//:
// \file
#include <testlib/testlib_test.h>
#include <bsta/bsta_kent.h>
#include <vgl/vgl_plane_3d.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>

//: Test bsta kent distribution
void test_kent()
{
  vgl_plane_3d<double> wpl0(0.0990,0.9901,0.0990,0.0);
  vgl_plane_3d<double> wpl1(0.9901,0.0990,0.0990,0.0);
  vgl_plane_3d<double> wpl2(0.7036,0.7036,0.0995,1);

  vcl_vector<vgl_plane_3d<double> > planes;
  planes.push_back(wpl0);
  planes.push_back(wpl1);
  planes.push_back(wpl2);

  bsta_kent<double> kent(planes);
}

TESTMAIN(test_kent);
