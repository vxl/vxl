// This is rpl/rrel/rrel_ran_sam_search.cxx
#include "rrel_ran_sam_search.h"
#include <rrel/rrel_objective.h>
#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_util.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

// Random number generator. This will be shared by all ran_sam instances.
static vnl_random global_generator_;


rrel_ran_sam_search::rrel_ran_sam_search( )
  : generate_all_(false),
    generator_( &global_generator_ ),
    own_generator_( false ),
    params_(0), scale_(0),
    samples_to_take_(0),
    trace_level_(0)
{
  set_sampling_params();
}

rrel_ran_sam_search::rrel_ran_sam_search( int seed )
  : generate_all_(false),
    generator_( new vnl_random(seed) ),
    own_generator_( true ),
    params_(0), scale_(0),
    samples_to_take_(0),
    trace_level_(0)
{
  set_sampling_params();
}


rrel_ran_sam_search::~rrel_ran_sam_search( )
{
  if ( own_generator_ )
    delete generator_;
}


// ------------------------------------------------------------
void
rrel_ran_sam_search::set_gen_all_samples()
{
  generate_all_ = true;
}


// ------------------------------------------------------------
void
rrel_ran_sam_search::set_sampling_params( double max_outlier_frac,
                                          double desired_prob_good,
                                          unsigned int max_populations_expected,
                                          unsigned int min_samples )
{
  generate_all_ = false;
  max_outlier_frac_ = max_outlier_frac;
  desired_prob_good_ = desired_prob_good;
  max_populations_expected_ = max_populations_expected;
  min_samples_ = min_samples;
}


// ------------------------------------------------------------
bool
rrel_ran_sam_search::estimate( const rrel_estimation_problem * problem,
                               const rrel_objective * obj_fcn )
{
  //
  //  Initialize the random sampling.
  //
  this->calc_num_samples( problem );
  if ( trace_level_ >= 1 )
    vcl_cout << "\nSamples = " << samples_to_take_ << vcl_endl;

  if ( obj_fcn->requires_prior_scale() &&
       problem->scale_type() == rrel_estimation_problem::NONE )
  {
    vcl_cerr << "ran_sam::estimate: Objective function requires a prior scale,"
             << " and the problem does not provide one.\n"
             << "                   Aborting estimation.\n";
    return false;
  }

  unsigned int points_per = problem->num_samples_to_instantiate();
  unsigned int num_points = problem->num_samples();
  vcl_vector<int> point_indices( points_per );
  vnl_vector<double> new_params;
  vcl_vector<double> residuals( num_points );
  double min_obj = 0.0;
  bool  obj_set=false;

  scale_ = -1;

  //
  //  The main loop repeatedly establishes a sample, generates fit
  //  parameters from the sample, calculates the objective function
  //  value, and tests the value against the best found thus far.  If
  //  a sample doesn't yield a parameter vector, it is still counted
  //  toward the total number to take.  This prevents errors that
  //  would arise when all samples are to be used, but still works
  //  correctly for probabilistic sampling because the possibility
  //  is rare.
  //
  for ( unsigned int s = 0; s<samples_to_take_; ++s ) {
    this->next_sample( s, num_points, point_indices, points_per );
    if ( trace_level_ >= 2 )
      this->trace_sample( point_indices );
    if ( problem->fit_from_minimal_set( point_indices, new_params ))
    {
      if ( trace_level_ >= 1 )
        vcl_cout << "Fit = " << new_params << vcl_endl;
      problem->compute_residuals( new_params, residuals );
      if ( trace_level_ >= 2)
        this->trace_residuals( residuals );

      double new_obj = 0.0;
      switch ( problem->scale_type() ) {
       case rrel_estimation_problem::NONE:
        new_obj = obj_fcn->fcn( residuals.begin(), residuals.end(), scale_, &new_params );
        break;
       case rrel_estimation_problem::SINGLE:
        new_obj = obj_fcn->fcn( residuals.begin(), residuals.end(), problem->prior_scale(), &new_params );
        break;
       case rrel_estimation_problem::MULTIPLE:
        new_obj = obj_fcn->fcn( residuals.begin(), residuals.end(), problem->prior_multiple_scales().begin(), &new_params );
        break;
       default:
        vcl_cerr << __FILE__ << ": unknown scale type\n";
        vcl_abort();
      }
      if ( trace_level_ >= 1)
        vcl_cout << "Objective = " << new_obj << vcl_endl;
      if ( !obj_set || new_obj<min_obj ) {
        if ( trace_level_ >= 2)
          vcl_cout << "New best\n";
        obj_set = true;
        min_obj = new_obj;
        params_ = new_params;
      }
      indices_ = point_indices;
      residuals_ = residuals;
    }
    else if (trace_level_ >= 1)
      vcl_cout << "No fit to sample.\n";
  }

  if ( ! obj_set ) {
    return false;
  }

  //
  // Estimation succeeded.  Now, estimate scale and then return.
  //
  problem->compute_residuals( params_, residuals );
  if ( trace_level_ >= 1)
    vcl_cout << "\nOptimum fit = " << params_ << vcl_endl;
  if ( trace_level_ >= 2)
    this->trace_residuals( residuals );
  if ( obj_fcn->can_estimate_scale() )
    scale_ = obj_fcn->scale( residuals.begin(), residuals.end() );
  else if ( residuals.size() > 1 )
    scale_ = rrel_util_median_abs_dev_scale( residuals.begin(), residuals.end() );
  else {
    scale_ = 0;
    vcl_cout << "Can't estimate scale from one residual!\n";
    return false;
  }
  if ( trace_level_ >= 1)
    vcl_cout << "Scale = " << scale_ << vcl_endl;
  return true;
}


