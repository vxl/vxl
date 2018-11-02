// This is mul/vimt/tests/test_transform_2d.cxx
#include <iostream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <vgl/vgl_distance.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
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
  std::cout<<"Pre-multiply with Identity\n";
  test_product(trans0,trans1);

  std::cout<<"Pre-multiply with Translation\n";
  trans0.set_translation(1.2, 3.4);
  test_product(trans0,trans1);

  std::cout<<"Pre-multiply with ZoomOnly\n";
  trans0.set_zoom_only(2, 3, -4);
  test_product(trans0,trans1);

  std::cout<<"Pre-multiply with RigidBody\n";
  trans0.set_rigid_body(1.5, 3.4, 5.6);
  test_product(trans0,trans1);

  std::cout<<"Pre-multiply with Sivimtarity\n";
  trans0.set_similarity(2,2,3,4);
  test_product(trans0,trans1);

   std::cout<<"Pre-multiply with Affine\n";
   vnl_matrix<double> A(2,3);
   for (int i=0;i<2;++i)
     for (int j=0;j<3;++j) A(i,j)=3+i-j*j;
   trans0.set_affine(A);
   test_product(trans0,trans1);

   std::cout<<"Pre-multiply with Projective\n";
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

static void test_simplify(const vimt_transform_2d& input, const vimt_transform_2d& expected)
{
  std::cout << "Testing Simplify\n";

  vimt_transform_2d copy(input);
  copy.simplify();

  TEST("Expected form", copy.form(), expected.form());
  TEST_NEAR("Expected matrix", (copy.matrix()-expected.matrix()).fro_norm(), 0, 1e-12);
}

//=========================================================================
// Test the set_matrix(matrix) function
static void test_set_matrix()
{
  double tx=1.0, ty=2.0;
  double r=0.1;
  double sx=0.9, sy=1.0;

  vnl_matrix<double> M,N;
  vimt_transform_2d T;
  vimt_transform_2d::Form form;

  // rigid_body
  form = vimt_transform_2d::RigidBody;
  vimt_transform_2d R;
  R.set_rigid_body(r, tx, ty).matrix(M);
  T.set_matrix(M).simplify().matrix(N);
  TEST_NEAR("set_matrix and simplify of RigidBody", (M-N).fro_norm(), 0, 1e-12);
  TEST("set_matrix and simplify of RigidBody", T.form(), form);

  // zoom_only
  form = vimt_transform_2d::ZoomOnly;
  vimt_transform_2d Z;
  Z.set_zoom_only(sx, sy, tx, ty).matrix(M);
  T.set_matrix(M).simplify().matrix(N);
  TEST_NEAR("set_matrix and simplify of ZoomOnly", (M-N).fro_norm(), 0, 1e-12);
  TEST("set_matrix and simplify of ZoomOnly", T.form(), form);

  // similarity
  form = vimt_transform_2d::Similarity;
  vimt_transform_2d S;
  S.set_similarity(sx, r, tx, ty).matrix(M);
  T.set_matrix(M).simplify().matrix(N);
  TEST_NEAR("set_matrix and simplify of Similarity", (M-N).fro_norm(), 0, 1e-12);
  TEST("set_matrix and simplify of Similarity", T.form(), form);

  // affine
  form = vimt_transform_2d::Affine;
  vimt_transform_2d A;
  A.set_affine(vgl_point_2d<double>(tx, ty),
               sx*vgl_vector_2d<double>(std::cos(r), std::cos(r)),
               sy*vgl_vector_2d<double>(-std::sin(r), std::cos(r)) ).matrix(M);
  T.set_matrix(M).simplify().matrix(N);
  TEST_NEAR("set_matrix and simplify of Affine", (M-N).fro_norm(), 0, 1e-12);
  TEST("set_matrix and simplify of Affine", T.form(), form);
}


