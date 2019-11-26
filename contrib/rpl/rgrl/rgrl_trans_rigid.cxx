#include "rgrl_trans_rigid.h"
//:
// \file
// \author Tomasz Malisiewicz
// \date   March 2004

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vnl/algo/vnl_svd.h>
#include "vnl/vnl_math.h"
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_rotation.h>

rgrl_trans_rigid::
rgrl_trans_rigid( unsigned int dimension )
  : R_( vnl_matrix<double>( dimension, dimension, vnl_matrix_identity ) ),
    trans_( vnl_vector<double>( dimension, 0.0 ) )
{
  // only accept 2d or 3d rigid transformation, it can be zero when using a reader to initialize it
  assert (dimension==0 || dimension==2 || dimension==3);
}

rgrl_trans_rigid::
rgrl_trans_rigid( vnl_matrix<double> const& rot,
                  vnl_vector<double> const& in_trans )
  : R_( rot ),
    trans_( in_trans )
{
  assert ( R_.rows() == R_.cols() );  //make sure rotation is square matrix
  assert ( R_.rows() == trans_.size() );
}

rgrl_trans_rigid::
rgrl_trans_rigid( vnl_matrix<double> const& rot,
                  vnl_vector<double> const& in_trans,
                  vnl_matrix<double> const& in_covar )
  : rgrl_transformation( in_covar ),
    R_( rot ),
    trans_( in_trans )
{
  assert ( R_.rows() == R_.cols() );
  assert ( R_.rows() == trans_.size() );
  if ( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( ( R_.rows() != 2 || covar_.rows() == 3 ) ); // 2d has 3 params (1 angle + 2 displacements)
    assert ( ( R_.rows() != 3 || covar_.rows() == 6 ) ); // 3d has 6 params (3 angles + 3 displacements)
  }
}

void rgrl_trans_rigid::set_translation(double tx, double ty, double tz)
{
  assert ( trans_.size() == 3);
  trans_[0]=tx;
  trans_[1]=ty;
  trans_[2]=tz;
}

void rgrl_trans_rigid::set_translation(double tx, double ty)
{
  assert ( trans_.size() == 2);
  trans_[0]=tx;
  trans_[1]=ty;
}

void rgrl_trans_rigid::set_rotation(double theta, double alpha, double phi)
{
  assert ( trans_.size() == 3);
        rgrl_rotation_3d_to_matrix(theta, alpha, phi, R_);
}

void rgrl_trans_rigid::set_rotation(double theta)
{
  assert ( trans_.size() == 2);

  rgrl_rotation_2d_to_matrix(theta, R_);
}

void rgrl_trans_rigid::determine_angles(double& phi, double& alpha, double& theta) const
{
  assert (trans_.size() == 3);
        rgrl_rotation_3d_from_matrix(R_, phi, alpha, theta);
}

void rgrl_trans_rigid::determine_angles( double& theta ) const
{
  assert (trans_.size() == 2);
  rgrl_rotation_2d_from_matrix(R_, theta);
}

void
rgrl_trans_rigid::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to   ) const
{
  assert ( from.size() == R_.rows() );
  to = R_ * from + trans_;
}

void
rgrl_trans_rigid::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
{
  assert ( from_loc.size() == R_.cols() );
  assert ( from_dir.size() == R_.cols() );
  to_dir = R_ * from_dir;
  to_dir.normalize();
}

vnl_matrix<double>
rgrl_trans_rigid::
transfer_error_covar( vnl_vector<double> const& p  ) const
{
  assert ( is_covar_set() );
  assert ( p.size() == trans_.size() );
  vnl_matrix<double> jacobian(3,6,0.0);

  double phi,alpha,theta;

  determine_angles(phi,alpha,theta);

  vnl_matrix<double> Rphid(3,3,0.0);
  vnl_matrix<double> Ralphad(3,3,0.0);
  vnl_matrix<double> Rthetad(3,3,0.0);

  // derivative matrices now
  Rthetad(0,0) = -std::sin(theta);
  Rthetad(0,1) = -std::cos(theta);
  Rthetad(1,0) = std::cos(theta);
  Rthetad(1,1) = -std::sin(theta);

  Ralphad(0,0) = -std::sin(alpha);
  Ralphad(0,2) = std::cos(alpha);
  Ralphad(2,0) = -std::cos(alpha);
  Ralphad(2,2) = -std::sin(alpha);

  Rphid(1,1) = -std::sin(phi);
  Rphid(1,2) = -std::cos(phi);
  Rphid(2,1) = std::cos(phi);
  Rphid(2,2) = -std::sin(phi);

  jacobian.set_column(0,Rthetad*p);
  jacobian.set_column(1,Ralphad*p);
  jacobian.set_column(2,Rphid*p);
  jacobian.update(vnl_matrix<double>(3,3,vnl_matrix_identity),0,3);

  return jacobian * covar_ * jacobian.transpose();
}

vnl_matrix<double> const&
rgrl_trans_rigid::
R() const
{
  return R_;
}

vnl_vector<double>
rgrl_trans_rigid::
t() const
{
  return trans_;
}

void
rgrl_trans_rigid::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  constexpr double epsilon = 0.01;
  vnl_vector<double> to_est = this->map_location(from);

  // compute the inverse of the Jacobian, which is the R_^-1
  vnl_svd<double> svd( R_ );
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
rgrl_trans_rigid::
jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& /*from_loc*/ ) const
{
  jac = R_;
}

void
rgrl_trans_rigid::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  vnl_svd<double> svd( R_ );
  from = svd.inverse()*to - svd.inverse()*trans_;
}

rgrl_transformation_sptr
rgrl_trans_rigid::
inverse_transform( ) const
{
  vnl_matrix<double> invR = R().transpose();
  rgrl_transformation_sptr result = new rgrl_trans_rigid( invR, -invR * t() );

  const unsigned m = scaling_factors_.size();
  if ( m > 0 ) {
    vnl_vector<double> scaling( m );
    for ( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}

rgrl_transformation_sptr
rgrl_trans_rigid::
scale_by( double scale ) const
{
  rgrl_transformation_sptr xform
    = new rgrl_trans_rigid( R_, trans_ * scale,
                            covar_ );
  xform->set_scaling_factors( this->scaling_factors() );
  return xform;
}

void
rgrl_trans_rigid::
write( std::ostream& os ) const
{
  // tag
  os << "RIGID\n"
  // parameters
     << trans_.size() << std::endl
     << R_ << trans_ << std::endl;

  // parent
  rgrl_transformation::write( os );
}

bool
rgrl_trans_rigid::
read( std::istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  std::string str;
  std::getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "RIGID" ) != 0 ) {
    WarningMacro( "The tag is not RIGID. reading is aborted.\n" );
    return false;
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    R_.set_size( dim, dim );
    trans_.set_size( dim );
    is >> R_ >> trans_;
  }

  // parent
  return is.good() && rgrl_transformation::read( is );
}

//: make a clone copy
rgrl_transformation_sptr
rgrl_trans_rigid::
clone() const
{
  return new rgrl_trans_rigid( *this );
}