// ------------------------------------------------------------
void
rrel_ran_sam_search::calc_num_samples( const rrel_estimation_problem* problem )
{
  if ( generate_all_ ) {
    //
    //  Calculate C(num_points, points_per_sample)
    //
    unsigned long numer=1;
    unsigned long denom=1;
    unsigned int n=problem->num_samples();
    unsigned int k=problem->num_samples_to_instantiate();
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
    double prob_pt_inlier = (1 - max_outlier_frac_) * problem->num_unique_samples() / double(problem->num_samples());
    double prob_pt_good
      = max_populations_expected_
        * vcl_pow( prob_pt_inlier / max_populations_expected_, (int)problem->num_samples_to_instantiate());
    samples_to_take_ = int(vcl_ceil( vcl_log(1.0 - desired_prob_good_) /
                                     vcl_log(1.0 - prob_pt_good) ));
    if ( samples_to_take_ < min_samples_ )
      samples_to_take_ = min_samples_;
  }
}

// ------------------------------------------------------------
void
rrel_ran_sam_search::next_sample( unsigned int taken,
                                  unsigned int num_points,
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

// ------------------------------------------------------------
void
rrel_ran_sam_search::print_params() const
{
  vcl_cout << "  max_outlier_frac_ = " << max_outlier_frac_ << '\n'
           << "  desired_prob_good_ = " << desired_prob_good_ <<  '\n'
           << "  max_populations_expected_ = " << max_populations_expected_ << '\n'
           << "  min_samples_ = " << min_samples_ << '\n'
           << "  generate_all_ = " << generate_all_ << vcl_endl;
}


// ------------------------------------------------------------
void
rrel_ran_sam_search::trace_sample( const vcl_vector<int>& indices ) const
{
  vcl_cout << "\nNew sample: ";
  for ( unsigned int i=0; i<indices.size(); ++i)
    vcl_cout << ' ' << indices[i];
  vcl_cout << vcl_endl;
}

// ------------------------------------------------------------
void
rrel_ran_sam_search::trace_residuals( const vcl_vector<double>& residuals ) const
{
  vcl_cout << "\nResiduals:\n";
  for ( unsigned int i=0; i<residuals.size(); ++i )
    vcl_cout << "  " << i << ":  " << residuals[i] << '\n';
  vcl_cout << vcl_endl;
}
