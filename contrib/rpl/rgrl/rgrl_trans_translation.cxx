#include "rgrl_trans_translation.h"
//:
// \file
// \author Charlene Tsai
// \date   Dec 2003

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_translation::
rgrl_trans_translation( unsigned int dimension )
  : trans_( vnl_vector<double>( dimension, 0.0 ) ),
    from_centre_( dimension, 0.0 )
{
}

rgrl_trans_translation::
rgrl_trans_translation( vnl_vector<double> const& in_trans,
                        vnl_matrix<double> const& in_covar )
  : rgrl_transformation( in_covar ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == trans_.size() );
  }
}

rgrl_trans_translation::
rgrl_trans_translation( vnl_vector<double> const& in_trans )
  : trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
}

rgrl_trans_translation::
rgrl_trans_translation( vnl_vector<double> const& in_trans,
                        vnl_matrix<double> const& in_covar,
                        vnl_vector<double> const& in_from_centre,
                        vnl_vector<double> const& in_to_centre )
  : rgrl_transformation( in_covar ),
    trans_( in_trans + in_to_centre ),
    from_centre_( in_from_centre )
{ 
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == trans_.size() );
  }
  assert ( from_centre_.size() == trans_.size() );
}

void
rgrl_trans_translation::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to   ) const
{
  assert ( from.size() == trans_.size() );

  to = (from-from_centre_) + trans_;
}

void
rgrl_trans_translation::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
{
  assert ( from_loc.size() == trans_.size() );
  assert ( from_dir.size() == trans_.size() );

  to_dir = from_dir;
}

vnl_matrix<double>
rgrl_trans_translation::
transfer_error_covar( vnl_vector<double> const& p  ) const
{
  assert ( is_covar_set() );
  assert ( p.size() == trans_.size() );

  return covar_;
}

vnl_vector<double>
rgrl_trans_translation::
t() const
{
  return trans_ -  from_centre_;
}

void
rgrl_trans_translation::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  const double epsilon = 0.01;
  vnl_vector<double> to_est = this->map_location(from);

  // update from to become true inv_map of to, based on (to - to_est)
  if (vnl_vector_ssd(to, to_est) > epsilon*epsilon) {
    from += to - to_est;
  }

  if ( initialize_next ) {
    from_next_est = from + to_delta;
  }
}

void
rgrl_trans_translation::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  from = to - trans_ + from_centre_;
}

rgrl_transformation_sptr 
rgrl_trans_translation::
inverse_transform( ) const
{
  rgrl_transformation_sptr result = new rgrl_trans_translation( -t() );

  const unsigned m = scaling_factors_.size();
  if( m > 0 ) {
    vnl_vector<double> scaling( m );
    for( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }
  
  return result;
}


vnl_matrix<double>
rgrl_trans_translation::
jacobian(  vnl_vector<double> const& /*from_loc*/ ) const
{
  unsigned int m = trans_.size();
  return vnl_matrix<double> (m, m, vnl_matrix_identity);
}

rgrl_transformation_sptr
rgrl_trans_translation::
scale_by( double scale ) const
{
  return new rgrl_trans_translation( trans_ * scale,
                                     covar_, from_centre_ * scale,
                                     vnl_vector<double>(from_centre_.size(), 0.0) );
}

void
rgrl_trans_translation::
write( vcl_ostream& os ) const
{
  // tag
  os << "TRANSLATION\n"
  // parameters
     << trans_.size() << vcl_endl
     << trans_ << ' ' << from_centre_ << vcl_endl;
}

void
rgrl_trans_translation::
read( vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "TRANSLATION" ) != 0 ) {
    WarningMacro( "The tag is not TRANSLATION. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    trans_.set_size( dim );
    from_centre_.set_size( dim );
    is >> trans_ >> from_centre_;
  }
}
