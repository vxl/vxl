#include <testlib/testlib_test.h>

#include <vnl/vnl_math.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_feature_trace_region.h>
#include <rgrl/rgrl_feature_face_region.h>

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

  {
    testlib_test_begin( "transformation of 3D trace region with length / radius" );
    double length = 7.0, radius = 4;
    vnl_vector<double> loc3d(3);  loc3d(0) = 4; loc3d(1) = 6; loc3d(2) = 8;
    vnl_vector<double> tan3d(3);  tan3d(0) = 1; tan3d(1) = 0; tan3d(2) = 0;
    vnl_vector<double> norm1(3);  norm1(0) = 0; norm1(1) = 1; norm1(2) = 0;
    vnl_vector<double> norm2(3);  norm2(0) = 0; norm2(1) = 0; norm2(2) = 1;

    rgrl_feature_sptr pf3d = new rgrl_feature_trace_region( loc3d, tan3d, length, radius );
    rgrl_feature_trace_region * trace3d_ptr = 
      rgrl_feature_caster<rgrl_feature_trace_region> ( pf3d );

    testlib_test_perform( pf3d->is_type( rgrl_feature_trace_region::type_id() ) &&
			  pf3d->location() == loc3d &&
			  !pf3d->error_projector().is_identity() &&
			  trace3d_ptr -> region_length() == length &&
			  trace3d_ptr -> region_radius() == radius );


    vnl_matrix<double> A( 3, 3 );
    vnl_vector<double> t( 3 );
    vnl_matrix<double> covar( 12, 12, vnl_matrix_identity );

    A(0,0) = 4.0;  A(0,1) =  0.0;  A(0,2) = 0.0;
    A(1,0) = 1.0;  A(1,1) =  2.0;  A(1,2) = 0.0;
    A(2,0) = 0.0;  A(2,1) =  0.0;  A(2,2) = 3.0;
    t[0] = -3.0;  t[1] = 5.0;  t[2] = 4.0;

    vnl_vector<double> transformed_loc = A * loc3d + t;
    vnl_vector<double> transformed_tan = A * tan3d;
    transformed_tan . normalize();

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = trace3d_ptr->transform_region( xform );
    rgrl_feature_trace_region * res_trace3d_ptr = 
      rgrl_feature_caster<rgrl_feature_trace_region> ( result );
    TEST_NEAR( "                 , location correct ",
	       ( result -> location() - transformed_loc ) . magnitude(), 0, 1e-6 );
    TEST_NEAR( "                 , direction correct",
	       ( res_trace3d_ptr -> tangent() - transformed_tan ) . magnitude(), 0, 1e-6 );

    
    TEST_NEAR( "                  , length scales ",
	       res_trace3d_ptr -> region_length(), (length * A * tan3d).magnitude(), 1e-6 );

    TEST_NEAR( "                  , radius scales ",
	       res_trace3d_ptr -> region_radius(), 
	       0.5 * radius * ( (A * norm1). magnitude()  + (A * norm2).magnitude()), 1e-6 );
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
    xform.map_direction( loc2d, nor2d, x_nor );

    TEST_NEAR( "                        , error projector",
               ( result->error_projector() - outer_product( x_nor, x_nor ) ).absolute_value_max(), 0, 1e-6 );
  }



  vnl_vector<double> loc4d( 4 );
  loc4d[0] =  2.0;
  loc4d[1] =  7.0;
  loc4d[2] = -1.0;
  loc4d[3] =  3.0;

  vnl_vector<double> nor4d( 4 );
  nor4d[0] = -2.0;
  nor4d[1] =  4.0;
  nor4d[2] = -1.5;
  nor4d[3] =  1.0;
  nor4d.normalize();

  double normal_width = 3,  radius = 10;
  
  testlib_test_begin( "4D face point feature" );
  rgrl_feature_sptr pf4d = new rgrl_feature_face_region( loc4d, nor4d, normal_width, radius );
  rgrl_feature_face_region* face_region_ptr = rgrl_feature_caster<rgrl_feature_face_region>( pf4d );
  testlib_test_perform( pf4d->is_type( rgrl_feature_face_region::type_id() ) &&
                        pf4d->location() == loc4d &&
                        !pf4d->error_projector().is_identity() &&
			face_region_ptr -> thickness() == normal_width &&
			face_region_ptr -> radius() == radius );

  TEST_NEAR( "4D face region feature error projector",
             ( pf4d->error_projector() - outer_product( nor4d, nor4d) ). max_value(), 0.0, 1e-6 );

  {
    testlib_test_begin( "transformation of 3D face point with region" );
    double normal_width = 4.0, radius = 8.0;
    vnl_vector<double> loc3d(3);  loc3d(0) = 4; loc3d(1) = 6; loc3d(2) = 8;
    vnl_vector<double> nor3d(3);  nor3d(0) = 1; nor3d(1) = 0; nor3d(2) = 0;
    vnl_vector<double> tan1(3);  tan1(0) = 0; tan1(1) = 1; tan1(2) = 0;
    vnl_vector<double> tan2(3);  tan2(0) = 0; tan2(1) = 0; tan2(2) = 1;

    rgrl_feature_sptr pf3d = new rgrl_feature_face_region( loc3d, nor3d, normal_width, radius );
    rgrl_feature_face_region * face3d_ptr = 
      rgrl_feature_caster<rgrl_feature_face_region> ( pf3d );

    testlib_test_perform( pf3d->is_type( rgrl_feature_face_region::type_id() ) &&
			  pf3d->location() == loc3d &&
			  !pf3d->error_projector().is_identity() &&
			  face3d_ptr -> thickness() == normal_width &&
			  face3d_ptr -> radius() == radius );


    vnl_matrix<double> A( 3, 3 );
    vnl_vector<double> t( 3 );
    vnl_matrix<double> covar( 12, 12, vnl_matrix_identity );

    A(0,0) = 4.0;  A(0,1) =  0.0;  A(0,2) = 0.0;
    A(1,0) = 1.0;  A(1,1) =  2.0;  A(1,2) = 0.0;
    A(2,0) = 0.0;  A(2,1) =  0.0;  A(2,2) = 3.0;
    t[0] = -3.0;  t[1] = 5.0;  t[2] = 4.0;

    vnl_vector<double> transformed_loc = A * loc3d + t;
    rgrl_trans_affine xform( A, t, covar );
    vnl_vector<double> x_norm3d(3);
    xform.map_direction( loc3d, nor3d, x_norm3d );

    rgrl_feature_sptr result = face3d_ptr->transform_region( xform );
    rgrl_feature_face_region * res_face3d_ptr = 
      rgrl_feature_caster<rgrl_feature_face_region> ( result );
    TEST_NEAR( "                 , location correct ",
	       ( result -> location() - transformed_loc ) . magnitude(), 0, 1e-6 );
    TEST_NEAR( "                 , normal direction correct",
	       ( res_face3d_ptr -> normal() - x_norm3d ) . magnitude(), 0, 1e-6 );
    TEST_NEAR( "                  , normal width scales ",
	       res_face3d_ptr -> thickness(), (normal_width * A * nor3d).magnitude(), 1e-6 );

    TEST_NEAR( "                  , radius scales ",
	       res_face3d_ptr -> radius(), 
	       0.5 * radius * ( (A * tan1). magnitude()  + (A * tan2).magnitude()), 1e-6 );
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
