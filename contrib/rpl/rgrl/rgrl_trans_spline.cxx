//:
// \file
// \autor Lee, Ying-Lin (Bess)
// \date  Sept 2003

#include "rgrl_trans_spline.h"
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_trans_reader.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

rgrl_trans_spline::
rgrl_trans_spline( unsigned int dim )
  : x0_( dim, 0.0 ), delta_( dim, 0.0 )
{
}

rgrl_trans_spline::
rgrl_trans_spline( vcl_vector<rgrl_spline_sptr> const& splines,
                   vnl_vector< double > const& x0, vnl_vector< double > const& delta,
                   rgrl_transformation_sptr xform )
  : xform_( xform ), splines_( splines ),
    x0_( x0 ), delta_( delta )
{
  assert( x0_.size() == delta_.size() );
  // covar_ = vnl_matrix< double >( splines[0]->num_of_control_points(), splines[0]->num_of_control_points(), 0.0 );
}

rgrl_trans_spline::
rgrl_trans_spline( vcl_vector<rgrl_spline_sptr> const& splines,
                   vnl_vector< double > const& x0, vnl_vector< double > const& delta,
                   vnl_matrix< double > const& covar,
                   rgrl_transformation_sptr xform )
  : rgrl_transformation( covar ),
    xform_( xform ), splines_( splines ), x0_( x0 ), delta_( delta )
{
  assert( x0_.size() == delta_.size() );
}

void
rgrl_trans_spline::
point_in_knots( vnl_vector< double > const& point, vnl_vector< double > & spline_pt ) const
{
  spline_pt.set_size( point.size() );
  for ( unsigned i = 0; i < point.size(); ++i ) {
    spline_pt[ i ] = ( point[ i ] - x0_[ i ] ) / delta_[ i ];
  }
}

void
rgrl_trans_spline::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double> & to) const
{
  unsigned dim = from.size();
  assert(dim == splines_.size());
  vnl_vector< double > to_no_spline( from );

  if ( xform_ )
    xform_->map_location( from, to_no_spline );

  vnl_vector< double > from_in_spline;
  point_in_knots( from, from_in_spline );
  vnl_vector<double> delta( dim );
  for (unsigned i=0; i<dim; ++i)
    delta[i] = splines_[i]->f_x( from_in_spline );

  to = to_no_spline + delta;
}

void
rgrl_trans_spline::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double> & to_dir) const
{
  unsigned dim = x0_.size();
  vnl_matrix< double > spline_jacobian( dim, dim );

  for ( unsigned i=0; i<dim; ++i )
    spline_jacobian.set_row( i, splines_[ i ]->jacobian( from_loc ) );

  // ???? -Gehua
  // Don't understand why plus
  // to_dir = jacobian * from_dir + from_dir;

  // Gehua's way:
  // to_dir = jacobian * from_dir
  // but jacobian = xform_jacobian + spline_jacobian
  to_dir = (spline_jacobian + xform_->jacobian( from_loc ) ) * from_dir;

  to_dir.normalize();
}

//: Return the jacobian of the transform
vnl_matrix<double>
rgrl_trans_spline::
jacobian( vnl_vector<double> const& from_loc ) const
{
  // ???? Don't understand why map_dir is implemented but not this function
  // assert( false );
  // return vnl_matrix<double>(from_loc.size(), from_loc.size(), vnl_matrix_identity);

  unsigned dim = x0_.size();
  vnl_matrix< double > spline_jacobian( dim, dim );

  for ( unsigned i=0; i<dim; ++i )
    spline_jacobian.set_row( i, splines_[ i ]->jacobian( from_loc ) );

  // add with the jocobian of xform
  return spline_jacobian + xform_->jacobian( from_loc );
}

// ??? -Gehua
// since covariance is assumed to be identical
// in each axis, the transfer error has to be
// diagonal?
// need to be fixed
vnl_matrix<double>
rgrl_trans_spline::
transfer_error_covar( vnl_vector<double> const& p ) const
{
  const unsigned dim = p.size();
  assert(is_covar_set());
  assert(dim==splines_.size());

  vnl_matrix<double> transfer_err_cov(dim,dim,0);
  vnl_vector<double> f_prime;
  vnl_vector<double> tmp;

  for (unsigned i=0; i<dim; ++i) {
    splines_[i]->basis_response( p, f_prime );
    tmp = f_prime.pre_multiply(covar_);
    for (unsigned j=0; j<tmp.size(); ++j)
      transfer_err_cov[i][i] += f_prime[j] * tmp[j];
  }
  return transfer_err_cov;
}

void
rgrl_trans_spline::
write( vcl_ostream& os ) const
{
  // output tag
  os << "BSPLINE\n";

  // global xform
  if ( xform_ ) {
    xform_->write( os );
    os << vcl_endl;
  }

  // dim
  const unsigned int dim = x0_.size();
  os << dim << vcl_endl;
  // x0
  os << x0_ << vcl_endl;
  // deltas
  assert( delta_.size() == dim );
  os << delta_ << vcl_endl;
  // output the spline
  assert( splines_.size() == dim );
  for (unsigned int i=0; i<splines_.size(); ++i)
    os << *splines_[i] << vcl_endl;
}

void
rgrl_trans_spline::
read( vcl_istream& is )
{
  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "BSPLINE" ) != 0 ) {
    WarningMacro( "The tag is not AFFINE. reading is aborted.\n" );
    vcl_exit(10);
  }

  // read global xform
  xform_ = rgrl_trans_reader( is );

  // dimension of spline transformation
  int dim;
  dim=-1;
  is >> dim;
  vnl_vector<double> tmp;
  // x0
  x0_.set_size( dim );
  is >> x0_;
  // deltas
  delta_.set_size( dim );
  is >> delta_;

  // input of splines
  splines_.resize( dim );
  for ( int i=0; i<dim; ++i ) {
    splines_[i] = new rgrl_spline;
    is >> *splines_[i];
  }
}

void
rgrl_trans_spline::
inv_map( vnl_vector<double> const& /*to*/,
         bool /*initialize_next*/,
         vnl_vector<double> const& /*to_delta*/,
         vnl_vector<double>& /*from*/,
         vnl_vector<double>& /*from_next_est*/) const
{
  assert ( ! "rgrl_trans_spline::inv_map() is not defined" );
}

void
rgrl_trans_spline::
inv_map( vnl_vector<double> const& /*to*/,
         vnl_vector<double>& /*from*/ ) const
{
  assert ( ! "rgrl_trans_spline::inv_map() is not defined" );
}

rgrl_transformation_sptr
rgrl_trans_spline::
inverse_transform( ) const
{
  assert ( ! "rgrl_trans_spline::inverse_transform() is not defined" );
  return 0;
}

rgrl_transformation_sptr
rgrl_trans_spline::
scale_by( double /*scale*/ ) const
{
  assert ( ! "rgrl_trans_spline::scale_by() is not defined" );
  return 0;
}
