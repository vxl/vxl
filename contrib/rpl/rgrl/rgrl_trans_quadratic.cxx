#include "rgrl_trans_quadratic.h"
//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <rgrl/rgrl_util.h>

rgrl_trans_quadratic::
rgrl_trans_quadratic( unsigned int dim)
  : Q_( vnl_matrix<double>( dim, dim + int(dim*(dim-1)/2), 0.0 ) ),
    A_( vnl_matrix<double>( dim, dim, vnl_matrix_identity ) ),
    trans_( vnl_vector<double>( dim, 0.0 ) ),
    from_centre_( dim, 0.0 )
{
}

rgrl_trans_quadratic::
rgrl_trans_quadratic( vnl_matrix<double> const& in_Q,
                      vnl_matrix<double> const& in_A,
                      vnl_vector<double> const& in_trans,
                      vnl_matrix<double> const& in_covar )
  : rgrl_transformation( in_covar ),
    Q_( in_Q ),
    A_( in_A ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  assert ( Q_.rows() + Q_.rows()*(Q_.rows()-1)/2 == Q_.cols() );
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == A_.rows()* (Q_.cols()+A_.cols() + 1 ) );
  }
}

rgrl_trans_quadratic::
rgrl_trans_quadratic( vnl_matrix<double> const& in_Q,
                      vnl_matrix<double> const& in_A,
                      vnl_vector<double> const& in_trans )
  : Q_( in_Q ),
    A_( in_A ),
    trans_( in_trans ),
    from_centre_( in_trans.size(), 0.0 )
{
  int dim = trans_.size();
  assert ( Q_.rows() + Q_.rows()*(Q_.rows()-1)/2 == Q_.cols() );
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
}


rgrl_trans_quadratic::
rgrl_trans_quadratic( vnl_matrix<double> const& in_Q,
                      vnl_matrix<double> const& in_A,
                      vnl_vector<double> const& in_trans,
                      vnl_matrix<double> const& in_covar,
                      vnl_vector<double> const& in_from_centre,
                      vnl_vector<double> const& in_to_centre )
  : rgrl_transformation( in_covar ),
    Q_( in_Q ),
    A_( in_A ),
    trans_( in_trans ),
    from_centre_( in_from_centre )
{
  assert ( Q_.rows() + Q_.rows()*(Q_.rows()-1)/2 == Q_.cols() );
  assert ( A_.rows() == A_.cols() );
  assert ( A_.rows() == trans_.size() );
  assert ( from_centre_.size() == trans_.size() );
  if( is_covar_set() ) {
    assert ( covar_.rows() == covar_.cols() );
    assert ( covar_.rows() == A_.rows()* ( Q_.cols() + A_.cols() + 1) );
  }   

  // Uncenter the transform
  vnl_vector<double> new_trans;
  map_loc( -in_from_centre, new_trans );
  trans_ = new_trans + in_to_centre;

  if (A_.rows() == 2) { //2D
    vnl_matrix<double> new_A(2,2, 0.0);
    new_A(0,0) = -2*in_from_centre[0]*Q_(0,0)-in_from_centre[1]*Q_(0,2)+A_(0,0);
    new_A(0,1) = -2*in_from_centre[1]*Q_(0,1)-in_from_centre[0]*Q_(0,2)+A_(0,1);
    new_A(1,0) = -2*in_from_centre[0]*Q_(1,0)-in_from_centre[1]*Q_(1,2)+A_(1,0);
    new_A(1,1) = -2*in_from_centre[1]*Q_(1,1)-in_from_centre[0]*Q_(1,2)+A_(1,1);
    A_ = new_A;
  }
  else { //3D,
    vnl_matrix<double> new_A(3,3, 0.0);
    double cx = in_from_centre[0];
    double cy = in_from_centre[1];
    double cz = in_from_centre[2];

    new_A(0,0) = -2*cx*Q_(0,0)-cy*Q_(0,3)-cz*Q_(0,5)+A_(0,0);
    new_A(0,1) = -2*cy*Q_(0,1)-cx*Q_(0,3)-cz*Q_(0,4)+A_(0,1);
    new_A(0,2) = -2*cz*Q_(0,2)-cy*Q_(0,4)-cx*Q_(0,5)+A_(0,2);

    new_A(1,0) = -2*cx*Q_(1,0)-cy*Q_(1,3)-cz*Q_(1,5)+A_(1,0);
    new_A(1,1) = -2*cy*Q_(1,1)-cx*Q_(1,3)-cz*Q_(1,4)+A_(1,1);
    new_A(1,2) = -2*cz*Q_(1,2)-cy*Q_(1,4)-cx*Q_(1,5)+A_(1,2);

    new_A(2,0) = -2*cx*Q_(2,0)-cy*Q_(2,3)-cz*Q_(2,5)+A_(2,0);
    new_A(2,1) = -2*cy*Q_(2,1)-cx*Q_(2,3)-cz*Q_(2,4)+A_(2,1);
    new_A(2,2) = -2*cz*Q_(2,2)-cy*Q_(2,4)-cx*Q_(2,5)+A_(2,2);

    A_ = new_A;
  }
}

