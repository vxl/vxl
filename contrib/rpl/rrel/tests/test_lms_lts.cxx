// This is rpl/rrel/tests/test_lms_lts.cxx
#include <vnl/vnl_math.h>

#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_lts_obj.h>

#include <testlib/testlib_test.h>

static void test_lms_lts()
{
  //
  //  Testing the LMS objective functions
  //
  int dof = 3;
  double frac = 0.5;
  rrel_objective * lms1 = new rrel_lms_obj( dof, frac );

  vcl_vector<double> test_lms;
  test_lms.push_back( -1.0 );
  test_lms.push_back( -2.0 );
  test_lms.push_back( 1.2 );
  test_lms.push_back( -4.0 );
  test_lms.push_back( -1.5 );
  test_lms.push_back( -11.0 );
  test_lms.push_back( 14.1 );
  test_lms.push_back( 2.2 );
  test_lms.push_back( -56.0 );
  test_lms.push_back( -2.4 );
  test_lms.push_back( 3.1 );
  test_lms.push_back( 1.1 );
  test_lms.push_back( 2.15 );
  test_lms.push_back( -2.1 );
  test_lms.push_back( -0.8 );
  test_lms.push_back( 0.6 );
  double corr_obj = 2.2*2.2;
  double obj = lms1->fcn( test_lms.begin(), test_lms.end(), 0.0, 0 );
  TEST_NEAR("LMS with 0.5 inlier fraction:", obj, corr_obj, 1e-6);

  frac = 0.7;
  rrel_objective * lms2 = new rrel_lms_obj( dof, frac );
  corr_obj = 4.0*4.0;
  obj = lms2->fcn( test_lms.begin(), test_lms.end(), 0.0, 0 );
  TEST_NEAR("LMS with 0.7 inlier fraction:", obj, corr_obj, 1e-6);

  //
  //  Testing the LTS objective function
  //
  dof = 3;
  frac = 0.5;
  rrel_objective * lts1 = new rrel_lts_obj( dof, frac );
  vcl_vector<double> test_lts;
  test_lts.push_back( -1.0 );
  test_lts.push_back( -2.0 );
  test_lts.push_back( 1.2 );
  test_lts.push_back( -4.0 );
  test_lts.push_back( -1.5 );
  test_lts.push_back( -11.0 );
  test_lts.push_back( 14.1 );
  test_lts.push_back( 2.2 );
  test_lts.push_back( -56.0 );
  test_lts.push_back( -2.4 );
  test_lts.push_back( 3.1 );
  test_lts.push_back( 1.1 );
  test_lts.push_back( 2.15 );
  test_lts.push_back( -2.1 );
  test_lts.push_back( -0.8 );
  test_lts.push_back( 0.6 );
  corr_obj =
    vnl_math_sqr(-1.0) + vnl_math_sqr(-2.0) + vnl_math_sqr(1.2) + vnl_math_sqr(-1.5) +
    vnl_math_sqr(2.2)  + vnl_math_sqr(1.1) + vnl_math_sqr(2.15) + vnl_math_sqr(-2.1) +
    vnl_math_sqr(-0.8) + vnl_math_sqr(0.6);
  obj = lts1->fcn( test_lts.begin(), test_lts.end(), 0.0, 0 );
  TEST_NEAR("LTS with 0.5 inlier fraction:", obj, corr_obj, 1e-6);

  frac = 0.7;
  rrel_objective * lts2 = new rrel_lts_obj( dof, frac );
  corr_obj +=  vnl_math_sqr(-4.0) + vnl_math_sqr(-2.4) + vnl_math_sqr(3.1) ;
  obj = lts2->fcn( test_lts.begin(), test_lts.end(), 0.0, 0 );
  TEST_NEAR("LTS with 0.7 inlier fraction:", obj, corr_obj, 1e-6);
}

TESTMAIN(test_lms_lts);
