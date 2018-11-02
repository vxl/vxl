#include <iostream>
#include <testlib/testlib_test.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_transpose.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_face_pt.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <rgrl/rgrl_trans_affine.h>

static
void
test_feature_caster()
{
  vnl_double_2 loc2d(5.0, -4.0);
  rgrl_feature_point* pf2d = new rgrl_feature_point( loc2d );
  rgrl_feature_sptr fsptr = pf2d;
  TEST("feature sptr cast to 2D point feature", rgrl_feature_caster<rgrl_feature_point>(fsptr), pf2d);
}

static
void
test_feature_point()
{
  std::cout << "Test normal point feature\n";

  vnl_double_2 loc2d(5.0, -4.0);
  vnl_double_3 loc3d(5.0, -4.0,  2.0);
  vnl_double_4 loc4d(2.0,  7.0, -1.0, 3.0);

  rgrl_feature_sptr pf2d = new rgrl_feature_point( loc2d );
  TEST("2D point feature",
       pf2d->is_type( rgrl_feature_point::type_id() ) &&
       pf2d->location() == loc2d &&
       pf2d->error_projector().is_identity(), true);

  rgrl_feature_sptr pf3d = new rgrl_feature_point( loc3d );
  TEST("3D point feature",
       pf3d->is_type( rgrl_feature_point::type_id() ) &&
       pf3d->location() == loc3d &&
       pf3d->error_projector().is_identity(), true);

  rgrl_feature_sptr pf4d = new rgrl_feature_point( loc4d );
  TEST("4D point feature",
       pf4d->is_type( rgrl_feature_point::type_id() ) &&
       pf4d->location() == loc4d &&
       pf4d->error_projector().is_identity(), true);

  {
    double a[] = {2.0,1.0,4.0,-3.0};
    vnl_double_2x2 A(a);
    vnl_double_2 t(-3.0, 5.0);
    vnl_matrix_fixed<double,6,6> covar;

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    TEST("Transform 2D point feature (point location remains unchanged)", pf2d->location(), loc2d );
    TEST("Transform 2D point feature (feature is of point type)", result->is_type( rgrl_feature_point::type_id() ), true );
    TEST("Transform 2D point feature (mapped feature has location same as mapping location only)", result->location(), xform.map_location( loc2d ) );
    // std::cout << "Error projector: " << result->error_projector() << std::endl;
    vnl_double_2x2 true_error_projector(0.0);
    true_error_projector.fill_diagonal(1.0/result->scale()/result->scale());
    TEST("Transform 2D point feature (Error projector is identity matrix)", result->error_projector(), true_error_projector );
  }
}


static
void
test_feature_trace_pt()
{
  std::cout << "Test trace point feature\n";

  vnl_double_2 loc2d(5.0, -4.0);
  vnl_double_2 dir2d(3.0, 1.0);
  vnl_double_2 nor2d(-dir2d[1], dir2d[0]); nor2d.normalize();
  vnl_double_4 loc4d( 2.0, 7.0,-1.0, 3.0);
  vnl_double_4 dir4d(-2.0, 4.0,-1.5, 1.0);
  vnl_double_4 err4d( 2.0,-1.0, 3.0,-8.0);

  rgrl_feature_sptr pf2d = new rgrl_feature_trace_pt( loc2d, dir2d );
  rgrl_feature_caster<rgrl_feature_trace_pt> trace_ptr( pf2d );

  TEST("2D trace point",
       pf2d->is_type( rgrl_feature_trace_pt::type_id() ) &&
       pf2d->location() == loc2d &&
       !pf2d->error_projector().is_identity() &&
       ((rgrl_feature_trace_pt*) trace_ptr) -> length() == 0 &&
       ((rgrl_feature_trace_pt*) trace_ptr) -> radius() == 0, true);

  rgrl_feature_sptr pf4d = new rgrl_feature_trace_pt( loc4d, dir4d );
  TEST("4D trace point feature",
       pf4d->is_type( rgrl_feature_trace_pt::type_id() ) &&
       pf4d->location() == loc4d &&
       !pf4d->error_projector().is_identity(), true);

  TEST_NEAR( "4D trace point feature error projector",
             dot_product( pf4d->error_projector() * err4d, dir4d ), 0.0, 1e-6 );

  {
    double a[] = {2.0,1.0,4.0,-3.0};
    vnl_double_2x2 A(a);
    vnl_double_2 t(-3.0, 5.0);
    vnl_matrix_fixed<double,6,6> covar;

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    TEST("Transform 2D trace point, location",
         pf2d->location() == loc2d &&
         result->is_type( rgrl_feature_trace_pt::type_id() ) &&
         result->location() == xform.map_location( loc2d ) &&
         !result->error_projector().is_identity(), true);

    // affine transforms do not preseve angles, calculate the
    // transformed normal by transforming the tangent and re-computing
    // the normal.

    dir2d.normalize();
    vnl_vector<double> x_dir;
    xform.map_direction( loc2d, dir2d, x_dir );
    vnl_double_2 x_nor(-x_dir[1], x_dir[0]);

    TEST_NEAR( "                        , error projector",
               ( result->error_projector() - outer_product( x_nor, x_nor ) ).absolute_value_max(), 0, 1e-6 );
  }
}