void
rgrl_trans_quadratic::
set_from_center( vnl_vector<double> const& from_center )
{
  from_centre_ = from_center;
}

void
rgrl_trans_quadratic::
map_loc( vnl_vector<double> const& from,
         vnl_vector<double>      & to   ) const
{
  assert ( from.size() == A_.rows() );
  vnl_vector<double> hot = higher_order_terms(from);
  to = Q_ * hot + A_ * from + trans_;
}


void
rgrl_trans_quadratic::
map_dir( vnl_vector<double> const& from_loc,
         vnl_vector<double> const& from_dir,
         vnl_vector<double>      & to_dir    ) const
{
  assert ( from_loc.size() == A_.cols() );
  assert ( from_dir.size() == A_.cols() );
  vnl_vector<double> to_loc_begin, to_loc_end;
  this->map_loc(from_loc, to_loc_begin);
  this->map_loc(from_loc+from_dir, to_loc_end);

  to_dir = to_loc_end - to_loc_begin;
  to_dir.normalize();
}


vnl_matrix<double>
rgrl_trans_quadratic::
transfer_error_covar( vnl_vector<double> const& p  ) const
{
  unsigned const m = A_.rows();

  assert ( is_covar_set() );
  assert ( p.size() == m );
  int num_terms = m+int(m*(m-1)/2) + m +1;

  vnl_matrix<double> temp( m, m*num_terms, 0.0 );
  vnl_vector<double> p_centered = p - from_centre_;
  vnl_vector<double> hot = higher_order_terms(p_centered);

  for ( unsigned i=0; i < m; ++i ) {
    unsigned off = i*num_terms;
    for ( unsigned j=0; j < hot.size(); ++j ) {
      temp(i,off+j) = hot[j];
    }
    off += hot.size();
    for ( unsigned j=0; j < m; ++j ) {
      temp(i,off+j) = p_centered[j];
    }
    temp(i,off+m) = 1.0;
  }

  return temp * covar_ * temp.transpose();
}

vnl_matrix<double> const&
rgrl_trans_quadratic::
Q() const
{
  return Q_;
}

vnl_matrix<double> const&
rgrl_trans_quadratic::
A() const
{
  return A_;
}


vnl_vector<double> const&
rgrl_trans_quadratic::
t() const
{
  return trans_;
}

void
rgrl_trans_quadratic::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& to_delta,
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  const double epsilon = 0.01;
  const double eps_squared = epsilon*epsilon;
  int t=0;
  const int max_t = 20;  //  Generally, only one or two iterations should be needed.
  assert (to.size() == from.size());
  int m = to.size();
  vnl_vector<double> to_est = this->map_location(from);
  vnl_matrix<double> approx_A_inv;

  while ( vnl_vector_ssd(to, to_est) > eps_squared && t<max_t ) {
    t ++ ;
    // compute the inverse of the approximated affine
    vnl_matrix<double> approx_A = A_;
    vnl_matrix<double> X(Q_.cols(), Q_.rows(), 0.0); //content of X depends on m
    if (m == 2) { //2D
      X(0,0) = 2*from[0]; X(1,1) = 2*from[1];
      X(2,0) = from[1]; X(2,1) = from[0];
    }
    else { //3D
      X(0,0) = 2*from[0]; X(1,1) = 2*from[1]; X(2,2) = 2*from[2];
      X(3,0) = from[1]; X(2,1) = from[0];
      X(4,1) = from[2]; X(4,2) = from[1];
      X(5,0) = from[2]; X(5,2) = from[0];
    }
    approx_A += Q_*X;
    vnl_svd<double> svd(approx_A);
    approx_A_inv = svd.inverse();

    // increase from by approx_A^-1*(to-to_est)
    from += 0.95 * approx_A_inv * (to - to_est);
    to_est = this->map_location(from);
  }
  if ( t > max_t )
    DebugMacro( 0, " rgrl_trans_quadratic::inv_map()--  no convergence\n");

  if ( initialize_next ) {
    if ( t == 0 ) { //approx_A_inv not yet calculated
      vnl_matrix<double> approx_A = A_;
      vnl_matrix<double> X(Q_.cols(), Q_.rows(), 0.0); //content of X depends on m
      if (m == 2) { //2D
        X(0,0) = 2*from[0]; X(1,1) = 2*from[1];
        X(2,0) = from[1]; X(2,1) = from[0];
      }
      else { //3D
        X(0,0) = 2*from[0]; X(1,1) = 2*from[1]; X(2,2) = 2*from[2];
        X(3,0) = from[1]; X(2,1) = from[0];
        X(4,1) = from[2]; X(4,2) = from[1];
        X(5,0) = from[2]; X(5,2) = from[0];
      }
      approx_A += Q_*X;
      vnl_svd<double> svd(approx_A);
      approx_A_inv = svd.inverse();
    }
    from_next_est = from + approx_A_inv * to_delta;
  }
}

