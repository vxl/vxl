#include <testlib/testlib_test.h>

#include <vnl/vnl_math.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_face_pt.h>

#include <vcl_iostream.h>

#include <rgrl/rgrl_trans_affine.h>


namespace {

void
test_feature_caster()
{
  vnl_vector<double> loc2d( 2 );
  loc2d[0] =  5.0;
  loc2d[1] = -4.0;

  rgrl_feature_point* pf2d = new rgrl_feature_point( loc2d );
  rgrl_feature_sptr fsptr = pf2d;
  testlib_test_begin( "feature sptr cast to 2D point feature" );
  testlib_test_perform( rgrl_feature_caster<rgrl_feature_point>(fsptr) == pf2d );
}

void
test_feature_point()
{
  vcl_cout << "Test normal point feature\n";

  vnl_vector<double> loc2d( 2 );
  loc2d[0] =  5.0;
  loc2d[1] = -4.0;

  vnl_vector<double> loc3d( 3 );
  loc3d[0] =  5.0;
  loc3d[1] = -4.0;
  loc3d[2] =  2.0;

  vnl_vector<double> loc4d( 4 );
  loc4d[0] =  2.0;
  loc4d[1] =  7.0;
  loc4d[2] = -1.0;
  loc4d[3] =  3.0;

  testlib_test_begin( "2D point feature" );
  rgrl_feature_sptr pf2d = new rgrl_feature_point( loc2d );
  testlib_test_perform( pf2d->is_type( rgrl_feature_point::type_id() ) &&
                        pf2d->location() == loc2d &&
                        pf2d->error_projector().is_identity() );

  testlib_test_begin( "3D point feature" );
  rgrl_feature_sptr pf3d = new rgrl_feature_point( loc3d );
  testlib_test_perform( pf3d->is_type( rgrl_feature_point::type_id() ) &&
                        pf3d->location() == loc3d &&
                        pf3d->error_projector().is_identity() );

  testlib_test_begin( "4D point feature" );
  rgrl_feature_sptr pf4d = new rgrl_feature_point( loc4d );
  testlib_test_perform( pf4d->is_type( rgrl_feature_point::type_id() ) &
                        pf4d->location() == loc4d &&
                        pf4d->error_projector().is_identity() );

  {
    testlib_test_begin( "Transform 2D point feature" );

    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 2.0;  A(1,0) =  1.0;
    A(1,0) = 4.0;  A(1,1) = -3.0;

    t[0] = -3.0;
    t[1] =  5.0;

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    testlib_test_perform( pf2d->location() == loc2d &&
                          result->is_type( rgrl_feature_point::type_id() ) &&
                          result->location() == xform.map_location( loc2d ) &&
                          result->error_projector().is_identity() );
  }
}


void
test_feature_trace_pt()
{
  vcl_cout << "Test trace point feature\n";

  vnl_vector<double> loc2d( 2 );
  loc2d[0] =  5.0;
  loc2d[1] = -4.0;

  vnl_vector<double> dir2d( 2 );
  dir2d[0] = 3.0;
  dir2d[1] = 1.0;

  vnl_vector<double> nor2d( 2 );
  nor2d[0] = -dir2d[1];
  nor2d[1] =  dir2d[0];
  nor2d.normalize();

  vnl_vector<double> loc4d( 4 );
  loc4d[0] =  2.0;
  loc4d[1] =  7.0;
  loc4d[2] = -1.0;
  loc4d[3] =  3.0;

  vnl_vector<double> dir4d( 4 );
  dir4d[0] = -2.0;
  dir4d[1] =  4.0;
  dir4d[2] = -1.5;
  dir4d[3] =  1.0;

  vnl_vector<double> err4d( 4 );
  err4d[0] =  2.0;
  err4d[1] = -1.0;
  err4d[2] =  3.0;
  err4d[3] = -8.0;

  testlib_test_begin( "2D trace point" );
  rgrl_feature_sptr pf2d = new rgrl_feature_trace_pt( loc2d, dir2d );
  rgrl_feature_caster<rgrl_feature_trace_pt> trace_ptr( pf2d );

  testlib_test_perform( pf2d->is_type( rgrl_feature_trace_pt::type_id() ) &&
                        pf2d->location() == loc2d &&
                        !pf2d->error_projector().is_identity() &&
                        ((rgrl_feature_trace_pt*) trace_ptr) -> length() == 0 &&
                        ((rgrl_feature_trace_pt*) trace_ptr) -> radius() == 0 );

  testlib_test_begin( "4D trace point feature" );
  rgrl_feature_sptr pf4d = new rgrl_feature_trace_pt( loc4d, dir4d );
  testlib_test_perform( pf4d->is_type( rgrl_feature_trace_pt::type_id() ) &&
                        pf4d->location() == loc4d &&
                        !pf4d->error_projector().is_identity() );

  TEST_NEAR( "4D trace point feature error projector",
             dot_product( pf4d->error_projector() * err4d, dir4d ), 0.0, 1e-6 );

  {
    testlib_test_begin( "Transform 2D trace point, location" );

    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 2.0;  A(0,1) =  1.0;
    A(1,0) = 4.0;  A(1,1) = -3.0;

    t[0] = -3.0;
    t[1] =  5.0;

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    testlib_test_perform( pf2d->location() == loc2d &&
                          result->is_type( rgrl_feature_trace_pt::type_id() ) &&
                          result->location() == xform.map_location( loc2d ) &&
                          !result->error_projector().is_identity() );

    // affine transforms do not preseve angles, calculate the
    // transformed normal by transforming the tangent and re-computing
    // the normal.

    vnl_vector<double> x_nor( 2 );
    vnl_vector<double> x_dir( 2 );
    dir2d.normalize();
    xform.map_direction( loc2d, dir2d, x_dir );
    x_nor[0] = -x_dir[1];
    x_nor[1] =  x_dir[0];

    TEST_NEAR( "                        , error projector",
               ( result->error_projector() - outer_product( x_nor, x_nor ) ).absolute_value_max(), 0, 1e-6 );
  }
}


void
test_feature_face()
{
  vcl_cout << "Test face feature\n";

  vnl_vector<double> loc2d( 2 );
  loc2d[0] =  3.0;
  loc2d[1] = -2.0;

  vnl_vector<double> nor2d( 2 );
  nor2d[0] = 3.0;
  nor2d[1] = 1.0;
  nor2d.normalize();

  testlib_test_begin( "2D face point" );
  rgrl_feature_sptr pf2d = new rgrl_feature_face_pt( loc2d, nor2d );
  rgrl_feature_face_pt* face_ptr = rgrl_feature_caster<rgrl_feature_face_pt>( pf2d );

  //  Basic constructor, type and normal first

  testlib_test_perform( pf2d->is_type( rgrl_feature_face_pt::type_id() ) &&
                        pf2d->location() == loc2d &&
                        !pf2d->error_projector().is_identity() );

  TEST_NEAR( "2d face point normal",
             (face_ptr->normal() - nor2d).inf_norm(), 0.0, 1e-6 );   // need the downcast here

 //  Test error projector in 2d

  vnl_vector<double> dir2d( 2 );
  dir2d[0] = -nor2d[1];
  dir2d[1] = nor2d[0];

  double dist = 3.0;
  vnl_vector<double> test_pt2d( loc2d + 10.0 * dir2d + dist * nor2d );
  vnl_vector<double> error_vect2d( test_pt2d - loc2d );

  TEST_NEAR( "2d face point error projector",
             vnl_math_abs( dot_product( pf2d->error_projector() * error_vect2d, error_vect2d ) ),
             dist*dist, 1e-6 );


  //  Onto testing the transformation

  {
    testlib_test_begin( "Transform 2D face point, location" );

    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_matrix<double> covar( 6, 6, vnl_matrix_identity );

    A(0,0) = 2.0;  A(0,1) =  1.0;
    A(1,0) = 4.0;  A(1,1) = -3.0;

    t[0] = -3.0;
    t[1] =  5.0;

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    testlib_test_perform( pf2d->location() == loc2d &&
                          result->is_type( rgrl_feature_face_pt::type_id() ) &&
                          result->location() == xform.map_location( loc2d ) &&
                          !result->error_projector().is_identity() );

    // affine transforms do not preseve angles, calculate the
    // transformed normal by transforming the tangent and re-computing
    // the normal.

    vnl_vector<double> x_nor( 2 );
    xform.map_normal( loc2d, nor2d, x_nor );

    TEST_NEAR( "                        , error projector",
               ( result->error_projector() - outer_product( x_nor, x_nor ) ).absolute_value_max(), 0, 1e-6 );
  }
}

} // end anonymous namespace

MAIN( test_feature )
{
  START( "various feature types" );

  test_feature_caster();
  test_feature_point();
  test_feature_trace_pt();
  test_feature_face();

  SUMMARY();
}
