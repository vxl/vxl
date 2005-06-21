//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include "rgrl_initializer_ran_sam.h"
#include "rgrl_match_set.h"
#include "rgrl_estimator.h"
#include "rgrl_scale_estimator.h"
#include "rgrl_scale.h"
#include "rgrl_trans_affine.h"
#include "rgrl_view.h"
#include "rgrl_util.h"

#include <vnl/vnl_random.h>

static const unsigned verbose_ = 0;

// Random number generator. This will be shared by all ran_sam instances.
static vnl_random global_generator_;

// Iterators to go over the matches
//
typedef rgrl_match_set::from_iterator FIter;
typedef FIter::to_iterator TIter;

rgrl_initializer_ran_sam::
rgrl_initializer_ran_sam( )
  : generate_all_(false),
    generator_( &global_generator_ ),
    own_generator_( false ),
    xform_(0), scale_(0),
    samples_to_take_(0)
{
  set_sampling_params();
}

rgrl_initializer_ran_sam::
rgrl_initializer_ran_sam( int seed )
  : generate_all_(false),
    generator_( new vnl_random( seed ) ),
    own_generator_( true ),
    xform_(0), scale_(0),
    samples_to_take_(0)
{
  set_sampling_params();
}

rgrl_initializer_ran_sam::
~rgrl_initializer_ran_sam()
{
  if ( own_generator_ )
    delete generator_;
}

void
rgrl_initializer_ran_sam::
set_gen_all_samples()
{
  generate_all_ = true;
}

void
rgrl_initializer_ran_sam::
set_sampling_params( double max_outlier_frac,
                     double desired_prob_good,
                     unsigned int max_populations_expected,
                     unsigned int min_samples )
{
  data_set_ = false;
  generate_all_ = false;
  max_outlier_frac_ = max_outlier_frac;
  desired_prob_good_ = desired_prob_good;
  max_populations_expected_ = max_populations_expected;
  min_samples_ = min_samples;
}

void
rgrl_initializer_ran_sam::
set_data(rgrl_match_set_sptr                init_match_set,
         rgrl_scale_estimator_unwgted_sptr  scale_est,
         rgrl_view_sptr                     prior_view,
         bool should_estimate_global_region)
{
  match_set_ = init_match_set;
  transform_estiamtor_ = prior_view->xform_estimator();
  scale_estimator_ = scale_est;
  init_view_ = prior_view;
  data_set_ = true;
  called_before_ = false;
  should_estimate_global_region_ = should_estimate_global_region;
}

void
rgrl_initializer_ran_sam::
set_data(rgrl_match_set_sptr                init_match_set,
         rgrl_scale_estimator_unwgted_sptr  scale_est,
         rgrl_mask_sptr           const&    from_image_roi,
         rgrl_mask_sptr           const&    to_image_roi,
         rgrl_mask_box            const&    initial_from_image_roi,
         rgrl_estimator_sptr                xform_estimator,
         unsigned                           initial_resolution,
         bool should_estimate_global_region)
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  rgrl_view_sptr  prior_view  = new rgrl_view( from_image_roi, to_image_roi,
                                               initial_from_image_roi,
                                               global_region, xform_estimator,
                                               0, initial_resolution );
  set_data( init_match_set, scale_est, prior_view, should_estimate_global_region );
}

void
rgrl_initializer_ran_sam::
set_data(rgrl_match_set_sptr                init_match_set,
         rgrl_scale_estimator_unwgted_sptr  scale_est,
         rgrl_mask_sptr      const&         from_image_roi,
         rgrl_mask_sptr      const&         to_image_roi,
         rgrl_estimator_sptr                xform_estimator,
         unsigned                           initial_resolution )
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  rgrl_view_sptr  prior_view  = new rgrl_view( from_image_roi, to_image_roi,
                                               global_region, global_region,
                                               xform_estimator, 0,
                                               initial_resolution );
  set_data( init_match_set, scale_est, prior_view, false );
}

void
rgrl_initializer_ran_sam::
set_data(rgrl_match_set_sptr                init_match_set,
         rgrl_scale_estimator_unwgted_sptr  scale_est,
         rgrl_mask_sptr      const&         from_image_roi,
         rgrl_estimator_sptr                xform_estimator,
         unsigned                           initial_resolution )
{
  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  rgrl_view_sptr  prior_view  = new rgrl_view( from_image_roi, from_image_roi,
                                               global_region, global_region,
                                               xform_estimator, 0,
                                               initial_resolution );
  set_data( init_match_set, scale_est, prior_view, false );
}

