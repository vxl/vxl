#include "rgrl_weighter_indiv_scale.h"
//:
// \file
// \author Gehua Yang
// \date   March 2006

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <rrel/rrel_m_est_obj.h>

#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_transformation.h>

rgrl_weighter_indiv_scale::
rgrl_weighter_indiv_scale( vcl_auto_ptr<rrel_m_est_obj>  m_est,
                     bool                          use_signature_error,
                     bool                          use_precomputed_signature_wgt )
 :rgrl_weighter_m_est( m_est, use_signature_error, use_precomputed_signature_wgt )
{
}


rgrl_weighter_indiv_scale::
~rgrl_weighter_indiv_scale()
{
}


void
rgrl_weighter_indiv_scale::
compute_weights( rgrl_scale const&  scales,
                 rgrl_match_set&    match_set ) const
{
  //  Cache the inverse covariance of the signature error if signature
  //  errors are used.  Be careful of null entries.

  vnl_matrix<double> signature_inv_covar;
  if ( use_signature_error_ && !signature_precomputed_ ) {
    assert ( scales.has_signature_inv_covar() );
    signature_inv_covar = scales.signature_inv_covar();
  }

  //  cache the geometric scale.
  assert ( scales.has_geometric_scale() );
  double geometric_scale = scales.geometric_scale();

  typedef rgrl_match_set::from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;

  DebugMacro(1,'\n');
  DebugMacro_abv(1, "Matched points : from\t to\t geo_err\t geo_wgt\t cum_wgt:\n" );

  //  for each from image feature being matched
  for ( from_iter fitr = match_set.from_begin();
       fitr != match_set.from_end(); ++fitr ) {

    DebugMacro_abv(1, fitr.from_feature()->location() << '\t');
    if ( fitr.empty() ) DebugMacro_abv(2, '\n' );

    if ( fitr.size() == 0 )  continue;

    double sum_weights = 0; // for normalizing, later
    rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();

    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr )
    {
      DebugMacro_abv( 1, titr.to_feature()->location() << "\t " );

      //  for each match with a "to" image feature
      rgrl_feature_sptr to_feature = titr.to_feature();
      const double scaled_err = to_feature->geometric_error( *mapped_from );

      // It is important to factor in the feature scale, along with geometric scale
      // As the error projector is already divided by feature scale square,
      //
      const double geometric_wgt = m_est_->wgt( scaled_err, geometric_scale );

      DebugMacro_abv(1, scaled_err << "\t " << geometric_wgt << "\t " );

      double signature_wgt = 1.0;
      if ( signature_precomputed_ ) {
        signature_wgt = titr . signature_weight( );
      }
      else if ( use_signature_error_ ) {
        vnl_vector<double> error_vector = to_feature->signature_error_vector( *mapped_from );
        assert ( error_vector.size() > 0 );
        double signature_err = vcl_sqrt( dot_product( error_vector * signature_inv_covar, error_vector ) );
        // CS: we may need to add some chi-squared normalization here for large signature vectors.
        signature_wgt = m_est_->wgt( signature_err );  // already normalized at this point
      }

      double cumul_wgt = geometric_wgt * signature_wgt;

      DebugMacro_abv(1, cumul_wgt << vcl_endl );

      titr.set_geometric_weight( geometric_wgt );
      if ( !signature_precomputed_ ) titr.set_signature_weight( signature_wgt );
      titr.set_cumulative_weight( cumul_wgt );
      sum_weights += cumul_wgt;
    }

    // Now, assign the cumulative weights for each match by scaling
    // the initial cumulative weight by the fraction of the total
    // cumulative weight.
    // If weight_more_on_distinct_match_ is set,
    // a feature with more matches will be penalized for the
    // ambiguity, and a feature with a unique match will not be
    // affected at all.

    if ( sum_weights > 1e-16 ) { //sum_weights not approaching 0
      if ( weight_more_on_distinct_match_ )
        for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
          double wgt = titr.cumulative_weight();
            titr.set_cumulative_weight( wgt*wgt / sum_weights );
        }
      else
        for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
          double wgt = titr.cumulative_weight();
            titr.set_cumulative_weight( wgt / sum_weights );
        }
    }
  }
}

// It is unclear how to handle multiple matches.
// The current method is to pick the one with minimum distance
// and evaluate the likelihood based on that.
//
double
rgrl_weighter_indiv_scale::
aux_sum_rho_values( rgrl_scale const&  scale,
                    rgrl_match_set&    match_set,
                    rgrl_transformation const&  xform )
{
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  if ( match_set.from_size() == 0 ) return 0;

  double sum_rho = 0;

  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr )
  {
    if ( fitr.size() == 0 )  continue;

    rgrl_feature_sptr mapped_from = fitr.from_feature()->transform( xform );
    to_iter titr = fitr.begin();
    double min_val = titr.to_feature()->geometric_error( *mapped_from );
    double fea_scale = titr.to_feature()->scale();

    for ( ++titr; titr != fitr.end(); ++titr ) {
      //  for each match with a "to" image feature
      rgrl_feature_sptr to_feature = titr.to_feature();
      const double scaled_err = to_feature->geometric_error( *mapped_from );

      // signature weight
      //
      //GY: don't know how to handle this in a correct way
      // double signature_wgt = 1.0;
      //if ( signature_precomputed_ ) {
      //  signature_wgt = titr . signature_weight( );
      //}

      if ( min_val > scaled_err ) {
        min_val = scaled_err;
        fea_scale = to_feature->scale();
      }
    }
    // sum of rho is weighted by signature??
    sum_rho += vcl_log(fea_scale) +
               m_est_->rho(min_val, scale.geometric_scale()*fea_scale);
  }

  return sum_rho;
}
