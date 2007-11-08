// This is mul/vimt3d/tests/test_transform_3d.cxx
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vnl/vnl_math.h>
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


//=========================================================================
// Create separate scaling, rotation and translation transforms and compose.
// Check that this is consistent with creating the transform directly.
// NB This test fails for versions of Similarity and Affine transforms for
// versions of vimt3d_transform_3d.cxx prior to 1.13
//=========================================================================
static void test_composition_and_construction(
  vimt3d_transform_3d::Form form)
{
  if (form==vimt3d_transform_3d::Identity ||
      form==vimt3d_transform_3d::Translation)
    return;

  // Define some parameters for scaling, rotation and translations components.
  double sx=0.7, sy=1.3, sz=2.1;
  double rx=vnl_math::pi/18, ry=-vnl_math::pi/36, rz=vnl_math::pi/21.34;
  double tx=-3.7, ty=2.2, tz=5.1;

  // Create separate scaling, rotation and translation transforms
  vimt3d_transform_3d T, Sa, Si, R;
  Si.set_zoom_only(sx, 0, 0, 0);         // (isotropic) scaling only
  Sa.set_zoom_only(sx, sy, sz, 0, 0, 0); // (anisotropic) scaling only
  R.set_rigid_body(rx, ry, rz, 0, 0, 0); // rotation only
  T.set_translation(tx, ty, tz);

  // Compose appropriate components
  // Create the equivalent affine transform directly.
  vimt3d_transform_3d A;
  vimt3d_transform_3d B;
  switch (form)
  {
  case vimt3d_transform_3d::ZoomOnly:
    A = T * Sa;
    B.set_zoom_only(sx, sy, sz, tx, ty, tz);
    break;
  case vimt3d_transform_3d::RigidBody:
    A = T * R;
    B.set_rigid_body(rx, ry, rz, tx, ty, tz);
    break;
  case vimt3d_transform_3d::Similarity:
    A = T * R * Si;
    B.set_similarity(sx, rx, ry, rz, tx, ty, tz);
    break;
  case vimt3d_transform_3d::Affine:
    A = T * R * Sa;
    B.set_affine(sx, sy, sz, rx, ry, rz, tx, ty, tz);
    break;
  default:
    vcl_cerr << "test_composition_and_construction() is only intended for "
             << "forms ZoomOnly, RigidBody, Similarity, Affine\n";
    break;
  }

  // Test equivalence
  bool transf_ok = A==B;
  TEST("Construction and composition", transf_ok, true);
#ifndef NDEBUG
  if (!transf_ok)
  {
    vcl_cout << "Composition:\n";
    A.print_all(vcl_cout);
    vcl_cout << vcl_endl;

    vcl_cout << "Construction:\n";
    B.print_all(vcl_cout);
    vcl_cout << vcl_endl;
  }
#endif
}


//=========================================================================
// Series of tests to apply to a transform
//=========================================================================
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

   test_composition_and_construction(t.form());
}


//=========================================================================
// Special test for the set_affine(p,u,v,w) method
//=========================================================================
static void test_affine_puvw()
{
  // Define some parameters for scaling, rotation and translations components.
  double sx=0.7, sy=1.3, sz=2.1;
  double rx=vnl_math::pi/18, ry=-vnl_math::pi/36, rz=vnl_math::pi/21.34;
  double tx=-3.7, ty=2.2, tz=5.1;

  // Create an affine transform specifying scale, rotation, translation components
  vimt3d_transform_3d A;
  A.set_affine(sx, sy, sz, rx, ry, rz, tx, ty, tz);

  // Apply this transform to the default origin and axis vectors
  vgl_point_3d<double> p = A(0,0,0);
  vgl_vector_3d<double> u = A(1,0,0) - p;
  vgl_vector_3d<double> v = A(0,1,0) - p;
  vgl_vector_3d<double> w = A(0,0,1) - p;

  // Create an affine transform using p,u,v,w
  vimt3d_transform_3d B;
  B.set_affine(p, u, v, w);

  // Test equivalence - NB operator== is not good enough for this test.
  bool transf_ok = (A.matrix()-B.matrix()).absolute_value_max()<1e-8;
  TEST("test_affine_puvw()", transf_ok, true);
#ifndef NDEBUG
  if (!transf_ok)
  {
    vcl_cout << "set_affine(sx, sy, sz, rx, ry, rz, tx, ty, tz):\n";
    A.print_all(vcl_cout);
    vcl_cout << vcl_endl;

    vcl_cout << "set_affine(p, u, v, w):\n";
    B.print_all(vcl_cout);
    vcl_cout << vcl_endl;

    vnl_matrix<double> M = A.matrix();
    vnl_matrix<double> N = B.matrix();
    vcl_cout << "Max diff= " << (M-N).absolute_value_max() << vcl_endl;
  }
#endif
}


