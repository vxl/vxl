#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vcl_cmath.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_2.h>

#include <rrel/rrel_muset_obj.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_initializer_ran_sam.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_scale.h>

#include "test_util.h"

static
rgrl_match_set_sptr
generate_match_set(rgrl_transformation_sptr trans)
{
  rgrl_trans_affine* xform = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
  vnl_matrix<double> A = xform->A();
  vnl_vector<double> t = xform->t();

  //generate 15 correct matches
  vcl_vector< rgrl_feature_sptr > from_pts;
  vcl_vector< rgrl_feature_sptr > to_pts;

  { //pt 1
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 2
    vnl_vector<double> v = vec2d( 10.0, 4.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 3
    vnl_vector<double> v = vec2d( 5.0, 15.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 4
    vnl_vector<double> v = vec2d( 7.0, 19.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 5
    vnl_vector<double> v = vec2d( 20.0, -7.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 6
    vnl_vector<double> v = vec2d( 15.0, 31.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 7
    vnl_vector<double> v = vec2d( -10.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 8
    vnl_vector<double> v = vec2d( 40.0, -29.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 9
    vnl_vector<double> v = vec2d( 2.0, 43.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt  10
    vnl_vector<double> v = vec2d( 55.0, 7.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 11
    vnl_vector<double> v = vec2d( 27.0, 33.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 12
    vnl_vector<double> v = vec2d( 63.0, -7.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 13
    vnl_vector<double> v = vec2d( -22.0, 35.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 14
    vnl_vector<double> v = vec2d( 0.0, 43.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 15
    vnl_vector<double> v = vec2d( 12.0, 23.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  //generate 5 random matches
  { //pt 16
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t + vec2d(5.0, 13.0)) );
  }

  { //pt 17
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t + vec2d(-5.0, 23.0)) );
  }

  { //pt 18
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t + vec2d(-27.0, 7.0)) );
  }

  { //pt 19
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t + vec2d(0.0, 19.0)) );
  }

  { //pt 20
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t + vec2d(-30.0, -1.0)) );
  }

  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  for ( unsigned i=0; i < from_pts.size(); ++i ) {
    ms->add_feature_and_match( from_pts[i], 0, to_pts[i] );
  }

  return ms;
}

static
rgrl_match_set_sptr
generate_ambiguous_match_set(rgrl_transformation_sptr trans)
{
  //generate 15 correct matches, with 5 having incorrect 2nd match
  rgrl_trans_affine* xform = dynamic_cast<rgrl_trans_affine*>(trans.as_pointer());
  vnl_matrix<double> A = xform->A();
  vnl_vector<double> t = xform->t();

  //generate 15 correct matches
  vcl_vector< rgrl_feature_sptr > from_pts;
  vcl_vector< rgrl_feature_sptr > to_pts;

  { //pt 1
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 2
    vnl_vector<double> v = vec2d( 10.0, 4.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 3
    vnl_vector<double> v = vec2d( 5.0, 15.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 4
    vnl_vector<double> v = vec2d( 7.0, 19.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 5
    vnl_vector<double> v = vec2d( 20.0, -7.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 6
    vnl_vector<double> v = vec2d( 15.0, 31.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 7
    vnl_vector<double> v = vec2d( -10.0, 3.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 8
    vnl_vector<double> v = vec2d( 40.0, -29.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt 9
    vnl_vector<double> v = vec2d( 2.0, 43.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  { //pt  10
    vnl_vector<double> v = vec2d( 55.0, 7.0);
    from_pts.   push_back( pf( v ) );
    to_pts.     push_back( pf( A*v + t ) );
  }

  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  for ( unsigned i=0; i < from_pts.size(); ++i ) {
    ms->add_feature_and_match( from_pts[i], 0, to_pts[i] );
  }

  //another 5 matches, each with 2 matches, one correct and one incorrect
  vcl_vector< rgrl_feature_sptr > from_pts_II;
  vcl_vector< vcl_vector< rgrl_feature_sptr > > to_pts_II;
  vcl_vector< rgrl_feature_sptr > empty;
  {
    to_pts_II.push_back(empty);
    vnl_vector<double> v = vec2d( 27.0, 33.0);
    from_pts_II.push_back( pf( v ) );
    to_pts_II[0].push_back( pf( A*v + t ) );
    to_pts_II[0].push_back( pf( A*v + t + vec2d(5.0, 13.0)) );
  }

  {
    to_pts_II.push_back(empty);
    vnl_vector<double> v = vec2d( 63.0, -7.0);
    from_pts_II.push_back( pf( v ) );
    to_pts_II[1].push_back( pf( A*v + t ) );
    to_pts_II[1].push_back( pf( A*v + t + vec2d(-5.0, 23.0)) );
  }

  {
    to_pts_II.push_back(empty);
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts_II.push_back( pf( v ) );
    to_pts_II[2].push_back( pf( A*v + t + vec2d(-27.0, 7.0)) );
    to_pts_II[2].push_back( pf( A*v + t ) );
  }

  {
    to_pts_II.push_back(empty);
    vnl_vector<double> v = vec2d( 2.0, 3.0);
    from_pts_II.push_back( pf( v ) );
    to_pts_II[3].push_back( pf( A*v + t + vec2d(0.0, 19.0)) );
    to_pts_II[3].push_back( pf( A*v + t ) );
  }

  {
    to_pts_II.push_back(empty);
    vnl_vector<double> v = vec2d( 12.0, 23.0);
    from_pts_II.push_back( pf( v ) );
    to_pts_II[4].push_back( pf( A*v + t ) );
    to_pts_II[4].push_back( pf( A*v + t + vec2d(-30.0, -1.0)) );
  }

  for ( unsigned i=0; i < from_pts_II.size(); ++i ) {
    ms->add_feature_and_matches( from_pts_II[i], 0, to_pts_II[i] );
  }

  return ms;
}

static
unsigned int
gen_num_samples(unsigned int total_matches, unsigned int unique_matches, bool all)
{
  if (all) {
    unsigned long numer=1;
    unsigned long denom=1;
    unsigned int n=total_matches;
    unsigned int k=3;
    for ( ; k>0; --k, --n ) {
      numer *= n;
      denom *= k;
    }
    return (unsigned int)( numer / denom );
  }

  double max_outlier_frac = 0.5;
  double desired_prob_good = 0.99;
  unsigned int max_populations_expected = 1;
  double prob_pt_inlier = (1 - max_outlier_frac) * unique_matches / double(total_matches);
  double prob_pt_good
    = max_populations_expected
    * vcl_pow( prob_pt_inlier / max_populations_expected, 3);
  return int(vcl_ceil( vcl_log(1.0 - desired_prob_good) /
                       vcl_log(1.0 - prob_pt_good) ));
}

static
void
test_on_matches(rgrl_transformation_sptr xform, rgrl_match_set_sptr matches, unsigned int total_matches)
{
  rgrl_mask_box from_roi( vnl_double_2(-50.0, -50.0),
                          vnl_double_2(100.0, 100.0));
  rgrl_mask_box to_roi = from_roi;
  rgrl_estimator_sptr est = new rgrl_est_affine(2);
  rgrl_view_sptr  view = new rgrl_view( from_roi, to_roi, from_roi, from_roi, est, xform, 0 );
  vcl_auto_ptr<rrel_objective> obj( new rrel_muset_obj(matches->from_size()) );
  rgrl_scale_estimator_unwgted_sptr scale_est = new rgrl_scale_est_closest( obj );

  rgrl_initializer_ran_sam* init = new rgrl_initializer_ran_sam();
  init->set_sampling_params();
  init->set_data(matches, scale_est, view);
  //rgrl_initializer_sptr init(init_ran_sam);

  rgrl_transformation_sptr t;
  rgrl_view_sptr v;
  rgrl_scale_sptr s;

  testlib_test_begin( "Generate the first view for not gen_all" );
  testlib_test_perform( init->next_initial( v, s ) );

  testlib_test_begin( "Total number of samples for not gen_all" );
  int total_samples = gen_num_samples(total_matches, matches->from_size(), false);
  testlib_test_perform( init->samples_tested() == total_samples );

  testlib_test_begin( "Transformation for not gen_all" );
  rgrl_trans_affine* estimated_aff = dynamic_cast<rgrl_trans_affine*>(v->xform_estimate().as_pointer());
  rgrl_trans_affine* org_aff = dynamic_cast<rgrl_trans_affine*>(xform.as_pointer());
  testlib_test_perform( estimated_aff&&
                        close( org_aff->A(), estimated_aff->A() ) &&
                        close( org_aff->t(), estimated_aff->t() ) );

  testlib_test_begin( "Update maximum overlap correctly" );
  rgrl_mask_box max_region = v->global_region();
  testlib_test_perform(close(max_region.x0(), vnl_double_2(-50,-50)) &&
                       close(max_region.x1(), vnl_double_2(98, 90)));

  testlib_test_begin( "No second" );
  testlib_test_perform( !init->next_initial( v, s ) );

  init->set_data(matches, scale_est, view);
  init-> set_gen_all_samples();

  testlib_test_begin( "Generate the first view for gen_all" );
  testlib_test_perform( init->next_initial( v,s  ) );

  testlib_test_begin( "Total number of samples for gen_all" );
  total_samples = gen_num_samples(total_matches, matches->from_size(), true);
  testlib_test_perform( init->samples_tested() == total_samples );

  testlib_test_begin( "Transformation for gen_all" );
  estimated_aff = dynamic_cast<rgrl_trans_affine*>(v->xform_estimate().as_pointer());
  testlib_test_perform( estimated_aff&&
                        close( org_aff->A(), estimated_aff->A() ) &&
                        close( org_aff->t(), estimated_aff->t() ) );

  delete init;
}

MAIN( test_initializer_ran_sam )
{
  rgrl_transformation_sptr trans =
    new rgrl_trans_affine( vnl_matrix<double>( 2, 2, vnl_matrix_identity ),
                           vnl_double_2( 2.0, 10.0 ),
                           vnl_matrix<double>( 6, 6, 0.0 ) );

  START( "initializers_ran_sam w/ unique matches" );
  rgrl_match_set_sptr unique_matches = generate_match_set(trans);
  int total_matches = 20;
  test_on_matches(trans, unique_matches, total_matches);

  START( "initializers_ran_sam w/ ambiguous matches" );
  rgrl_match_set_sptr ambiguous_matches = generate_ambiguous_match_set(trans);
  test_on_matches(trans, ambiguous_matches, total_matches);

  SUMMARY();
}

