// This is rpl/rrel/rrel_wgted_ran_sam_search.cxx
#include "rrel_wgted_ran_sam_search.h"
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
#include <vcl_algorithm.h>

rrel_wgted_ran_sam_search::~rrel_wgted_ran_sam_search( )
{
  if ( own_generator_ )
    delete generator_;
}


// ------------------------------------------------------------
bool
rrel_wgted_ran_sam_search::estimate( const rrel_estimation_problem * problem,
                                    const rrel_objective * obj_fcn )
{
  // assume the weights are already sorted.
  // get similarity weights
  const vcl_vector<double>& wgts = problem->similarity_weights();
  if ( !wgts.empty() )
  {
    is_sim_wgt_set_ = true;
    assert( wgts.size() == problem->num_samples() );

    // sums up weights
    double sum_wgt = 0.0;
    for ( unsigned i=0; i<wgts.size(); ++i )
      sum_wgt += wgts[i];

    // build probility interval
    double current_lower = 0.0;
    double next_lower;
    intervals_.resize( wgts.size() );
    prob_interval one;
    for ( unsigned i=0; i<wgts.size(); ++i ) {
      one.index_ = i;
      one.lower_ = current_lower;
      next_lower = current_lower+wgts[i]/sum_wgt;
      one.upper_ = next_lower;
      intervals_[i] = one;
      current_lower = next_lower;
    }
  }
  // call same function in base class
  return rrel_ran_sam_search::estimate( problem, obj_fcn );
}

// ------------------------------------------------------------
void
rrel_wgted_ran_sam_search::next_sample( unsigned int taken,
                                  unsigned int num_points,
                                  vcl_vector<int>& sample,
                                  unsigned int points_per_sample )
{
  typedef vcl_vector<prob_interval>::iterator interval_iter;

  if ( generate_all_ || !is_sim_wgt_set_ ) {
    rrel_ran_sam_search::next_sample( taken, num_points, sample, points_per_sample );
    return;
  }

  if ( num_points == 1 ) {
    sample[0] = 0;
  } else {
    unsigned int k=0, counter=0;
    prob_interval one;
    interval_iter iter;
    int id;
    while ( k<points_per_sample ) // This might be an infinite loop!
    {
      one.upper_ = generator_->drand32();
      iter = vcl_lower_bound( intervals_.begin(), intervals_.end(), one );
      // though this should not happen
      if ( iter == intervals_.end() )
        continue;

      // get index;
      id = iter->index_;
      ++counter;
      bool different = true;
      for ( int i=k-1; i>=0 && different; --i )
        different = (id != sample[i]);
      if ( different )
        sample[k++] = id, counter = 0;
      else if (counter > 1000)
      {
        vcl_cerr << "rrel_wgted_ran_sam_search::next_sample --- WARNING: "
                 << "drand32() generated 1000x the same value "<< id
                 << " from the range [0," << num_points-1 << "]\n"
                 << " prob is " << one.lower_ << " in range [" << iter->lower_
                 << ", " << iter->upper_ << "]\n";
        sample[k++] = id+1;
      }
    }
  }
}