//=========================================================================
// Main testing function
//=========================================================================
static void test_transform_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vimt3d_transform_3d\n"
           << "*****************************\n";

  vimt3d_transform_3d trans0;
  vgl_point_3d<double> p0(1,2,0),p1;

  trans0.set_identity();
  vcl_cout<<"\n== Testing Identity ==\n";
  TEST_NEAR("Identity transform",vgl_distance(trans0(p0),p0),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"\n== Testing Translation ==\n";
  trans0.set_translation(1.2, 3.4,0);
  p1 = vgl_point_3d<double>(2.2,5.4,0);
  TEST_NEAR("Translation transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"\n== Testing ZoomOnly ==\n";
  trans0.set_zoom_only(2, 3, 4,0);
  p1 = vgl_point_3d<double>(5,8,0);
  TEST_NEAR("Zoom transform",vgl_distance(trans0(p0),p1),0.0,1e-8);
  test_the_transform(trans0);

  vcl_cout<<"\n== Testing RigidBody ==\n";
  trans0.set_rigid_body(1.2,1,5,1,2,3);
  test_the_transform(trans0);

  vcl_cout<<"\n== Testing Similarity ==\n";
  trans0.set_similarity(0.51,2,3,4,0.2,1,-4);
  test_the_transform(trans0);
  
  vcl_cout<<"\n== Testing Similarity Parameters ==\n";
  double s= 0.51;
  double r_x= 2.0, r_y=0.1, r_z=4.0;
  double t_x= 0.2, t_y=1.0, t_z=-4.0;
  vcl_cout<<"original parameters:-"<<vcl_endl;
  vcl_cout<<"s= "<<s<<vcl_endl;
  vcl_cout<<"r_x= "<<r_x<<vcl_endl;
  vcl_cout<<"r_y= "<<r_y<<vcl_endl;
  vcl_cout<<"r_z= "<<r_z<<vcl_endl;
  vcl_cout<<"t_x= "<<t_x<<vcl_endl;
  vcl_cout<<"t_y= "<<t_y<<vcl_endl;
  vcl_cout<<"t_z= "<<t_z<<vcl_endl;
  trans0.set_similarity( s, r_x, r_y, r_z, t_x, t_y, t_z);
  vnl_vector<double> trans0_vec;
  trans0.params( trans0_vec );
  vcl_cout<<"trans0_vec= "<<trans0_vec<<vcl_endl;
  vimt3d_transform_3d trans0_test;
  trans0_test.set( trans0_vec, trans0.form() );
  vcl_cout<<"trans0= "<<trans0<<vcl_endl;
  vcl_cout<<"trans0_test= "<<trans0_test<<vcl_endl;
  //TEST("Similarity Params", trans0_test== trans0, true);
  
  // check transforms agree when applied to basis vectors
  vgl_point_3d<double> vec_x(1,0,0);
  vgl_point_3d<double> vec_x0= trans0( vec_x );
  vcl_cout<<"vec_x0= "<<vec_x0<<vcl_endl;
  vgl_point_3d<double> vec_x0_test= trans0_test( vec_x );
  vcl_cout<<"vec_x0_test= "<<vec_x0_test<<vcl_endl;
  TEST("Test x basis vec", (vec_x0- vec_x0_test).length()< 1e-6, true);
  
  vgl_point_3d<double> vec_y(0,1,0);
  vgl_point_3d<double> vec_y0= trans0( vec_y );
  vcl_cout<<"vec_y0= "<<vec_y0<<vcl_endl;
  vgl_point_3d<double> vec_y0_test= trans0_test( vec_y );
  vcl_cout<<"vec_y0_test= "<<vec_y0_test<<vcl_endl;
  TEST("Test y basis vec", (vec_y0- vec_y0_test).length()< 1e-6, true);
  
  vgl_point_3d<double> vec_z(0,0,1);
  vgl_point_3d<double> vec_z0= trans0( vec_z );
  vcl_cout<<"vec_z0= "<<vec_z0<<vcl_endl;
  vgl_point_3d<double> vec_z0_test= trans0_test( vec_z );
  vcl_cout<<"vec_z0_test= "<<vec_z0_test<<vcl_endl;
  TEST("Test z basis vec", (vec_z0- vec_z0_test).length()< 1e-6, true);
  

  vcl_cout<<"\n== Testing Affine ==\n";
  trans0.set_affine(0.2,-0.3,4,2,1,4,5,0.1,-0.21);
  test_the_transform(trans0);
  test_affine_puvw();

  /*
  vcl_cout<<"\n== Testing Affine Parameters ==\n";
  trans0.set_affine(0.2,-0.3,4,2,1,4,5,0.1,-0.21);
  vcl_cout<<"trans0= "<<trans0<<vcl_endl;
  vnl_vector<double> trans0_affine_vec;
  trans0.params( trans0_affine_vec );
  vcl_cout<<"trans0_affine_vec= "<<trans0_affine_vec<<vcl_endl;
  vimt3d_transform_3d trans0_affine_test;
  trans0_affine_test.set( trans0_affine_vec, trans0.form() );
  vcl_cout<<"trans0= "<<trans0<<vcl_endl;
  vcl_cout<<"trans0_affine_test= "<<trans0_affine_test<<vcl_endl;
  //TEST("Similarity Params", trans0_test== trans0, true);
  vcl_abort();
  */

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