bool
rgrl_initializer_ran_sam::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale)
{
  if ( called_before_ ) return false;

  if (!estimate()) return false;

  int dim = init_view_->from_image_roi()->x0().size();
  rgrl_mask_box global_region( dim );

  if ( should_estimate_global_region_ )
    global_region =
      rgrl_util_estimate_global_region(init_view_->from_image_roi(),
                                       init_view_->to_image_roi(),
                                       init_view_->global_region(),
                                       *xform_);
  else {
    global_region.set_x0( init_view_->from_image_roi()->x0() );
    global_region.set_x1( init_view_->from_image_roi()->x1() );
  }

  view = new rgrl_view(init_view_->from_image_roi(), init_view_->to_image_roi(),
                       init_view_->region(), global_region,
                       init_view_-> xform_estimator(),
                       xform_,
                       init_view_->resolution());
  prior_scale = scale_;

  called_before_ = true;

  return true;
}

bool
rgrl_initializer_ran_sam::
estimate()
{
  //
  //  Initialize the random sampling.
  //
  // Calculate the total number of matches. In most problems, this equals
  // the match_set_->size() (number of "unique samples"). With estimation
  // problems involving non-unique correspondences, however, the total
  // number of possible correspondences generally
  // much greater than the number of unique samples
  //
  unsigned int total_num_matches = 0;
  for ( FIter fi = match_set_->from_begin(); fi != match_set_->from_end(); ++fi ) {
    total_num_matches += fi.size();
  }
  this->calc_num_samples( total_num_matches );

  DebugMacro_abv( 1 , "Samples = " << samples_to_take_ <<'\n' );

  unsigned int points_per = (int)vcl_floor((double)transform_estiamtor_->param_dof()/match_set_->num_constraints_per_match());
  vcl_vector<int> point_indices( points_per );
  rgrl_trans_affine dummy_trans(3);
  rgrl_scale_sptr dummy_scale;
  bool  scale_set=false;

  //
  //  The main loop repeatedly establishes a sample, generates fit
  //  parameters from the sample, calculates the error scale
  //  value, and tests the value against the best found thus far.  If
  //  a sample doesn't yield a parameter vector, it is still counted
  //  toward the total number to take.  This prevents errors that
  //  would arise when all samples are to be used, but still works
  //  correctly for probabilistic sampling because the possibility
  //  is rare.
  //
  for ( unsigned int s = 0; s<samples_to_take_; ++s ) {
    this->next_sample( s, total_num_matches, point_indices, points_per );
    rgrl_match_set_sptr
      sub_match_set = this->get_matches(point_indices,total_num_matches );
    if (this->debug_flag() > 2) this->trace_sample( point_indices );

    rgrl_transformation_sptr
      new_xform = transform_estiamtor_->estimate( sub_match_set, dummy_trans );
    if ( new_xform )
    {
      match_set_->remap_from_features(*new_xform);
      rgrl_scale_sptr
        new_scale = scale_estimator_->estimate_unweighted(*match_set_, dummy_scale);

      if ( !scale_set || (new_scale->has_geometric_scale() &&
                          new_scale->geometric_scale() < scale_->geometric_scale()) ) {
        scale_ = new_scale;
        xform_ = new_xform;
        scale_set = true;
      }
    }
    else DebugMacro_abv(1, "No fit to sample.\n");
  }

  if ( ! scale_set ) {
    return false;
  }

  DebugMacro_abv(1,"Final geometric scale = "<<scale_->geometric_scale()<<'\n');

  return true;
}

