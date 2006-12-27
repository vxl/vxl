#include "rgrl_trans_mixed_spline.h"
#include <vcl_cassert.h>

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

//: Compute jacobian w.r.t. location
void
rgrl_trans_mixed_spline::
jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const
{
  assert( !"rgrl_trans_mixed_spline::jacobian_wrt_loc() is not implemented!" );
}

rgrl_transformation_sptr
rgrl_trans_mixed_spline::
scale_by( double scale ) const
{
  assert( !"rgrl_trans_mixed_spline::scale_by() is not implemented!" );
  return 0;
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

//: make a clone copy
rgrl_transformation_sptr 
rgrl_trans_mixed_spline::
clone() const
{
  return new rgrl_trans_mixed_spline( *this );
}
