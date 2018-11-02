//:
// \file
// \author Gamze Tunali
// \brief  this code tests the function boxm_plane_ransac.
//  Creates 10 planes that the same line goes through and 10 random planes
//  which do not contain the plane. It makes sure it returns only the
//  first 10 planes as the fittest ones.
//
// \date   March 25, 2010

#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#include <boxm/util/boxm_plane_ransac.h>
#include <boxm/sample/boxm_plane_obs.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const unsigned int test_num=10;
static const unsigned int test_set=10;

static void test_boxm_plane_ransac()
{
  // create 10 planes that the line go through and 10 random planes
  vgl_point_3d<float> p1(20,20,10);
  vgl_point_3d<float> p2(30,20,10);
  vgl_infinite_line_3d<float> line(p1,p2);

  std::vector<boxm_edge_tangent_sample<float> > planes;
  vnl_random rand;
  // get two points from the line, and create a random 3rd point to define a plane

  for (unsigned i=0; i<test_num; ++i) {
    auto x=float(rand.drand32()*100);
    auto y=float(rand.drand32()*100);
    auto z=float(rand.drand32()*100);
    vgl_point_3d<float> p3(x,y,z);
    vgl_plane_3d<float> plane(p1,p2,p3);
    boxm_plane_obs<float> obs(plane,1);
    boxm_edge_tangent_sample<float> sample;
    sample.insert(obs);
    planes.push_back(sample);
  }

  // add the random planes
  for (unsigned i=0; i<test_set; ++i) {
    auto a=float(rand.drand32()*100);
    auto b=float(rand.drand32()*100);
    auto c=float(rand.drand32()*100);
    auto d=float(rand.drand32()*100);
    vgl_plane_3d<float> plane(a,b,c,d);
    boxm_plane_obs<float> obs(plane,1);
    boxm_edge_tangent_sample<float> sample;
    sample.insert(obs);
    planes.push_back(sample);
  }

  std::cout << " planes size: " << planes.size() << std::endl;

  std::vector<float> weights(test_set,1);
  int threshold=2;
  float residual;
  vgl_infinite_line_3d<float> l;
  vgl_box_3d<double> cell_global_box(18,15,5,32,25,15);
  float ortho_thres = 0.01f;
  float volume_ratio = 128;
  boxm_plane_ransac<float>(planes, weights, l, residual, cell_global_box, threshold, ortho_thres, volume_ratio);

  TEST_NEAR("test_boxm_plane_ransac: found the right plane set",
            (l.point()-line.point()).sqr_length() + (l.direction()-line.direction()).sqr_length(),
            0.0f, 0.01f);
}

TESTMAIN(test_boxm_plane_ransac);
