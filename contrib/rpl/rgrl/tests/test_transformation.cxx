//:
// \file
#include <vcl_iostream.h>

#include <testlib/testlib_test.h>

#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_trans_spline.h>
#include <rgrl/rgrl_trans_rigid.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

#include "test_util.h"

namespace {

void
test_trans_affine()
{
  vnl_matrix<double> A( 3, 3 );
  vnl_vector<double> t( 3 );
  vnl_matrix<double> covar( 12, 12 );

  A(0,0) = 2.0;  A(0,1) = 4.0;  A(0,2) = 1.0;
  A(1,0) = 1.0;  A(1,1) =-1.0;  A(1,2) = 5.0;
  A(2,0) = 3.0;  A(2,1) = 2.0;  A(2,2) = 2.0;

  t[0] =  3.0;
  t[1] = -4.0;
  t[2] =  1.0;

  covar.set_identity();

  rgrl_transformation_sptr xform = new rgrl_trans_affine( A, t, covar );
  TEST("Construct 3D affine transform object", !xform, false);

  testlib_test_begin( "Transform 3D location" );

  vnl_vector<double> point( 3 );
  point[0] = 3.0;
  point[1] = 2.0;
  point[2] = 1.0;

  vnl_vector<double> true_point( 3 );
  //true_point = A * point + t;  // should specify manually?
  true_point[0] = 18.0;
  true_point[1] = 2.0;
  true_point[2] = 16.0;

  vnl_vector<double> xformed_point( 3 );

  xform->map_location( point, xformed_point );
  testlib_test_perform( xformed_point == true_point );
  TEST( "Transform 3D location, 2nd interface", xform->map_location(point), true_point );

  testlib_test_begin( "Transform 3D direc" );

  vnl_vector<double> direction( 3 );
  direction[0] =  5.0;
  direction[1] =  2.0;
  direction[2] = -3.0;
  direction.normalize();

  vnl_vector<double> true_direction( 3 );
  true_direction = A * direction;  // should specify manually?
  true_direction.normalize();

  vnl_vector<double> xformed_direction( 3 );

  // for the affine, the location is irrelevant when transforming a
  // direction.
  //
  xform->map_direction( point, direction, xformed_direction );
  testlib_test_perform( xformed_direction == true_direction );

  TEST( "Covariance is as given", rgrl_cast<rgrl_trans_affine*>(xform)->covar(), covar );
  testlib_test_begin( "Inverse_map points" );
  // Test the inv_map functions
  vnl_vector<double> inv_map_pt(3);
  xform->inv_map(true_point, inv_map_pt);
  testlib_test_perform( close(inv_map_pt, point, 1e-8) );

  vnl_vector<double> point2(3), next_inv_map(3), delta_to(3), inv_map_pt2(3);
  point2[0] = 5.0;
  point2[1] = 2.0;
  point2[2] = -2.0;

  vnl_vector<double> true_point2(3);
  true_point2[0] = 19;
  true_point2[1] = -11;
  true_point2[2] = 16;

  xform->inv_map(true_point2, false, delta_to, inv_map_pt2, next_inv_map);
  testlib_test_begin( "Inverse_map points, 2nd interface" );
  testlib_test_perform( close(inv_map_pt2, point2, 1e-8) );
}

void
test_trans_quadratic()
{
  vnl_matrix<double> Q(2,3);
  vnl_matrix<double> A( 2, 2 ), uc_A(2,2);
  vnl_vector<double> t( 2 ), uc_tt( 2 );
  vnl_matrix<double> covar( 12, 12 );

  Q(0,0) = 1.0;  Q(0,1) = 0; Q(0,2) = 0.5;
  Q(1,0) = 0.05; Q(1,1) = 1; Q(1,2) = 0.001;

  A(0,0) = 2.0;  A(0,1) = 4.0;
  A(1,0) = 1.0;  A(1,1) =-1.0;

  t[0] =  3.0;
  t[1] = -4.0;

  // The same transform, centered at from_center(4,10) and to_center(-5,30)

  uc_A(0,0) = -8-5+2;
  uc_A(0,1) = -2+4;
  uc_A(1,0) = -0.4-0.01+1;
  uc_A(1,1) = -20-0.004-1;

  uc_tt[0] =  16+20-8-40+3-5;
  uc_tt[1] =  0.05*16+100+0.04-4+10-4+30;

  vnl_vector<double> from_center(2), to_center(2);
  from_center[0] = 4; from_center[1] = 10;
  to_center[0] = -5; to_center[1] = 30;

  testlib_test_begin( "Construct 2D quadratic transform object" );
  rgrl_transformation_sptr xform = new rgrl_trans_quadratic( Q, A, t, covar );
  testlib_test_perform( true );

  testlib_test_begin( "Transform 2D location" );

  vnl_vector<double> point( 2 );
  point[0] = 3.0;
  point[1] = 2.0;

  vnl_vector<double> true_point(2);
  true_point[0] = 29;
  true_point[1] = 1.4560;

  vnl_vector<double> xformed_point( 2 );

  xform->map_location( point, xformed_point );
  testlib_test_perform( close(xformed_point, true_point) );
  testlib_test_begin("Transform 2D location, 2nd interface");
  testlib_test_perform( close(xform->map_location(point), true_point) );

  testlib_test_begin( "Transform 2D direc" );

  vnl_vector<double> direction( 2 );
  direction[0] =  5.0;
  direction[1] =  2.0;
  direction.normalize();

  vnl_vector<double> true_direction( 2 );
  true_direction[0] = 0.9768;
  true_direction[1] = 0.2141;
  true_direction.normalize();

  vnl_vector<double> xformed_direction( 2 );
  xform->map_direction( point, direction, xformed_direction );
  testlib_test_perform( close(xformed_direction,true_direction,  1e-4) );

  testlib_test_begin( "Inverse map point" );
  vnl_vector<double> next_inv_map(2), delta_to(2), inv_map_pt(2);
  inv_map_pt[0] = 50.0;
  inv_map_pt[1] = 100.0;
  xform->inv_map(true_point, false, delta_to, inv_map_pt, next_inv_map);
  testlib_test_perform(  close(point, inv_map_pt, 1e-2) );

  //test the constructor with input xform as centered
  testlib_test_begin( "Construct 2D quadratic transform object with centered xform" );
  rgrl_transformation_sptr xform2 = new rgrl_trans_quadratic( Q, A, t, covar, from_center, to_center);
  testlib_test_perform( rgrl_cast<rgrl_trans_quadratic*>(xform2)->Q() == Q
                        && close(rgrl_cast<rgrl_trans_quadratic*>(xform2)->A(),uc_A)
                        && close(rgrl_cast<rgrl_trans_quadratic*>(xform2)->t(),uc_tt) );
}

void
test_trans_spline()
{
  // 1D spline
  {
    vcl_cout << "test 1D spline transformation with affine\n";
    vcl_vector< rgrl_spline_sptr > splines;
    rgrl_spline_sptr spline = new rgrl_spline( vnl_vector< unsigned > ( 1, 1 ) );
    vnl_vector< double > c( spline->num_of_control_points() );
#if 0
    vnl_random randomizer;
    randomizer.reseed();
    for ( unsigned i=0; i<c.size(); ++i )
      c[i] = randomizer.drand32( 0, 10 );
#else
    c[0] = 1.0;
    c[1] = 2.0;
    c[2] = 2.0;
    c[3] = 1.0;
#endif // 0
    spline->set_control_points( c );
    splines.push_back( spline );
    vnl_vector< double > x0( 1, 0.0 );
    vnl_vector< double > delta( 1, 1.0 );

    vnl_matrix< double > A( 1, 1, 2.0 );
    vnl_vector< double > T( 1, 1.0 );
    rgrl_transformation_sptr affine = new rgrl_trans_affine( A, T );
    rgrl_trans_spline trans( splines, x0, delta, affine );

    vnl_vector< double > from( 1, 0.5 );
    vnl_vector< double > to( 1 );
    vnl_vector< double > mapped_to( 1, 11.75/6+0.5*2+1 );
    trans.map_location( from, to );
    vcl_cout << "mapped to = " << to << '\n';
    TEST( "test map_location()", (mapped_to-to).two_norm() < 1e-5, true );
  }
}

//: A test for the rigid transformation
void test_trans_rigid()
{
  // the tolerance for angle extraction
  double tol = 1e-10;

  vnl_random randomizer;
  randomizer.reseed();

  bool status3 = true, status2 = true;

  bool statusdet = true;

  const int NUM_TRIALS = 1000;

  for (int qqq=0; qqq<NUM_TRIALS; qqq++)
  {
    double phi = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);
    double alpha = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);
    double theta = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);

    double dphi,dalpha,dtheta;

    rgrl_trans_rigid rigid3(3);
    rgrl_trans_rigid rigid2(2);

    rigid3.set_rotation(theta,alpha,phi);
    rigid3.determine_angles(dphi,dalpha,dtheta);

    vnl_matrix<double> R = rigid3.R();

    double det = vnl_determinant(R);
    if (! det == 1.0 )
    {
      vcl_cout<<"Failing with determinant test\n"
              <<"Determinant is "<<det<<vcl_endl;
      statusdet = false;
    }

    if (!( vnl_math_abs(phi-dphi)< tol     &&
           vnl_math_abs(theta-dtheta)< tol &&
           vnl_math_abs(alpha-dalpha)< tol ))
    {
      testlib_test_perform(false);
      vcl_cout<<"Failing for this case:\n"
              <<"initial   phi,alpha,theta = "<<phi<<' '<<alpha<<' '<<theta<<vcl_endl
              <<"extracted phi,alpha,theta = "<<dphi<<' '<<dalpha<<' '<<dtheta<<vcl_endl;
      status3 = false;
    }

    rigid2.set_rotation(theta);
    rigid2.determine_angles(dtheta);
    if (! (vnl_math_abs(theta-dtheta)< tol ) )
    {
      testlib_test_perform(false);
      vcl_cout<<"Failing for this case:\n"
              <<"initial   theta = "<<theta<<vcl_endl
              <<"extracted theta = "<<dtheta<<vcl_endl;
      status2 = false;
    }
  }
  if (status3)
  {
    testlib_test_begin(" Extracting angles from 3D rotation matrix ");
    testlib_test_perform(true);
  }
  if (status2)
  {
    testlib_test_begin(" Extracting angles from 2D rotation matrix ");
    testlib_test_perform(true);
  }

  testlib_test_begin(" Determinant of 3d rotation is unity ");
  testlib_test_perform(statusdet);

  vnl_matrix<double> A;

  double phi = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);
  double alpha = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);
  double theta = randomizer.drand32(-vnl_math::pi/2,vnl_math::pi/2);

  double ttxx = randomizer.drand32(-1000,1000);
  double ttyy = randomizer.drand32(-1000,1000);
  double ttzz = randomizer.drand32(-1000,1000);

  rgrl_transformation_sptr xform = new rgrl_trans_rigid(3);
  TEST("Construct 3D rigid transform object", !xform, false);

  rgrl_trans_rigid* rigid = rgrl_cast<rgrl_trans_rigid*>(xform);
  rigid->set_rotation(theta,alpha,phi);
  rigid->set_translation(ttxx,ttyy,ttzz);

  A = rigid->R();

  double dphi,dalpha,dtheta;

  rigid->determine_angles(dphi,dalpha,dtheta);

  testlib_test_begin(" Extracted angles are close to real angles ");
  testlib_test_perform( vnl_math_abs(phi-dphi)< tol && vnl_math_abs(theta-dtheta)< tol && vnl_math_abs(alpha-dalpha)< tol );

  testlib_test_begin( "Transform 3D location" );

  vnl_vector<double> point( 3 );
  point[0] = 3.0;
  point[1] = 2.0;
  point[2] = 1.0;

  vnl_vector<double> t = rigid->t();

  vnl_vector<double> xformed_point( 3 );
  xform->map_location( point, xformed_point );
  testlib_test_perform( xformed_point == (A*point+t) );

  TEST( "Transform 3D location, 2nd interface", xform->map_location(point), A*point+t );

  testlib_test_begin( "Transform 3D direction" );

  vnl_vector<double> direction( 3 );
  direction[0] =  5.0;
  direction[1] =  2.0;
  direction[2] = -3.0;
  direction.normalize();

  vnl_vector<double> xformed_direction( 3 );

  xform->map_direction( point, direction, xformed_direction );
  testlib_test_perform( close(xformed_direction, A*direction,1e-6));

  testlib_test_begin( "inv_map and map_location are inverses" );

  // Test the inv_map functions
  vnl_vector<double> inv_map_pt(3),inv_inv_map_pt(3);
  xform->inv_map(point, inv_map_pt);
  xform->map_location(inv_map_pt,inv_inv_map_pt);
  testlib_test_perform( close(point, inv_inv_map_pt, 1e-8) );
}

} // end anonymous namespace

MAIN( test_transformation )
{
  START( "various transformation objects" );

  test_trans_affine();
  test_trans_quadratic();
  test_trans_spline();
  test_trans_rigid();

  SUMMARY();
}
