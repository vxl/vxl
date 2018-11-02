//:
// \file
#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#include <bsta/bsta_kent.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_inverse.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Test bsta kent distribution
void test_kent()
{
  vgl_plane_3d<double> wpl0(0.0990,0.9901,0.0990,0.0);
  vgl_plane_3d<double> wpl1(0.9901,0.0990,0.0990,0.0);
  vgl_plane_3d<double> wpl2(0.7036,0.7036,0.0995,1);

  std::vector<vgl_plane_3d<double> > planes;
  planes.push_back(wpl0);
  planes.push_back(wpl1);
  planes.push_back(wpl2);

  vnl_matrix<double> X(3,3,0);
  for (auto plane : planes) {
    vgl_vector_3d<double> normal = plane.normal();
    vnl_matrix<double> n(1,3);
    n.put(0,0,normal.x());
    n.put(0,1,normal.y());
    n.put(0,2,normal.z());
    vnl_matrix<double> nt = n.transpose();
    X += nt*n;
  }
  X/=planes.size();
  std::cout << X << std::endl;
  vnl_matrix<double> X_inv = vnl_inverse(X);

  bsta_kent<double> kent(X_inv);

  // test the probability distribution
  bsta_kent<double> kent2(1.0, 0.0,
                          vnl_vector_fixed<double,3>(0,0,1),
                          vnl_vector_fixed<double,3>(0,0,1),
                          vnl_vector_fixed<double,3>(0,0,1));
  vnl_vector_fixed<double,3> v(0,0,1);
  double r = kent2.prob_density(v);
  TEST_NEAR("r", r, 0.5, 0.5);
}

TESTMAIN(test_kent);
