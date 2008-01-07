#include "rgrl_estimator.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_match_set.h>

rgrl_estimator::
rgrl_estimator( unsigned int param_dof )
  : dof_( param_dof )
{
}

rgrl_estimator::
rgrl_estimator()
  : dof_( 0 )
{
}

rgrl_estimator::
~rgrl_estimator()
{
}


rgrl_transformation_sptr
rgrl_estimator::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  rgrl_set_of<rgrl_match_set_sptr> set;
  set.push_back( matches );
  return estimate( set, cur_transform );
}

//: Determine the weighted centres of the From and To points
//
bool
rgrl_est_compute_weighted_centres( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                                   vnl_vector<double>& from_centre,
                                   vnl_vector<double>& to_centre )
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  vnl_vector<double> from_pt;
  vnl_vector<double> to_pt;
  double sum_wgt = 0.0;

  // initialize centres to zero based on point dimension
  bool need_init = true;
  for ( unsigned ms=0; ms < matches.size() && need_init; ++ms ) {

    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end() && need_init; ++fi )
      for ( TIter ti = fi.begin(); ti != fi.end() && need_init ; ++ti ) {

        from_centre.set_size( fi.from_feature()->dim() );
        from_centre.fill( double(0) );

        to_centre.set_size( ti.to_feature()->dim() );
        to_centre.fill( double(0) );

        //done
        need_init = false;
      }
  }

  // sum of weighted point location
  for ( unsigned ms=0; ms < matches.size(); ++ms ) {

    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi )
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        double const wgt = ti.cumulative_weight();
        from_pt = fi.from_feature()->location();
        from_pt *= wgt;
        from_centre += from_pt;
        to_pt = ti.to_feature()->location();
        to_pt *= wgt;
        to_centre += to_pt;
        sum_wgt += wgt;
      }
  }

  // if the weight is too small or zero,
  // that means there is no good match
  if ( sum_wgt < 1e-8 ) {
    vcl_cerr << "Sum of weights is too small for centre computation.\n";
    return false;
  }

  from_centre /= sum_wgt;
  to_centre /= sum_wgt;

  return true;
}

unsigned
rgrl_est_matches_residual_number(rgrl_set_of<rgrl_match_set_sptr> const& matches)
{
  // count the number of constraints/residuals
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;
  unsigned int tot_num = 0;
  for ( unsigned ms = 0; ms<matches.size(); ++ms )
    if ( matches[ms] ) { // if pointer is valid

      rgrl_match_set const& one_set = *(matches[ms]);
      for ( FIter fi=one_set.from_begin(); fi!=one_set.from_end(); ++fi )
        if ( fi.size() ) {
          tot_num += fi.size() * fi.begin().to_feature()->dim();  // each point provides two constraints
        }
    }

  return tot_num;
}