vnl_matrix<double>
rgrl_trans_quadratic::
jacobian( vnl_vector<double> const& from_loc ) const
{
  unsigned int m = trans_.size();
  vnl_matrix<double> jacobian_Q(m, m);
  if ( m == 2 ) {
    for (unsigned int i = 0; i<m; i++) {
      jacobian_Q(i,0) = 2*Q_(i,0)*from_loc[0] + Q_(i,2)*from_loc[1];
      jacobian_Q(i,1) = 2*Q_(i,1)*from_loc[1] + Q_(i,2)*from_loc[0];
    }
  }
  else { // m == 3
    for (unsigned int i = 0; i<m; i++) {
      jacobian_Q(i,0) = 2*Q_(i,0)*from_loc[0] + Q_(i,3)*from_loc[1] + Q_(i,5)*from_loc[2];
      jacobian_Q(i,1) = 2*Q_(i,1)*from_loc[1] + Q_(i,3)*from_loc[0] + Q_(i,4)*from_loc[2];
      jacobian_Q(i,2) = 2*Q_(i,2)*from_loc[2] + Q_(i,4)*from_loc[1] + Q_(i,5)*from_loc[0];
    }
  }

  return A_ + jacobian_Q;
}

rgrl_transformation_sptr
rgrl_trans_quadratic::
scale_by( double scale ) const
{
  rgrl_trans_quadratic* quad = new rgrl_trans_quadratic( Q_/scale, A_,
                                                         trans_ * scale,
                                                         covar_);
  quad->set_from_center( from_centre_ * scale );

  return quad;
}

vnl_vector<double>
rgrl_trans_quadratic::
higher_order_terms(vnl_vector<double> p) const
{
  int m = p.size();
  vnl_vector<double> higher_terms( m + int(m*(m-1)/2));

  if (m == 3) {
    higher_terms[0] = vnl_math_sqr(p[0]);   //x^2
    higher_terms[1] = vnl_math_sqr(p[1]);   //y^2
    higher_terms[2] = vnl_math_sqr(p[2]);   //z^2
    higher_terms[3] = p[0]*p[1];       //xy
    higher_terms[4] = p[1]*p[2];       //yz
    higher_terms[5] = p[0]*p[2];       //xz
  }
  else { //m ==2
    higher_terms[0] = vnl_math_sqr(p[0]);         //x^2
    higher_terms[1] = vnl_math_sqr(p[1]);         //y^2
    higher_terms[2] = p[0]*p[1];                  //xy
  }

  return higher_terms;
}

void
rgrl_trans_quadratic::
write( vcl_ostream& os ) const
{
  // tag
  os << "QUADRATIC\n"
  // parameters
     << t().size() << vcl_endl
     << Q_<< A_ << trans_ << ' ' << from_centre_ << vcl_endl;
}

void
rgrl_trans_quadratic::
read( vcl_istream& is )
{
  int dim;

  // skip empty lines
  rgrl_util_skip_empty_lines( is );

  vcl_string str;
  vcl_getline( is, str );

  // The token should appear at the beginning of line
  if ( str.find( "QUADRATIC" ) != 0 ) {
    WarningMacro( "The tag is not QUADRATIC. reading is aborted.\n" );
    vcl_exit(10);
  }

  // input global xform
  dim=-1;
  is >> dim;
  if ( dim > 0 ) {
    Q_.set_size( dim, dim + int(dim*(dim-1)/2) );
    A_.set_size( dim, dim );
    trans_.set_size( dim );
    from_centre_.set_size( dim );
    is >> Q_ >> A_ >> trans_ >> from_centre_;
  }
}