static void test_transform_2d()
{
  std::cout << "***************************\n"
           << " Testing vimt_transform_2d\n"
           << "***************************\n";

  vimt_transform_2d trans0, trans1, trans2;
  vgl_point_2d<double> p0(1,2),p1;

  trans0.set_identity();
  std::cout<<"== Testing Identity ==\n";
  TEST_NEAR("Identity transform",vgl_distance(trans0(p0),p0),0.0,1e-8);
  test_the_transform(trans0);
  test_simplify(trans0, trans0);

  std::cout<<"== Testing Translation ==\n";
  trans0.set_translation(1.2, 3.4);
  p1 = vgl_point_2d<double>(2.2,5.4);
  TEST_NEAR("Translation transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  trans0.set_translation(0, 0);
  trans1.set_identity();
  test_simplify(trans1, trans2);

  std::cout<<"== Testing ZoomOnly ==\n";
  trans0.set_zoom_only(2, 3, 4);
  p1 = vgl_point_2d<double>(5,8);
  TEST_NEAR("Zoom transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  trans1.set_zoom_only(-1.0, 1.0, -30, -40);
  test_simplify(trans1, trans1);
  trans1.set_zoom_only(1.0, 1.0, -30, -40);
  trans2.set_translation(-30, -40);
  test_simplify(trans1, trans2);
  trans1.set_zoom_only(1.0, 1.0, 0, 0);
  trans2.set_identity();
  test_simplify(trans1, trans2);

  std::cout<<"== Testing RigidBody ==\n";
  trans0.set_rigid_body(1.2, 3.4, 5.6);
  test_the_transform(trans0);

  trans1.set_rigid_body(vnl_math::pi/4.0, -30, -40);
  test_simplify(trans1, trans1);
  trans1.set_rigid_body( 0, -30, -40);
  trans2.set_translation(-30, -40);
  test_simplify(trans1, trans2);
  trans1.set_rigid_body( 0, 0, 0);
  trans2.set_identity();
  test_simplify(trans1, trans2);

  std::cout<<"== Testing Similarity ==\n";
  trans0.set_similarity(1,2,3,4);
  test_the_transform(trans0);

  trans1.set_similarity(2.0, vnl_math::pi/4.0, -30, -40);
  test_simplify(trans1, trans1);
  trans1.set_similarity(-2.0, 0, -30, -40);
  trans2.set_zoom_only(-2.0, -30, -40);
  test_simplify(trans1, trans2);
  trans1.set_similarity(1.0, vnl_math::pi/4.0, -30, -40);
  trans2.set_rigid_body(vnl_math::pi/4.0, -30, -40);
  test_simplify(trans1, trans2);
  trans1.set_similarity(1.0, 0, -30, -40);
  trans2.set_translation(-30, -40);
  test_simplify(trans1, trans2);

  std::cout<<"== Testing Affine ==\n";
  vnl_matrix<double> A(2,3);
  for (int i=0;i<2;++i)
   for (int j=0;j<3;++j) A(i,j)=2+i-j*j;
  trans0.set_affine(A);
  test_the_transform(trans0);

  trans1.set_affine(vgl_point_2d<double>(-30, -40),
                    vgl_vector_2d<double>(-1.0, 0),
                    vgl_vector_2d<double>(0, -2.0));
  trans2.set_zoom_only(-1.0, -2.0, -30, -40);
  test_simplify(trans1, trans2);
  trans1.set_affine(vgl_point_2d<double>(-30, -40),
                    2.0 * vgl_vector_2d<double>(std::sqrt(0.5), std::sqrt(0.5)),
                    2.0 * vgl_vector_2d<double>(-std::sqrt(0.5), std::sqrt(0.5)) );
  trans2.set_similarity(2.0, vnl_math::pi/4.0, -30, -40);
  test_simplify(trans1, trans2);
  trans1.set_affine(vgl_point_2d<double>(-30, -40),
                    2.0 * vgl_vector_2d<double>(std::sqrt(0.5), std::sqrt(0.5)),
                    3.0 * vgl_vector_2d<double>(-std::sqrt(0.5), std::sqrt(0.5)) );
  test_simplify(trans1, trans1);
  trans1.set_affine(vgl_point_2d<double>(-30, -40),
                    vgl_vector_2d<double>(std::sqrt(0.75), 0.5),
                    vgl_vector_2d<double>(-0.5, std::sqrt(0.75)) );
#if 0
  trans1.set_affine(1.0, 1.0, 1.0, vnl_math::pi/4.0, vnl_math::pi/4.0, 0, -30, -40, -50);
#endif
  trans2.set_rigid_body(vnl_math::pi/6.0, -30, -40);
  test_simplify(trans1, trans2);
  trans1.set_affine(vgl_point_2d<double>(0,0),
                    vgl_vector_2d<double>(1.0, 0.0),
                    vgl_vector_2d<double>(0.0, 1.0));
  trans2.set_identity();
  test_simplify(trans1, trans2);

  std::cout<<"== Testing Projective ==\n";
  vnl_matrix<double> P(3,3);
  P(0,0)=2;  P(0,1)=1; P(0,2)=1;
  P(1,0)=-1; P(1,1)=1; P(1,2)=3;
  P(2,0)=1;  P(2,1)=1; P(2,2)=2;
  trans0.set_projective(P);
  test_the_transform(trans0);

  test_set_matrix();

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
