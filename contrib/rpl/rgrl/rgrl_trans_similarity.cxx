#include "rgrl_trans_similarity.h"
#include <vnl/vnl_fastops.h>

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vnl/algo/vnl_svd.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_similarity::
rgrl_trans_similarity( unsigned int dimension )
  : A_( vnl_matrix<double>( dimension, dimension, vnl_matrix_identity ) ),
    trans_( vnl_vector<double>( dimension, 0.0 ) ),
    from_centre_( dimension, 0.0 )
{
}

rgrl_trans_similarity::
rgrl_trans_similarity( vnl_matrix<double> const& rot_and_scale,
                       vnl_vector<double> const& in_trans )
  : A_( rot_and_scale ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  // assert ( ( A_.rows() != 2 || covar_.rows() == 4 ) ); // 2d has 4 params
  // assert ( ( A_.rows() != 3 || covar_.rows() == 7 ) ); // 3d has 7 params
}

rgrl_trans_similarity::
rgrl_trans_similarity( vnl_matrix<double> const& rot_and_scale,
                       vnl_vector<double> const& in_trans,
                       vnl_matrix<double> const& in_covar )
  : rgrl_transformation( in_covar ),
    A_( rot_and_scale ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( ( A_.rows() != 2 || covar_.rows() == 4 ) ); // 2d has 4 params
    assert ( ( A_.rows() != 3 || covar_.rows() == 7 ) ); // 3d has 7 params
  }
}

rgrl_trans_similarity::
rgrl_trans_similarity( vnl_matrix<double> const& in_A,
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
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( ( A_.rows() != 2 || covar_.rows() == 4 ) ); // 2d has 4 params
    assert ( ( A_.rows() != 3 || covar_.rows() == 7 ) ); // 3d has 7 params
  }
}

void
rgrl_trans_similarity::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to   ) const
{
  assert ( from.size() == A_.rows() );
  // for efficiency, rewrite the following as:
  // to = A_ * (from-from_centre_) + trans_;
  //
  vnl_fastops::Ab( to, A_, from-from_centre_ );
  to += trans_;
}

void
rgrl_trans_similarity::
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
rgrl_trans_similarity::
transfer_error_covar( vnl_vector<double> const& p  ) const
{
  unsigned const m = A_.rows();

  assert ( is_covar_set() );
  assert ( p.size() == m && m == 2); //only deal with 2D for now

  vnl_matrix<double> temp( 2, 4, 0.0 );
  temp(0,0) = p[0]-from_centre_[0];
  temp(0,1) = from_centre_[1] - p[1];
  temp(0,2) = 1;
  temp(1,0) = p[1]-from_centre_[1];
  temp(1,1) = p[0] - from_centre_[0];
  temp(1,3) = 1;

  return temp * covar_ * temp.transpose();
}

vnl_matrix<double> const&
rgrl_trans_similarity::
A() const
{
  return A_;
}

vnl_vector<double>
rgrl_trans_similarity::
t() const
{
  return trans_ - A_ * from_centre_;
}

void
rgrl_trans_similarity::
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
rgrl_trans_similarity::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  vnl_svd<double> svd( A_ );
  from = svd.inverse()*to - svd.inverse()*trans_ + from_centre_;
}

rgrl_transformation_sptr 
rgrl_trans_similarity::
inverse_transform( ) const
{
  vnl_svd<double> svd( A() );
  vnl_matrix<double> invA = svd.inverse();
  rgrl_transformation_sptr result =  new rgrl_trans_similarity( invA, -invA * t() );

  const unsigned m = scaling_factors_.size();
  if( m > 0 ) {
    vnl_vector<double> scaling( m );
    for( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}


void
rgrl_trans_similarity::
jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& /*from_loc*/ ) const
{
  jac = A_;
}

rgrl_transformation_sptr
rgrl_trans_similarity::
scale_by( double scale ) const
{
  return new rgrl_trans_similarity( A_, trans_ * scale,
                                    covar_, from_centre_ * scale,
                                    vnl_vector<double>(from_centre_.size(), 0.0) );
}

void
rgrl_trans_similarity::
write( vcl_ostream& os ) const
{
  // tag
  os << "SIMILARITY\n"
  // parameters
     << t().size() << vcl_endl
     << A_ << trans_ << ' ' << from_centre_ << vcl_endl;

  // parent
  rgrl_transformation::write( os );
}

void
rgrl_trans_similarity::
read( vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "SIMILARITY" ) != 0 ) {
    WarningMacro( "The tag is not SIMILARITY. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    A_.set_size( dim, dim );
    trans_.set_size( dim );
    from_centre_.set_size( dim );
    is >> A_ >> trans_ >> from_centre_;
  }

  // parent
  rgrl_transformation::read( is );
}
