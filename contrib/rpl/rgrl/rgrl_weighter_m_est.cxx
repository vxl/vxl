#include "rgrl_weighter_m_est.h"
//:
// \file
// \author Chuck Stewart
// \date   Feb 2003

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <rrel/rrel_m_est_obj.h>

#include "rgrl_match_set.h"
#include "rgrl_scale.h"
#include "rgrl_transformation.h"

rgrl_weighter_m_est::
rgrl_weighter_m_est( vcl_auto_ptr<rrel_m_est_obj>  m_est,
                     bool                          use_signature_error,
                     bool                          use_precomputed_signature_wgt )
  : m_est_( m_est ),
    use_signature_error_( use_signature_error ),
    signature_precomputed_( use_precomputed_signature_wgt )
{
}


rgrl_weighter_m_est::
~rgrl_weighter_m_est()
{
}


void
rgrl_weighter_m_est::
compute_weights( rgrl_scale const&  scales,
                 rgrl_match_set&    match_set ) const
{
  //  Cache the inverse covariance of the signature error if signature
  //  errors are used.  Be careful of null entries.

  vnl_matrix<double> signature_inv_covar;
  if ( use_signature_error_ && !signature_precomputed_ ) {
    assert ( scales.has_signature_covar() );
    vnl_svd<double> svd( scales.signature_covar() );
    svd.zero_out_absolute();
    signature_inv_covar = svd.inverse();   // pseudo-inverse at this point
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
      //double geometric_err = mapped_from->geometric_error( *to_feature );
      double geometric_err = to_feature->geometric_error( *mapped_from );
      double geometric_wgt = m_est_->wgt( geometric_err, geometric_scale );

      DebugMacro_abv(1, geometric_err << "\t " << geometric_wgt << "\t " );

      double signature_wgt = 1.0;
      if ( signature_precomputed_ ) {
        signature_wgt = titr . signature_weight( );
      }
      else if ( use_signature_error_ ) {
        vnl_vector<double> error_vector = to_feature->signature_error_vector( *mapped_from );
        assert ( error_vector.size() > 0 );
        double signature_err = vcl_sqrt( dot_product( error_vector * signature_inv_covar, error_vector ) );
        // CS: we may need to add some chi-squared normalization
        // here for large signature vectors.
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
    // So a feature with more matches will be penalized for the
    // ambiguity, and a feature with a unique match will not be
    // affected at all.

    if ( sum_weights > 1e-16 ) { //sum_weights not approaching 0
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        double wgt = titr.cumulative_weight();
        titr.set_cumulative_weight( wgt*wgt / sum_weights );
      }
    }
  }
}

double 
rgrl_weighter_m_est::
aux_sum_weighted_residuals( rgrl_scale const&  scale,
                            rgrl_match_set&    match_set,
                            rgrl_transformation const&  xform )
{
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  if ( match_set.from_size() > 0) {
    match_set.remap_from_features( xform );
    compute_weights( scale, match_set );
  }
  
  double weighted_sum = 0;
  
  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr ){
      if ( fitr.size() == 0 )  continue;
      
      rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        //  for each match with a "to" image feature
        rgrl_feature_sptr to_feature = titr.to_feature();
        double geometric_err = to_feature->geometric_error( *mapped_from );
        
        weighted_sum += vnl_math_sqr(geometric_err) * titr.geometric_weight();
      }
  }
  
  return weighted_sum;
}

double
rgrl_weighter_m_est::
aux_sum_rho_values( rgrl_scale const&  scale,
                    rgrl_match_set&    match_set,
                    rgrl_transformation const&  xform )
{
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  if ( match_set.from_size() == 0 ) return 0;

  double sum_rho = 0;

  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr ){
      if ( fitr.size() == 0 )  continue;
      
      rgrl_feature_sptr mapped_from = fitr.mapped_from_feature();
      for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        //  for each match with a "to" image feature
        rgrl_feature_sptr to_feature = titr.to_feature();
        double geometric_err = to_feature->geometric_error( *mapped_from );
        
        sum_rho += m_est_->rho(geometric_err, scale.geometric_scale());
      }
  }
  
  return sum_rho;
}

double 
rgrl_weighter_m_est::
aux_neg_log_likelihood( rgrl_scale const&  scale,
                        rgrl_match_set&    match_set,
                        rgrl_transformation const&  xform )
{
  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  int n = 0;

  for ( from_iter fitr = match_set.from_begin(); fitr != match_set.from_end(); ++fitr )
    if( fitr.size() > 0 ) {
      // multi-match counts as one constraint
      n ++;
    }

  double sum_rho_values = aux_sum_rho_values(scale, match_set, xform);
  double geometric_scale = scale.geometric_scale();
  return n*vcl_log(geometric_scale) + sum_rho_values;
}
