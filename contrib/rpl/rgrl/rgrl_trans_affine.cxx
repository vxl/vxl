#include "rgrl_trans_affine.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  Modifications
//   Sep 2003 Charlene Tsai, added inv_map(.) functions.
// \endverbatim

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vnl/algo/vnl_svd.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_affine::
rgrl_trans_affine( unsigned int dimension )
  : A_( vnl_matrix<double>( dimension, dimension, vnl_matrix_identity ) ),
    trans_( vnl_vector<double>( dimension, 0.0 ) ),
    from_centre_( dimension, 0.0 )
{
}


rgrl_trans_affine::
rgrl_trans_affine( vnl_matrix<double> const& in_A,
                   vnl_vector<double> const& in_trans,
                   vnl_matrix<double> const& in_covar )
  : rgrl_transformation( in_covar ),
    A_( in_A ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  if ( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == A_.rows()* (A_.rows()+1) );
  }
}

rgrl_trans_affine::
rgrl_trans_affine( vnl_matrix<double> const& in_A,
                   vnl_vector<double> const& in_trans )
  : A_( in_A ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
}

rgrl_trans_affine::
rgrl_trans_affine( vnl_matrix<double> const& in_A,
                   vnl_vector<double> const& in_trans,
                   vnl_matrix<double> const& in_covar,
                   vnl_vector<double> const& in_from_centre,
                   vnl_vector<double> const& in_to_centre )
  : rgrl_transformation( in_covar ),
    A_( in_A ),
    trans_( in_trans + in_to_centre ),
    from_centre_( in_from_centre )
{
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  assert ( from_centre_.size() == trans_.size() );
  if ( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == A_.rows()* (A_.rows()+1) );
  }
}


void
rgrl_trans_affine::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to   ) const
{
  assert ( from.size() == A_.rows() );
  to = A_ * (from-from_centre_) + trans_;
}


void
rgrl_trans_affine::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
{
  assert ( from_loc.size() == A_.cols() );
  assert ( from_dir.size() == A_.cols() );
  to_dir = A_ * from_dir;
  to_dir.normalize();
}


vnl_matrix<double>
rgrl_trans_affine::
transfer_error_covar( vnl_vector<double> const& p  ) const
{
  unsigned const m = A_.rows();

  assert ( is_covar_set() );
  assert ( p.size() == m );

  vnl_matrix<double> temp( m, m*(m+1), 0.0 );
  for ( unsigned i=0; i < m; ++i ) {
    unsigned off = i*(m+1);
    for ( unsigned j=0; j < m; ++j ) {
      temp(i,off+j) = p[j] - from_centre_[j];
    }
    temp(i,off+m) = 1.0;
  }

  return temp * covar_ * temp.transpose();
}

vnl_matrix<double> const&
rgrl_trans_affine::
A() const
{
  return A_;
}


vnl_vector<double>
rgrl_trans_affine::
t() const
{
  return trans_ - A_ * from_centre_;
}


void
rgrl_trans_affine::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  const double epsilon = 0.01;
  vnl_vector<double> to_est = this->map_location(from);

  // compute the inverse of the Jacobian, which is the A_^-1
  vnl_svd<double> svd( A_ );
  vnl_matrix<double> J_inv = svd.inverse();

  // update from to become true inv_map of to, based on A^-1 and (to - to_est)
  if (vnl_vector_ssd(to, to_est) > epsilon*epsilon) {
    from += J_inv * (to - to_est);
  }
  if ( initialize_next ) {
    from_next_est = from + (J_inv * to_delta);
  }
}


void
rgrl_trans_affine::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  vnl_svd<double> svd( A_ );
  from = svd.inverse()*to - svd.inverse()*trans_ + from_centre_;
}

rgrl_transformation_sptr
rgrl_trans_affine::
inverse_transform( ) const
{
  vnl_svd<double> svd( A() );
  vnl_matrix<double> invA = svd.inverse();
  rgrl_transformation_sptr result = new rgrl_trans_affine( invA, -invA * t() );

  const unsigned m = scaling_factors_.size();
  if ( m > 0 ) {
    vnl_vector<double> scaling( m );
    for ( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}

vnl_matrix<double>
rgrl_trans_affine::
jacobian( vnl_vector<double> const& /*from_loc*/ ) const
{
  return A_;
}


rgrl_transformation_sptr
rgrl_trans_affine::
scale_by( double scale ) const
{
  return new rgrl_trans_affine( A_, trans_ * scale,
                                covar_, from_centre_ * scale,
                                vnl_vector<double>(from_centre_.size(), 0.0) );
}


void
rgrl_trans_affine::
write( vcl_ostream& os ) const
{
  // tag
  os << "AFFINE\n"
  // parameters
     << trans_.size() << vcl_endl
     << A_ << trans_ << ' ' << from_centre_ << vcl_endl;
}


void
rgrl_trans_affine::
read( vcl_istream& is )
{
  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  if ( str.find("AFFINE") != 0 ) {
    WarningMacro( "The tag is not AFFINE. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  int dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    A_.set_size( dim, dim );
    trans_.set_size( dim );
    from_centre_.set_size( dim );
    is >> A_ >> trans_ >> from_centre_;
  }
}
