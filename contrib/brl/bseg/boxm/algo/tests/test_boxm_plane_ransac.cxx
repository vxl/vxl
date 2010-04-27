//:
// \file
// \author Gamze Tunali
// \brief  this code tests the function boxm_plane_ransac.
//  Creates 10 planes that the same line goes through and 10 random planes
//  which do not contain the plane. It makes sure it returns only the the
//  first 10 planes as the fittest ones.
//
// \date   March 25, 2010

#include <testlib/testlib_test.h>
#include <boxm/algo/boxm_plane_ransac.h>
#include <boxm/boxm_plane_obs.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vnl/vnl_random.h>
#include <vcl_vector.h>

static const unsigned int test_num=10;
static const unsigned int test_set=20;

static void test_boxm_plane_ransac()
{
  // create 10 planes that the line go through and 10 random planes
  vgl_point_3d<float> p1(20,20,10);
  vgl_point_3d<float> p2(30,20,10);
  vgl_infinite_line_3d<float> line(p1,p2);

  vcl_vector<boxm_edge_tangent_sample<float> > planes;
  vnl_random rand;
  // get two points from the line, and create a random 3rd point to define a plane
  unsigned i;
  for (i=0; i<test_num; i++) {
    float x=float(rand.drand32()*100);
    float y=float(rand.drand32()*100);
    float z=float(rand.drand32()*100);
    vgl_point_3d<float> p3(x,y,z);
    vgl_plane_3d<float> plane(p1,p2,p3);
    boxm_plane_obs<float> obs(plane,1);
    boxm_edge_tangent_sample<float> sample;
    sample.insert(obs);
    planes.push_back(sample);
  }

  // add the random planes
  for (; i<test_set; i++) {
    float a=float(rand.drand32()*100);
    float b=float(rand.drand32()*100);
    float c=float(rand.drand32()*100);
    float d=float(rand.drand32()*100);
    vgl_plane_3d<float> plane(a,b,c,d);
    boxm_plane_obs<float> obs(plane,1);
    boxm_edge_tangent_sample<float> sample;
    sample.insert(obs);
    planes.push_back(sample);
  }

  vcl_vector<float> weights(test_set,1);
  int threshold=2;
  float residual;
  vgl_infinite_line_3d<float> l;
  vgl_box_3d<double> cell_global_box(0,0,0,1,1,1);
  boxm_plane_ransac<float>(planes, weights, l, residual, cell_global_box, threshold);

  bool good=(line==l);

  TEST("test_boxm_plane_ransac: found the right plane set", good, true);
}

TESTMAIN(test_boxm_plane_ransac);