//
//: Calculate number of samples --- non-unique matching estimation problems
void
rgrl_initializer_ran_sam::
calc_num_samples( unsigned int num_matches )
{
  if ( generate_all_ ) {
    //
    //  Calculate C(num_points, points_per_sample)
    //
    unsigned long numer=1;
    unsigned long denom=1;
    unsigned int n=num_matches;
    unsigned int k=(int)vcl_floor((double)transform_estiamtor_->param_dof()/match_set_->num_constraints_per_match());
    for ( ; k>0; --k, --n ) {
      numer *= n;
      denom *= k;
    }
    samples_to_take_ = (unsigned int)( numer / denom );
  }
  else {
    //
    //  Calculate the probability that a sample is good.  Then, use this
    //  to determine the minimum number of samples required.
    //
    int num_samples_to_instantiate =
      (int)vcl_floor((double)transform_estiamtor_->param_dof()/match_set_->num_constraints_per_match());
    unsigned int num_unique_matches = match_set_->from_size();
    double prob_pt_inlier = (1 - max_outlier_frac_) * num_unique_matches / double(num_matches);
    double prob_pt_good
      = max_populations_expected_
      * vcl_pow( prob_pt_inlier / max_populations_expected_, num_samples_to_instantiate );
    samples_to_take_ = int(vcl_ceil( vcl_log(1.0 - desired_prob_good_) /
                                     vcl_log(1.0 - prob_pt_good) ));
    if ( samples_to_take_ < min_samples_ )
      samples_to_take_ = min_samples_;
  }
}

//: Determine the next random sample, filling in the "sample" vector.
void
rgrl_initializer_ran_sam::
next_sample( unsigned int taken, unsigned int num_points,
             vcl_vector<int>& sample,
             unsigned int points_per_sample )
{
  assert( sample.size() == points_per_sample );

  if ( generate_all_ ) {
    if ( taken == 0 ) {  //  initial sample
      for ( unsigned int i=0; i<points_per_sample; ++i )
        sample[i] = i;
    }
    else if ( taken >= samples_to_take_ )
      vcl_cerr << "rrel_ran_sam_search::next_sample -- ERROR: used all samples\n";
    else {
      //
      //  Generate the subsets in lexicographic order.
      //
      unsigned int i=points_per_sample-1;
      unsigned int k=num_points-1;
      while ( sample[i] == (int)k ) { --i; --k; }
      k = ++ sample[i];
      for ( ++k, ++i; i<points_per_sample; ++i, ++k )
        sample[i]=k;
    }
  }

  else {
    if ( num_points == 1 ) {
      sample[0] = 0;
    } else {
      unsigned int k=0, counter=0;
      while ( k<points_per_sample ) // This might be an infinite loop!
      {
        int id = generator_->lrand32( 0, num_points-1 );
        if ( id >= int(num_points) ) {   //  safety check
          vcl_cerr << "rrel_ran_sam_search::next_sample --- "
                   << "WARNING: random value out of range\n";
        }
        else
        {
          ++counter;
          bool different = true;
          for ( int i=k-1; i>=0 && different; --i )
            different = (id != sample[i]);
          if ( different )
            sample[k++] = id, counter = 0;
          else if (counter > 100)
          {
            vcl_cerr << "rrel_ran_sam_search::next_sample --- WARNING: "
                     << "lrand32() generated 100x the same value "<< id
                     << " from the range [0," << num_points-1 << "]\n";
            sample[k++] = id+1;
          }
        }
      }
    }
  }
}

rgrl_match_set_sptr
rgrl_initializer_ran_sam::
get_matches(const vcl_vector<int>&  point_indices, unsigned int total_num_matches)
{
  rgrl_match_set_sptr
    sub_match_set = new rgrl_match_set( match_set_->from_feature_type(), match_set_->to_feature_type() );

  if ( total_num_matches == match_set_->from_size() ) { //unique matches
    for (unsigned int i = 0; i<point_indices.size(); ++i) {
      FIter fi = match_set_->from_begin() + point_indices[i];
      TIter ti = fi.begin();
      sub_match_set->add_feature_and_match( fi.from_feature(),
                                            fi.mapped_from_feature(),
                                            ti.to_feature() );
    }
  }
  else {
    int match_count = 0;
    for ( FIter fi = match_set_->from_begin(); fi != match_set_->from_end(); ++fi ) {
      for (unsigned int i = 0; i<point_indices.size(); ++i) {
        if ( match_count <= point_indices[i] && match_count + (int)fi.size() > point_indices[i]) {
          unsigned int offset = point_indices[i] - match_count;
          TIter ti = fi.begin() + offset;
          sub_match_set->add_feature_and_match( fi.from_feature(),
                                                fi.mapped_from_feature(),
                                                ti.to_feature() );
        }
      }
      match_count += fi.size();
    }
  }

  return sub_match_set;
}

void
rgrl_initializer_ran_sam::
trace_sample( const vcl_vector<int>& indices ) const
{
  vcl_cout << "\nNew sample: ";
  for ( unsigned int i=0; i<indices.size(); ++i)
    vcl_cout << ' ' << indices[i];
  vcl_cout << vcl_endl;
}
