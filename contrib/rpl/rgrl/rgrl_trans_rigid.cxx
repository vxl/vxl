#include "rgrl_trans_rigid.h"
//:
// \file
// \author Tomasz Malisiewicz
// \date   March 2004

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_rigid::
rgrl_trans_rigid( unsigned int dimension )
  : R_( vnl_matrix<double>( dimension, dimension, vnl_matrix_identity ) ),
    trans_( vnl_vector<double>( dimension, 0.0 ) )
{
  // only accept 2d or 3d rigid transformation
  assert (dimension==2 || dimension==3);
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
  if( is_covar_set() ) {
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

  double cos_a = vcl_cos(alpha), sin_a = vcl_sin(alpha),
         cos_t = vcl_cos(theta), sin_t = vcl_sin(theta),
         cos_p = vcl_cos(phi),   sin_p = vcl_sin(phi);
  R_(0,0) =cos_a*cos_t ;                   R_(0,1) = -cos_a*sin_t;                   R_(0,2)=sin_a;
  R_(1,0) =cos_t*sin_a*sin_p+cos_p*sin_t;  R_(1,1) = -sin_a*sin_p*sin_t+cos_p*cos_t; R_(1,2)=-cos_a*sin_p;
  R_(2,0)=-cos_p*cos_t*sin_a+sin_p*sin_t;  R_(2,1) = cos_p*sin_a*sin_t+cos_t*sin_p;  R_(2,2)=cos_a*cos_p;
}

void rgrl_trans_rigid::set_rotation(double theta)
{
  assert ( trans_.size() == 2);

  R_(0,0) = vcl_cos(theta);    R_(0,1) = vcl_sin(theta);
  R_(1,0) = -vcl_sin(theta);   R_(1,1) = vcl_cos(theta);
}

void rgrl_trans_rigid::determine_angles(double& phi, double& alpha, double& theta) const
{
  assert (trans_.size() == 3);

  alpha = vcl_asin( R_(0,2) );

  if (R_(0,0) * vcl_cos(alpha) > 0)
  {
    theta = vcl_atan( -1 * R_(0,1)/R_(0,0) );
  }
  else
  {
    theta = vcl_atan( -1 * R_(0,1)/R_(0,0) ) + vnl_math::pi;
  }

  if (R_(2,2) * vcl_cos(alpha) > 0 )
  {
    phi = vcl_atan( -1 * R_(1,2) / R_(2,2) );
  }
  else
  {
    phi = vcl_atan( -1 * R_(1,2) / R_(2,2) ) + vnl_math::pi;
  }
}

void rgrl_trans_rigid::determine_angles( double& theta ) const
{
  assert (trans_.size() == 2);
  theta = vcl_asin( R_(0,1) );
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
  Rthetad(0,0) = -vcl_sin(theta);
  Rthetad(0,1) = -vcl_cos(theta);
  Rthetad(1,0) = vcl_cos(theta);
  Rthetad(1,1) = -vcl_sin(theta);

  Ralphad(0,0) = -vcl_sin(alpha);
  Ralphad(0,2) = vcl_cos(alpha);
  Ralphad(2,0) = -vcl_cos(alpha);
  Ralphad(2,2) = -vcl_sin(alpha);

  Rphid(1,1) = -vcl_sin(phi);
  Rphid(1,2) = -vcl_cos(phi);
  Rphid(2,1) = vcl_cos(phi);
  Rphid(2,2) = -vcl_sin(phi);

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
  const double epsilon = 0.01;
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

vnl_matrix<double>
rgrl_trans_rigid::
jacobian( vnl_vector<double> const& /*from_loc*/ ) const
{
  return R_;
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
  if( m > 0 ) {
    vnl_vector<double> scaling( m );
    for( unsigned int i=0; i<m; ++i )
      scaling[i] = 1.0 / scaling_factors_[i];
    result->set_scaling_factors( scaling );
  }

  return result;
}

rgrl_transformation_sptr
rgrl_trans_rigid::
scale_by( double scale ) const
{
  return new rgrl_trans_rigid( R_, trans_ * scale,
                                    covar_);
}

void
rgrl_trans_rigid::
write( vcl_ostream& os ) const
{
  // tag
  os << "RIGID\n"
  // parameters
     << trans_.size() << vcl_endl
     << R_ << trans_ << vcl_endl;
}

void
rgrl_trans_rigid::
read( vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "RIGID" ) != 0 ) {
    WarningMacro( "The tag is not RIGID. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    R_.set_size( dim, dim );
    trans_.set_size( dim );
    is >> R_ >> trans_;
  }
}
