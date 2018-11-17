//:
// \file
// \author Chuck Stewart

#include <iostream>
#include "rgrl_scale_est_closest.h"

#include <rrel/rrel_objective.h>
#include <vnl/vnl_math.h>

#include "rgrl_scale.h"
#include "rgrl_match_set.h"
#include "rgrl_util.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

rgrl_scale_est_closest::
rgrl_scale_est_closest( std::unique_ptr<rrel_objective>  obj,
                        bool                          do_signature_scale )
  : do_signature_scale_( do_signature_scale ),
    obj_( std::move(obj) )
{
  assert( obj_->can_estimate_scale() );
}


rgrl_scale_est_closest::
~rgrl_scale_est_closest() = default;


rgrl_scale_sptr
rgrl_scale_est_closest::
estimate_unweighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& /*current_scales*/,
                     bool penalize_scaling ) const
{
  rgrl_scale_sptr scales = new rgrl_scale;

  double scale = -1.0;
  vnl_matrix<double> inv_covar;

  if ( compute_geometric_scale( scale, match_set, penalize_scaling ) )
    scales->set_geometric_scale( scale );

  if ( do_signature_scale_ && compute_signature_inv_covar( inv_covar, match_set ) ) {
    scales->set_signature_inv_covar( inv_covar );
  }

  return scales;
}


bool
rgrl_scale_est_closest::
compute_geometric_scale( double& return_scale,
                         rgrl_match_set const& match_set,
                         bool penalize_scaling ) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  double scaling = 1;
  if ( penalize_scaling ) {
    // scaling applied here to penalize xform with distortion
    scaling =  rgrl_util_geometric_error_scaling( match_set );
  }

  std::vector<double> error_distances;
  error_distances.reserve( match_set.from_size() );
  DebugMacro(1, "\n");

  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr ) {
    //  If there aren't any matches, set the error_distance to an
    //  arbitrarily large number.
    if ( fitr.empty() ) {
      DebugMacro_abv(1," no matched points for from: "<< fitr.from_feature()->location()<< ", set its error distances = 1.0e30\n");
      error_distances.push_back( 1.0e30 );
    }
    else {
      to_iter titr = fitr.begin();

      const rgrl_feature_sptr& mapped_from = fitr.mapped_from_feature();
      double min_distance = titr.to_feature()->geometric_error( *mapped_from );

      for ( ++titr; titr != fitr.end(); ++titr ) {
        double distance = titr.to_feature()->geometric_error( *mapped_from );
        if ( distance < min_distance ) {
          min_distance = distance;
        }
      }
      error_distances.push_back( min_distance );
    }
  }

#if 0 // commented out
  std::cout << " error_distance :\n" << std::endl;
  unsigned zeros = 0;
  for ( unsigned i = 0; i < error_distances.size(); ++i ) {
    if ( error_distances[ i ] == 0 )
      ++zeros;
    std::cout << error_distances[ i ] << std::endl;
  }
  std::cout << " number of zers : " << zeros << " out of " << error_distances.size() << std::endl;
#endif // 0

  // empty set
  if ( error_distances.empty() )
    return false;

  const double epsilon = 1e-16;
  return_scale = scaling * std::max( obj_->scale( error_distances.begin(), error_distances.end() ), epsilon );

  // is finite?
  if ( !vnl_math::isfinite( return_scale ) )
    return false;

  // success
  return true;
}

bool
rgrl_scale_est_closest::
compute_signature_inv_covar( vnl_matrix<double>& inv_covar, rgrl_match_set const& match_set ) const
{
  if ( !match_set.from_size() ) return false;

  //  Do the same as above, one component at a time, BUT use the
  //  closest geometric feature to determine which signature vector to
  //  use.  (We really need to do better.)  This yields a diagonal
  //  matrix.  Be sure to set the size of the matrix.

  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  from_iter fitr = match_set.from_begin();
  const unsigned nrows = fitr.from_feature()->signature_error_dimension( match_set.to_feature_type() );

  // check on the error vector dimension
  if ( !nrows ) return false;

  std::vector< std::vector<double> > all_errors( nrows );
  bool success = true;

  for ( ; fitr != match_set.from_end(); ++fitr ) {
    //  If there aren't any matches, set the error_distance to an
    //  arbitrarily large number.
    if ( fitr.empty() ) {
      for ( unsigned r=0; r<nrows; ++r ) {
        all_errors[r].push_back( 1.0e30 );
      }
    }
    else {
      to_iter titr = fitr.begin();
      to_iter best_titr = titr;

      const rgrl_feature_sptr& mapped_from = fitr.mapped_from_feature();
      double min_distance = titr.to_feature()->geometric_error( *mapped_from );

      for ( ++titr; titr != fitr.end(); ++titr ) {
        double distance = titr.to_feature()->geometric_error( *mapped_from );
        if ( distance < min_distance ) {
          min_distance = distance;
          best_titr = titr;
        }
      }

      vnl_vector<double> signature_error = best_titr.to_feature()->signature_error_vector( *mapped_from );
      for ( unsigned r = 0; r < nrows; ++r ) {
        all_errors[r].push_back( signature_error[r] );
      }
    }
  }

  inv_covar.set_size( nrows, nrows );
  inv_covar.fill( 0.0 );

  for ( unsigned r = 0; r < nrows&&success; ++r )
  {
    if ( all_errors[r].empty() ) {
      success = false;
      break;
    }

    const double std = obj_->scale( all_errors[r].begin(), all_errors[r].end() );
    success = success && vnl_math::isfinite( std );
    if ( std < 1e-10 )  // if variance is too small
      inv_covar(r,r) = 0.0;
    else
      inv_covar(r,r) = 1 / vnl_math::sqr( std );
  }

  return success;
}


rgrl_scale_sptr
rgrl_scale_est_closest::
estimate_weighted( rgrl_match_set const& match_set,
                   rgrl_scale_sptr const& scales,
                   bool /*use_signature_only*/,
                   bool penalize_scaling ) const
{
  return estimate_unweighted( match_set, scales, penalize_scaling );
}