static
void
test_feature_face()
{
  std::cout << "Test face feature\n";

  vnl_double_2 loc2d(3.0,-2.0);
  vnl_double_2 nor2d(3.0, 1.0); nor2d.normalize();

  rgrl_feature_sptr pf2d = new rgrl_feature_face_pt( loc2d, nor2d );
  rgrl_feature_face_pt* face_ptr = rgrl_feature_caster<rgrl_feature_face_pt>( pf2d );

  //  Basic constructor, type and normal first

  TEST("2D face point",
       pf2d->is_type( rgrl_feature_face_pt::type_id() ) &&
       pf2d->location() == loc2d &&
       !pf2d->error_projector().is_identity(), true);

  TEST_NEAR( "2d face point normal",
             (face_ptr->normal() - nor2d).inf_norm(), 0.0, 1e-6 );   // need the downcast here

  //  Test error projector in 2d

  vnl_double_2 dir2d(-nor2d[1], nor2d[0]);

  double dist = 3.0;
  vnl_double_2 test_pt2d = loc2d + 10.0 * dir2d + dist * nor2d;
  vnl_double_2 error_vect2d = test_pt2d - loc2d;

  TEST_NEAR( "2d face point error projector",
             vnl_math::abs( dot_product( pf2d->error_projector() * error_vect2d, error_vect2d ) ),
             dist*dist, 1e-6 );

  //  Onto testing the transformation

  {
    double a[] = {2.0,1.0,4.0,-3.0};
    vnl_double_2x2 A(a);
    vnl_double_2 t(-3.0, 5.0);
    vnl_matrix_fixed<double,6,6> covar; covar.set_identity();

    rgrl_trans_affine xform( A, t, covar );
    rgrl_feature_sptr result = pf2d->transform( xform );
    TEST("Transform 2D face point, location",
         pf2d->location() == loc2d &&
         result->is_type( rgrl_feature_face_pt::type_id() ) &&
         result->location() == xform.map_location( loc2d ) &&
         !result->error_projector().is_identity(), true);

    // This matrix convert normal to tangent
    double b[] = {0.0, -1.0, 1.0, 0.0};
    vnl_double_2x2 B(b);

    // mapping tangent is easy:  A*tangent
    vnl_double_2x2 C = vnl_transpose(B)*A*B;

    // affine transforms do not preseve angles, calculate the
    // transformed normal by transforming the tangent and re-computing
    // the normal.

    vnl_double_2 x_nor;
    // xform.map_normal( loc2d, nor2d, x_nor );
    x_nor = nor2d;
    x_nor.normalize();
    x_nor = C*x_nor;

    // compute the inverse of outer product,
    // because it is rank insufficient,
    // the inverse has to be done analytically.
    double eig_val = x_nor.magnitude();
    x_nor.normalize();
    // vnl_double_2x2 outer = outer_product( x_nor, x_nor );
    vnl_double_2x2 real_proj = outer_product( x_nor, x_nor ) / vnl_math::sqr(eig_val);

    //std::cout << "error proj: " << result->error_projector() << std::endl;
    //std::cout << "transformed normal: " << x_nor << std::endl;
    //std::cout << "real proj: " << real_proj << std::endl;
    TEST_NEAR( "                        , error projector",
               ( result->error_projector() - real_proj ).absolute_value_max(), 0, 0.05 );
  }
}

static void test_feature()
{
  test_feature_caster();
  test_feature_point();
  test_feature_trace_pt();
  test_feature_face();
}

TESTMAIN(test_feature);
