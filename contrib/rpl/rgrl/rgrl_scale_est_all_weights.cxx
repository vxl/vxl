#include "rgrl_scale_est_all_weights.h"
//:
// \file
// \author Chuck Stewart

#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <vnl/vnl_math.h>

#include "rgrl_scale.h"
#include "rgrl_match_set.h"
#include "rgrl_util.h"
#include <vcl_iostream.h>

rgrl_scale_est_all_weights::
rgrl_scale_est_all_weights( bool do_signature_scale )
  : do_signature_scale_( do_signature_scale )
{
}


rgrl_scale_sptr
rgrl_scale_est_all_weights::
estimate_weighted( rgrl_match_set const& match_set,
                   rgrl_scale_sptr const& /*unused*/,
                   bool use_signature_only,
                   bool penalize_scaling ) const
{
  rgrl_scale_sptr scales = new rgrl_scale;

  scales->set_geometric_scale( compute_geometric_scale( match_set, use_signature_only, penalize_scaling ) );

  if ( do_signature_scale_ ) {
    scales->set_signature_covar( compute_signature_covar( match_set ) );
  }

  return scales;
}


double
rgrl_scale_est_all_weights::
compute_geometric_scale( rgrl_match_set const& match_set, bool use_signature_wgt, bool penalize_scaling  ) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  double sum_weighted_error = 0;
  double sum_weights = 0;

  double scaling = 1;
  if ( penalize_scaling ) {
    //scaling applied here to penalize xform with distortion
    scaling = rgrl_util_geometric_error_scaling( match_set );
  }

  DebugMacro(2, '\n');
  DebugMacro_abv(2, "from\t to\t residuals\t signature_wgt\t cumulative_wgt\t weight :\n");
  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr ) {
    // rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
      // double error = titr.to_feature()->geometric_error( *mapped_from );
      double error = titr.geometric_error();
      double weight;
      if ( use_signature_wgt )
        weight = titr.signature_weight();
      else
        weight = titr.cumulative_weight();

      DebugMacro_abv(2, fitr.from_feature()->location() << "\t ");
      DebugMacro_abv(2, titr.to_feature()->location() << "\t " << error << "\t ");
      DebugMacro_abv(2, titr.signature_weight() << "\t " );
      DebugMacro_abv(2, titr.cumulative_weight() << "\t " << weight <<'\n');

      sum_weighted_error += weight * vnl_math_sqr( error );
      sum_weights += weight;
    }
  }
  double epsilon = 1e-16;
  double final_scale = scaling * vnl_math_max( vcl_sqrt( sum_weighted_error / sum_weights ),
                                               epsilon );

  DebugMacro(1, "  Final geometric scale" << final_scale << vcl_endl );
  return final_scale;

#if 0
  double est_scale = vcl_sqrt( sum_weighted_error / sum_weights );
  vcl_cout << " rgrl_scale_est_all_weights : scale = " << est_scale << " (lower bound=1.0)\n";
  return vnl_math_max( est_scale, 1.0 );
#endif
}

vnl_matrix<double>
rgrl_scale_est_all_weights::
compute_signature_covar( rgrl_match_set const&  match_set ) const
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator              to_iter;

  from_iter fitr = match_set.from_begin();
  unsigned nrows = fitr.from_feature()->signature_error_dimension( match_set.to_feature_type() );
  assert ( nrows > 0 );

  vnl_matrix<double> weighted_covar( nrows, nrows, 0.0 );
  double sum_weights = 0.0;

  for ( ; fitr != match_set.from_end(); ++fitr ) {
    rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
      vnl_vector<double> error_vec = titr.to_feature()->signature_error_vector( *mapped_from );
      double weight = titr.cumulative_weight();
      weighted_covar += weight * outer_product( error_vec, error_vec );
      sum_weights += weight;
    }
  }

  return weighted_covar / sum_weights;
}
