#include <iostream>
#include "rgrl_trans_couple.h"
//:
// \file
// \brief a class that encapsulates a pair of transformations: forward & backward
// \author Gehua Yang
// \date Feb 2005

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <rgrl/rgrl_trans_reader.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_couple::
  rgrl_trans_couple( rgrl_transformation_sptr const& forward, rgrl_transformation_sptr const& backward )
  : rgrl_transformation( *forward ),
    forward_xform_( forward ),
    backward_xform_( backward )
{
}

rgrl_trans_couple::
~rgrl_trans_couple() = default;

void
rgrl_trans_couple::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to    ) const
{
  assert( forward_xform_ );
  forward_xform_ -> map_location( from, to );
}

void
rgrl_trans_couple::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
{
  assert( forward_xform_ );
  forward_xform_ -> map_direction( from_loc, from_dir, to_dir );
}

void
rgrl_trans_couple::
map_tangent( vnl_vector<double> const& from_loc,
             vnl_vector<double> const& from_dir,
             vnl_vector<double>      & to_dir    ) const
{
  assert( forward_xform_ );
  forward_xform_ -> map_tangent( from_loc, from_dir, to_dir );
}

void
rgrl_trans_couple::
map_normal( vnl_vector<double> const & from_loc,
            vnl_vector<double> const & from_dir,
            vnl_vector<double>       & to_dir    ) const
{
  assert( forward_xform_ );
  forward_xform_ -> map_normal( from_loc, from_dir, to_dir );
}

void
rgrl_trans_couple::
map_normal( vnl_vector<double> const  & from_loc,
            vnl_vector<double> const  & from_dir,
            vnl_matrix< double > const& tangent_subspace,
            vnl_vector<double>        & to_dir    ) const
{
  assert( forward_xform_ );
  forward_xform_ -> map_normal( from_loc, from_dir, tangent_subspace, to_dir );
}

vnl_matrix<double>
rgrl_trans_couple::
transfer_error_covar( vnl_vector<double> const& p ) const
{
  assert( forward_xform_ );
  return forward_xform_ -> transfer_error_covar( p );
}

void
rgrl_trans_couple::
jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const
{
  assert( forward_xform_ );
  forward_xform_ -> jacobian_wrt_loc( jac, from_loc );
}

rgrl_transformation_sptr
rgrl_trans_couple::
scale_by( double scale ) const
{
  assert( forward_xform_ );
  return forward_xform_ -> scale_by( scale );
}

//: output transformation
void
rgrl_trans_couple::
write( std::ostream& os ) const
{
  os << "COUPLE_TRANS" << std::endl;

  assert( forward_xform_ && backward_xform_ );

  forward_xform_ -> write( os );
  backward_xform_ -> write( os );
}

//: input transformation
bool
rgrl_trans_couple::
read( std::istream& is )
{
  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  std::string str;
  std::getline( is, str );

  if ( str.find("COUPLE_TRANS") != 0 ) {
    WarningMacro( "The tag is not COUPLE_TRANS. reading is aborted.\n" );
    return false;
  }

  // Read forward and backward
  forward_xform_ = rgrl_trans_reader::read( is );
  backward_xform_ = rgrl_trans_reader::read( is );

  // parent
  return forward_xform_
    && backward_xform_
    && is.good()
    && rgrl_transformation::read( is );
}


//:  Inverse map with an initial guess
void
rgrl_trans_couple::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  assert( forward_xform_ );
  forward_xform_ -> inv_map( to, initialize_next, to_delta, from, from_next_est );
}

void
rgrl_trans_couple::
inv_map( const vnl_vector<double>& to,
               vnl_vector<double>& from ) const
{
  assert( forward_xform_ );
  forward_xform_ -> inv_map( to, from );
}

bool
rgrl_trans_couple::
is_invertible() const
{
  return backward_xform_;
}

rgrl_transformation_sptr
rgrl_trans_couple::
inverse_transform() const
{
  if ( backward_xform_ )
    return new rgrl_trans_couple( backward_xform_, forward_xform_ );
  else
    return nullptr;
}

//: make a clone copy
rgrl_transformation_sptr
rgrl_trans_couple::
clone() const
{
  return new rgrl_trans_couple( *this );
}
