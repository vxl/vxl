#include "rgrl_trans_mixed_spline.h"

void
rgrl_trans_mixed_spline::
map_loc( vnl_vector< double > const& from,
         vnl_vector< double > & to ) const
{
  vnl_vector< double > temp_to;
  to.set( 0 );

  for( unsigned i = 0; i < trans_splines_.size(); ++i ) {
    trans_splines_[i].map_location( from, temp_to );
    to += temp_to;
  }
}

void
rgrl_trans_mixed_spline::
map_dir( vnl_vector< double > const& from_loc,
         vnl_vector< double > const& from_dir,
         vnl_vector< double > & to_dir ) const
{
  vnl_vector<double> delta;
  vnl_vector<double> delta2;

  map_loc( from_loc, delta );
  // There is a potential bug:
  // if from2 locates outside the spline boundary, the result won't be correct.
  vnl_vector<double> from2 = from_loc + from_dir / from_dir.two_norm() ;
  //vnl_vector<double> from2 = from_loc + from_dir;

  map_loc( from2, delta2 );
  to_dir = from2 + delta2 - from_loc - delta ;  
}

vnl_matrix<double>
rgrl_trans_mixed_spline::
transfer_error_covar( vnl_vector<double> const& p ) const
{
  unsigned dim = p.size();
  vnl_matrix<double> transfer_err_cov(dim, dim, 0);
  
  for( unsigned i = 0; i < trans_splines_.size(); ++i ) {
    transfer_err_cov += trans_splines_[i].transfer_error_covar( p );
  }

  return transfer_err_cov;
}
