// This is mul/vimt/tests/test_transform_2d.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vgl/vgl_distance.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vimt/vimt_transform_2d.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

static void test_product(const vimt_transform_2d& t0, const vimt_transform_2d& t1)
{
  vimt_transform_2d t01 = t0*t1;
  vgl_point_2d<double> p0(1,2);
  vgl_point_2d<double> q1=t0(t1(p0));
  vgl_point_2d<double> q2=t01(p0);
  TEST_NEAR("Testing product",vgl_distance(q1,q2),0.0,1e-6);
}

static void test_products(const vimt_transform_2d& trans1)
{
  vimt_transform_2d trans0;
  trans0.set_identity();
  vcl_cout<<"Pre-multiply with Identity\n";
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Translation\n";
  trans0.set_translation(1.2, 3.4);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with ZoomOnly\n";
  trans0.set_zoom_only(2, 3, -4);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with RigidBody\n";
  trans0.set_rigid_body(1.5, 3.4, 5.6);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Sivimtarity\n";
  trans0.set_similarity(2,2,3,4);
  test_product(trans0,trans1);

   vcl_cout<<"Pre-multiply with Affine\n";
   vnl_matrix<double> A(2,3);
   for (int i=0;i<2;++i)
     for (int j=0;j<3;++j) A(i,j)=3+i-j*j;
   trans0.set_affine(A);
   test_product(trans0,trans1);

   vcl_cout<<"Pre-multiply with Projective\n";
   vnl_matrix<double> P(3,3);
   P(0,0)=3;  P(0,1)=1; P(0,2)=1;
   P(1,0)=-1; P(1,1)=1; P(1,2)=3;
   P(2,0)=1;  P(2,1)=1; P(2,2)=2;
   trans0.set_projective(P);
   test_product(trans0,trans1);
}

static void test_the_transform(vimt_transform_2d& t)
{
   vimt_transform_2d t_inv = t.inverse();

   vgl_point_2d<double> p0(5,7);
   vgl_point_2d<double> p1 = t(p0);
   TEST_NEAR("Inverse correct",vgl_distance(t_inv(p1),p0),0.0,1e-6);

   vnl_matrix<double> I = t.matrix() * t_inv.matrix();
   vnl_matrix<double> I0(3,3);
   I0.fill(0);
   I0.fill_diagonal(1);

   TEST_NEAR("matrix() correct",(I-I0).absolute_value_max(),0.0,1e-6);

   test_products(t);
}

static void test_transform_2d()
{
  vcl_cout << "***************************\n"
           << " Testing vimt_transform_2d\n"
           << "***************************\n";

  vimt_transform_2d trans0;
  vgl_point_2d<double> p0(1,2),p1;

  trans0.set_identity();
  vcl_cout<<"== Testing Identity ==\n";
  TEST_NEAR("Identity transform",vgl_distance(trans0(p0),p0),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Translation ==\n";
  trans0.set_translation(1.2, 3.4);
  p1 = vgl_point_2d<double>(2.2,5.4);
  TEST_NEAR("Translation transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing ZoomOnly ==\n";
  trans0.set_zoom_only(2, 3, 4);
  p1 = vgl_point_2d<double>(5,8);
  TEST_NEAR("Zoom transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"== Testing RigidBody ==\n";
  trans0.set_rigid_body(1.2, 3.4, 5.6);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Sivimtarity ==\n";
  trans0.set_similarity(1,2,3,4);
  test_the_transform(trans0);

   vcl_cout<<"== Testing Affine ==\n";
   vnl_matrix<double> A(2,3);
   for (int i=0;i<2;++i)
     for (int j=0;j<3;++j) A(i,j)=2+i-j*j;
   trans0.set_affine(A);
   test_the_transform(trans0);

   vcl_cout<<"== Testing Projective ==\n";
   vnl_matrix<double> P(3,3);
   P(0,0)=2;  P(0,1)=1; P(0,2)=1;
   P(1,0)=-1; P(1,1)=1; P(1,2)=3;
   P(2,0)=1;  P(2,1)=1; P(2,2)=2;
   trans0.set_projective(P);
   test_the_transform(trans0);


  // -------- Test the binary I/O --------

  vsl_b_ofstream bfs_out("test_transform_2d.bvl.tmp");
  TEST("Created test_transform_2d.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, trans0);
  bfs_out.close();

  vimt_transform_2d trans0_in;

  vsl_b_ifstream bfs_in("test_transform_2d.bvl.tmp");
  TEST("Opened test_transform_2d.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, trans0_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_transform_2d.bvl.tmp");
#endif

  vnl_vector<double> v0,v0_in;
  trans0.params(v0);
  trans0_in.params(v0_in);
  TEST("Binary IO for form", trans0.form(), trans0_in.form());
  TEST_NEAR("Binary IO for params", (v0-v0_in).magnitude(),0.0,1e-6);
}

TESTMAIN(test_transform_2d);
