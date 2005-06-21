#include <testlib/testlib_test.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

#include <rgrl/rgrl_initializer_prior.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_invariant.h>
#include <rgrl/rgrl_invariant_sptr.h>
#include <rgrl/rgrl_initializer_inv_indexing.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_invariant_match.h>

#include <vnl/vnl_random.h>

namespace {

// A simple derived class of rgrl_invariant class. The class contains
// a location and a set of invariant features, consisting of x number
// of cartesians and y number of angulars.

class simple_invariant_feature: public rgrl_invariant
{
 public:
  simple_invariant_feature(vnl_vector<double> location,
                           vnl_vector<double> cart_inv,
                           vnl_vector<double> ang_inv)
    : location_(location),
      cart_inv_(cart_inv),
      ang_inv_(ang_inv)
  {}
  ~simple_invariant_feature(){}

  //estimate the a translation using the location
  bool estimate(rgrl_invariant_sptr         from,
                rgrl_transformation_sptr&   xform,
                rgrl_scale_sptr&            scale )
  {
    simple_invariant_feature* simple_from =
      rgrl_cast<simple_invariant_feature*>(from);
    vnl_vector<double> t = simple_from->location()-location_;
    xform = new rgrl_trans_translation(t);
    scale = new rgrl_scale;
    scale->set_geometric_scale( 0 );
    return true;
  }

  const vnl_vector<double>& location() const {return location_;}
  const vnl_vector<double>& cartesian_invariants() const {return cart_inv_;}
  const vnl_vector<double>& angular_invariants() const {return ang_inv_;}

 private:
  vnl_vector<double> location_;
  vnl_vector<double> cart_inv_;
  vnl_vector<double> ang_inv_;
};

void
test_inv_indexing()
{
  vnl_random rand;

  //generate invariant features
  vcl_vector<rgrl_invariant_sptr> fixed_set1;
  vcl_vector<rgrl_invariant_sptr> moving_set1;
  vcl_vector<rgrl_invariant_sptr> fixed_set2;
  vcl_vector<rgrl_invariant_sptr> moving_set2;

  vnl_vector_fixed<double,2> location1(10.0, 17.0);
  vnl_vector_fixed<double,2> location2(30.0, 84.0);
  vnl_vector_fixed<double,2> loc1_cart_inv(10.0, 20.0);
  vnl_vector_fixed<double,2> loc2_cart_inv(-3.0, 6.0);
  vnl_vector_fixed<double,2> loc1_ang_inv(vnl_math::pi/4, vnl_math::pi/6);
  vnl_vector_fixed<double,2> loc2_ang_inv(vnl_math::pi/2, vnl_math::pi);

  // Dataset 1
  vnl_vector_fixed<double,2> t(-15, 7); //shift
  vnl_vector_fixed<double,2> noise(0.001, 0.003);
  fixed_set1.push_back(new simple_invariant_feature(location1,
                                                    loc1_cart_inv,
                                                    loc1_ang_inv) );
  fixed_set1.push_back(new simple_invariant_feature(location2,
                                                    loc2_cart_inv+noise,
                                                    loc2_ang_inv+noise) );
  moving_set1.push_back(new simple_invariant_feature(location1 + t,
                                                     loc1_cart_inv,
                                                     loc1_ang_inv) );
  moving_set1.push_back(new simple_invariant_feature(location2 + t,
                                                     loc2_cart_inv,
                                                     loc2_ang_inv) );
  // Dataset2
  vnl_vector_fixed<double,2> location3(15.0, 37.0);
  vnl_vector_fixed<double,2> location4(-11.0, 24.0);
  vnl_vector_fixed<double,2> noise2(0.04, 0.007);
  vnl_vector_fixed<double,2> noise3(0.1, 0.06);
  vnl_vector<double> zero_vec(0);
  fixed_set2.push_back(new simple_invariant_feature(location3,
                                                    loc1_cart_inv+noise2,
                                                    zero_vec) );
  fixed_set2.push_back(new simple_invariant_feature(location4,
                                                    loc2_cart_inv+noise3,
                                                    zero_vec) );
  moving_set2.push_back(new simple_invariant_feature(location3 + t,
                                                     loc1_cart_inv,
                                                     zero_vec) );
  moving_set2.push_back(new simple_invariant_feature(location4 + t,
                                                     loc2_cart_inv,
                                                     zero_vec) );
  rgrl_mask_sptr roi = new rgrl_mask_box(2);
  rgrl_initializer_inv_indexing* initializer = new rgrl_initializer_inv_indexing( roi, roi, 0, 0, false);

  initializer->add_data(fixed_set1, moving_set1, 0, 1);
  initializer->add_data(fixed_set2, moving_set2, 0, 1);

  rgrl_view_sptr v;
  rgrl_scale_sptr s;

  vcl_vector<rgrl_invariant_match_sptr> matches = initializer->matches_for_moving_image(0);

  testlib_test_begin( "Last (best) match is correct" );
  simple_invariant_feature* from = rgrl_cast<simple_invariant_feature*>(matches[3]->from());
  simple_invariant_feature* to = rgrl_cast<simple_invariant_feature*>(matches[3]->to());
  testlib_test_perform(from->location() == location1+t &&
                       to->location() == location1);

  initializer->next_initial( v, s );
  rgrl_trans_translation* trans = rgrl_cast<rgrl_trans_translation*>(v->xform_estimate());

  testlib_test_begin( "Transform is correct" );
  testlib_test_perform(trans->t()==t);

  testlib_test_begin( "Remaining number of matches is correct" );
  matches = initializer->matches_for_moving_image(0);
  testlib_test_perform(matches.size() == 3);


  testlib_test_begin( "2nd last (2nd best) match is correct" );
  from = rgrl_cast<simple_invariant_feature*>(matches[2]->from());
  to = rgrl_cast<simple_invariant_feature*>(matches[2]->to());
  testlib_test_perform(from->location() == location2+t &&
                       to->location() == location2);

  delete initializer;
}

void
test_single_prior()
{
  rgrl_transformation_sptr trans =
    new rgrl_trans_affine( vnl_matrix<double>( 3, 3, vnl_matrix_identity ),
                           vnl_vector<double>( 3, 0.0 ),
                           vnl_matrix<double>( 12, 12, 0.0 ) );

  rgrl_estimator_sptr est = new rgrl_est_affine;

  rgrl_mask_sptr roi = new rgrl_mask_box(3);
  rgrl_view_sptr view = new rgrl_view( roi,
                                       roi,
                                       roi->bounding_box(),
                                       roi->bounding_box(),
                                       est,
                                       trans,
                                       0 );

  rgrl_initializer_sptr init = new rgrl_initializer_prior( view );

  rgrl_transformation_sptr t;
  rgrl_view_sptr v;

  testlib_test_begin( "First get is good" );
  rgrl_scale_sptr s;
  testlib_test_perform( init->next_initial( v, s ) && v->xform_estimate()==trans);

  // the view and transform shouldn't change
  //
  testlib_test_begin( "No second" );
  testlib_test_perform( !init->next_initial( v, s ) );

  testlib_test_begin( "No third" );
  testlib_test_perform( !init->next_initial( v, s ) );
}

} //end anonymous namespace

MAIN( test_initializer )
{
  START( "initializers" );

  test_single_prior();
  test_inv_indexing();

  SUMMARY();
}
