//:
// \file
// \author Chuck Stewart

#include "rgrl_scale_est_closest.h"

#include <rrel/rrel_objective.h>
#include <vnl/vnl_math.h>

#include "rgrl_scale.h"
#include "rgrl_match_set.h"
#include "rgrl_util.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

rgrl_scale_est_closest::
rgrl_scale_est_closest( vcl_auto_ptr<rrel_objective>  obj,
                        bool                          do_signature_scale )
  : do_signature_scale_( do_signature_scale ),
    obj_( obj )
{
  assert( obj_->can_estimate_scale() );
}


rgrl_scale_est_closest::
~rgrl_scale_est_closest()
{
}


rgrl_scale_sptr
rgrl_scale_est_closest::
estimate_unweighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& /*current_scales*/,
                     bool penalize_scaling ) const
{
  rgrl_scale_sptr scales = new rgrl_scale;

  scales->set_geometric_scale( compute_geometric_scale( match_set, penalize_scaling ) );

  if ( do_signature_scale_ ) {
    scales->set_signature_covar( compute_signature_covar( match_set ) );
  }

  return scales;
}


double
rgrl_scale_est_closest::
compute_geometric_scale( rgrl_match_set const& match_set,
                         bool penalize_scaling ) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  double scaling = 1;
  if ( penalize_scaling ) {
    // scaling applied here to penalize xform with distortion
    scaling =  rgrl_util_geometric_error_scaling( match_set );
  }

  vcl_vector<double> error_distances;
  DebugMacro(1, "\n");

  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr ) {
    //  If there aren't any matches, set the error_distance to an
    //  arbitrarily large number.
    if ( fitr.empty() ) {
      DebugMacro_abv(1," no matched points for from: "<< fitr.from_feature()->location()<< ", set its error distances = 1.0e30\n");
      error_distances.push_back( 1.0e30 );
    } else {
      to_iter titr = fitr.begin();

      rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
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
  vcl_cout << " error_distance :\n" << vcl_endl;
  unsigned zeros = 0;
  for ( unsigned i = 0; i < error_distances.size(); ++i ) {
    if ( error_distances[ i ] == 0 )
      ++zeros;
    vcl_cout << error_distances[ i ] << vcl_endl;
  }
  vcl_cout << " number of zers : " << zeros << " out of " << error_distances.size() << vcl_endl;
#endif // 0

  double epsilon = 1e-16;
  double return_scale = scaling * vnl_math_max( obj_->scale( error_distances.begin(), error_distances.end() ), epsilon );

  return return_scale;
}

vnl_matrix<double>
rgrl_scale_est_closest::
compute_signature_covar( rgrl_match_set const& match_set ) const
{
  //  Do the same as above, one component at a time, BUT use the
  //  closest geometric feature to determine which signature vector to
  //  use.  (We really need to do better.)  This yields a diagonal
  //  matrix.  Be sure to set the size of the matrix.

  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  from_iter fitr = match_set.from_begin();
  unsigned nrows = fitr.from_feature()->signature_error_dimension( match_set.to_feature_type() );
  assert( nrows );

  vcl_vector< vcl_vector<double> > all_errors( nrows );

  for ( ; fitr != match_set.from_end(); ++fitr ) {
    //  If there aren't any matches, set the error_distance to an
    //  arbitrarily large number.
    if ( fitr.empty() ) {
      for ( unsigned r=0; r<nrows; ++r ) {
        all_errors[r].push_back( 1.0e30 );
      }
    } else {
      to_iter titr = fitr.begin();
//    to_iter best_titr = titr;

      rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
      double min_distance = titr.to_feature()->geometric_error( *mapped_from );

      for ( ++titr; titr != fitr.end(); ++titr ) {
        double distance = titr.to_feature()->geometric_error( *mapped_from );
        if ( distance < min_distance ) {
          min_distance = distance;
//        best_titr = titr;
        }
      }

      vnl_vector<double> signature_error = titr.to_feature()->signature_error_vector( *mapped_from );
      for ( unsigned r = 0; r < nrows; ++r ) {
        all_errors[r].push_back( signature_error[r] );
      }
    }
  }

  vnl_matrix<double> covar( nrows, nrows, 0.0 );

  for ( unsigned r = 0; r < nrows; ++r ) {
    covar(r,r) = vnl_math_sqr( obj_->scale( all_errors[r].begin(), all_errors[r].end() ) );
  }

  return covar;
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
