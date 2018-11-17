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
  simple_invariant_feature(const vnl_vector<double>& location,
                           const vnl_vector<double>& cart_inv,
                           const vnl_vector<double>& ang_inv)
    : location_(location),
      cart_inv_(cart_inv),
      ang_inv_(ang_inv)
  {}
  ~simple_invariant_feature() override = default;

  //estimate the a translation using the location
  bool estimate(rgrl_invariant_sptr         from,
                rgrl_transformation_sptr&   xform,
                rgrl_scale_sptr&            scale ) override
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
  const vnl_vector<double>& cartesian_invariants() const override {return cart_inv_;}
  const vnl_vector<double>& angular_invariants() const override {return ang_inv_;}

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
  std::vector<rgrl_invariant_sptr> fixed_set1;
  std::vector<rgrl_invariant_sptr> moving_set1;
  std::vector<rgrl_invariant_sptr> fixed_set2;
  std::vector<rgrl_invariant_sptr> moving_set2;

  vnl_vector_fixed<double,2> location1(10.0, 17.0);
  vnl_vector_fixed<double,2> location2(30.0, 84.0);
  vnl_vector_fixed<double,2> loc1_cart_inv(10.0, 20.0);
  vnl_vector_fixed<double,2> loc2_cart_inv(-3.0, 6.0);
  vnl_vector_fixed<double,2> loc1_ang_inv(vnl_math::pi/4, vnl_math::pi/6);
  vnl_vector_fixed<double,2> loc2_ang_inv(vnl_math::pi/2, vnl_math::pi);

  // Dataset 1
  vnl_vector_fixed<double,2> t(-15, 7); //shift
  vnl_vector_fixed<double,2> noise(0.001, 0.003);
  fixed_set1.push_back(new simple_invariant_feature(location1.as_ref(),
                                                    loc1_cart_inv.as_ref(),
                                                    loc1_ang_inv.as_ref()) );
  fixed_set1.push_back(new simple_invariant_feature(location2.as_ref(),
                                                    (loc2_cart_inv+noise).as_ref(),
                                                    (loc2_ang_inv+noise).as_ref()) );
  moving_set1.push_back(new simple_invariant_feature((location1 + t).as_ref(),
                                                     loc1_cart_inv.as_ref(),
                                                     loc1_ang_inv.as_ref()) );
  moving_set1.push_back(new simple_invariant_feature((location2 + t).as_ref(),
                                                     loc2_cart_inv.as_ref(),
                                                     loc2_ang_inv.as_ref()) );
  // Dataset2
  vnl_vector_fixed<double,2> location3( 15.0, 37.0);
  vnl_vector_fixed<double,2> location4(-11.0, 24.0);
  vnl_vector_fixed<double,2> noise2(0.04, 0.007);
  vnl_vector_fixed<double,2> noise3(0.1,  0.06);
  vnl_vector_fixed<double,2> zero_vec(0.0, 0.0);
  fixed_set2.push_back(new simple_invariant_feature(location3.as_ref(),
                                                    (loc1_cart_inv+noise2).as_ref(),
                                                    zero_vec.as_ref()) );
  fixed_set2.push_back(new simple_invariant_feature(location4.as_ref(),
                                                    (loc2_cart_inv+noise3).as_ref(),
                                                    zero_vec.as_ref()) );
  moving_set2.push_back(new simple_invariant_feature((location3 + t).as_ref(),
                                                     loc1_cart_inv.as_ref(),
                                                     zero_vec.as_ref()) );
  moving_set2.push_back(new simple_invariant_feature((location4 + t).as_ref(),
                                                     loc2_cart_inv.as_ref(),
                                                     zero_vec.as_ref()) );
  rgrl_mask_sptr roi = new rgrl_mask_box(2);
  rgrl_initializer_inv_indexing* initializer = new rgrl_initializer_inv_indexing( roi, roi, nullptr, 0, false);

  initializer->add_data(fixed_set1, moving_set1, 0, 1);
  initializer->add_data(fixed_set2, moving_set2, 0, 1);

  rgrl_view_sptr v;
  rgrl_scale_sptr s;

  std::vector<rgrl_invariant_match_sptr> matches = initializer->matches_for_moving_image(0);

  simple_invariant_feature* from = rgrl_cast<simple_invariant_feature*>(matches[3]->from());
  simple_invariant_feature* to = rgrl_cast<simple_invariant_feature*>(matches[3]->to());
  TEST("Last (best) match is correct", from->location() == location1+t &&
                                       to->location() == location1, true);

  initializer->next_initial( v, s );
  rgrl_trans_translation* trans = rgrl_cast<rgrl_trans_translation*>(v->xform_estimate());

  TEST("Transform is correct", trans->t(), t);

  matches = initializer->matches_for_moving_image(0);
  TEST("Remaining number of matches is correct", matches.size(), 3);

  from = rgrl_cast<simple_invariant_feature*>(matches[2]->from());
  to = rgrl_cast<simple_invariant_feature*>(matches[2]->to());
  TEST("2nd last (2nd best) match is correct", from->location() == location2+t &&
                                               to->location() == location2, true);
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

  rgrl_scale_sptr s;
  TEST("First get is good", init->next_initial( v, s ) && v->xform_estimate()==trans, true);

  // the view and transform shouldn't change
  //
  TEST("No second", !init->next_initial(v,s), true);
  TEST("No third" , !init->next_initial(v,s), true);
}

} //end anonymous namespace

static void test_initializer()
{
  test_single_prior();
  test_inv_indexing();
}

TESTMAIN(test_initializer);
