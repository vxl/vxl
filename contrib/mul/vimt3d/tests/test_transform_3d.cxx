// This is mul/vimt3d/tests/test_transform_3d.cxx
#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <testlib/testlib_test.h>

#ifndef LEAVE_IMAGES_BEHIND
#define LEAVE_IMAGES_BEHIND 0
#endif

static void test_product(const vimt3d_transform_3d& t0, const vimt3d_transform_3d& t1)
{
  vimt3d_transform_3d t01 = t0*t1;
  vgl_point_3d<double> p0(1,2,0);
  vgl_point_3d<double> q1=t0(t1(p0));
  vgl_point_3d<double> q2=t01(p0);
  TEST_NEAR("Testing product",vgl_distance(q1,q2),0.0,1e-6);
}

static void test_products(const vimt3d_transform_3d& trans1)
{
  vimt3d_transform_3d trans0;
  trans0.set_identity();
  vcl_cout<<"Pre-multiply with Identity\n";
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Translation\n";
  trans0.set_translation(1.2, 3.4,0);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with ZoomOnly\n";
  trans0.set_zoom_only(2, 3, -4, 0);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with RigidBody\n";
  trans0.set_rigid_body(1.4,0.9,1.1,1.5,3.4, 5.6);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Similarity\n";
  trans0.set_similarity(2,3,1,3,4,5,6);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Affine\n";
  trans0.set_affine(0.5,2,3,2,3,1.5, 3.4, 5.6,0.1);
  test_product(trans0,trans1);
}

static void test_the_transform(vimt3d_transform_3d& t)
{
   vimt3d_transform_3d t_inv = t.inverse();
   vgl_point_3d<double> p0(5,7,0);
   vgl_point_3d<double> p1 = t(p0);
   TEST_NEAR("Inverse correct",vgl_distance(t_inv(p1),p0),0.0,1e-6);

   vnl_matrix<double> I = t.matrix() * t_inv.matrix();
   vnl_matrix<double> I0(4,4);
   I0.fill(0);
   I0.fill_diagonal(1);

   TEST_NEAR("matrix() correct",(I-I0).absolute_value_max(),0.0,1e-6);

   test_products(t);
}

static void test_transform_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vimt3d_transform_3d\n"
           << "*****************************\n";

  vimt3d_transform_3d trans0;
  vgl_point_3d<double> p0(1,2,0),p1;

  trans0.set_identity();
  vcl_cout<<"== Testing Identity ==\n";
  TEST_NEAR("Identity transform",vgl_distance(trans0(p0),p0),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Translation ==\n";
  trans0.set_translation(1.2, 3.4,0);
  p1 = vgl_point_3d<double>(2.2,5.4,0);
  TEST_NEAR("Translation transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing ZoomOnly ==\n";
  trans0.set_zoom_only(2, 3, 4,0);
  p1 = vgl_point_3d<double>(5,8,0);
  TEST_NEAR("Zoom transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing RigidBody ==\n";
  trans0.set_rigid_body(1.2,1,5,1,2,3);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Similarity ==\n";
  trans0.set_similarity(0.51,2,3,4,0.2,1,-4);
  test_the_transform(trans0);

   vcl_cout<<"== Testing Affine ==\n";
   trans0.set_affine(0.2,-0.3,4,2,1,4,5,0.1,-0.21);
   test_the_transform(trans0);

  // -------- Test the binary I/O --------

  vsl_b_ofstream bfs_out("test_transform_3d.bvl.tmp");
  TEST("Created test_transform_3d.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, trans0);
  bfs_out.close();

  vimt3d_transform_3d trans0_in;

  vsl_b_ifstream bfs_in("test_transform_3d.bvl.tmp");
  TEST("Opened test_transform_3d.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, trans0_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink("test_transform_3d.bvl.tmp");
#endif

  vnl_vector<double> v0,v0_in;
  trans0.params(v0);
  trans0_in.params(v0_in);
  TEST("Binary IO for form", trans0.form(), trans0_in.form());
  TEST_NEAR("Binary IO for params", (v0-v0_in).magnitude(),0.0,1e-6);
}

TESTMAIN(test_transform_3d);
