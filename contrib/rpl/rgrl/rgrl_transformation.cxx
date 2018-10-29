#include <iostream>
#include "rgrl_transformation.h"
//:
// \file
// \brief Base class for transformation representation, estimations and application in generalized registration library
// \author Chuck Stewart
// \date 15 Nov 2002

#include <rgrl/rgrl_util.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vcl_compiler.h>
#include <cassert>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

rgrl_transformation::
rgrl_transformation( const vnl_matrix<double>& cov )
  : is_covar_set_(false)
{
  set_covar( cov );
}

rgrl_transformation::
~rgrl_transformation() = default;

void
rgrl_transformation::
map_location( vnl_vector<double> const& from,
              vnl_vector<double>      & to    ) const
{
  map_loc( from, to );
}

vnl_vector<double>
rgrl_transformation::
map_location( vnl_vector<double> const& p ) const
{
  vnl_vector<double> result( p.size() );
  map_loc( p, result );
  return result;
}

void
rgrl_transformation::
map_direction( vnl_vector<double> const& from_loc,
               vnl_vector<double> const& from_dir,
               vnl_vector<double>      & to_dir    ) const
{
  map_dir( from_loc, from_dir, to_dir );
}

void
rgrl_transformation::
map_tangent( vnl_vector<double> const& from_loc,
             vnl_vector<double> const& from_dir,
             vnl_vector<double>      & to_dir    ) const
{
  vnl_matrix<double> J;
  this->jacobian_wrt_loc( J, from_loc );
  assert ( from_loc.size() == J.cols() );
  assert ( from_dir.size() == J.cols() );

  to_dir = J * from_dir;
  to_dir.normalize();
}

void
rgrl_transformation::
map_normal( vnl_vector<double> const & from_loc,
            vnl_vector<double> const & from_dir,
            vnl_vector<double>       & to_dir    ) const
{
#if 0
  // generate the tangent subspace
  vnl_matrix< double > tangent_subspace;
  vnl_matrix<double> one_row( 1, from_dir.size() );
  one_row.set_row( 0, from_dir );
  vnl_svd<double> normal_svd( one_row );
  tangent_subspace = normal_svd.nullspace();
  assert ( tangent_subspace.columns() == from_dir. size() - 1 );

  // compute the transformed normal from by transforming the tangent_subspace
  map_normal( from_loc, from_dir, tangent_subspace, to_dir );
#endif

  // assuming it is face feature, i.e., with dimension n-1
  unsigned int m = from_loc.size();
  if (m == 2) //rotate the tangent by 90 degrees
  {
    // rotate from_dir
    vnl_vector< double > from_tangent(2);
    from_tangent[0] =  from_dir[1];
    from_tangent[1] = -from_dir[0];

    // map tangent
    vnl_vector< double > xformed_tangent;
    map_tangent(from_loc, from_tangent, xformed_tangent);

    // rotate mapped tangent to get normal
    xformed_tangent.normalize();
    to_dir.set_size( 2 );
    to_dir[0] = -xformed_tangent[1];
    to_dir[1] =  xformed_tangent[0];
  }
  else //m == 3, compute the normal as the cross-product of the two xformed tangents
  {
    // avoid using SVD to
    // obtain two tangent bases
    // Description of this somewhat naive method:
    // In order to obtain the 1st tangent basis,
    // given normal vector n, 3D orthogonality constraint is:
    // t^T * n = 0
    // Now, if we set one particular element as 0(suppose it is the first one)
    // what is left in the constraint is 0 + t2*n2 + t3*n3 = 0
    // which is essentially 2D.
    // We can solve scalars t2 and t3 using the above method.
    // Then we obtain the first tangent [0 t2 t3]
    // We can use cross product to obtain the second tangent.
    // The only tricky part is which element to pick to set as 0!
    // The solution is to pick the element with smallest magnitude
    // Why? Think about a normal vector in such a form [1 0.2 0].
    vnl_double_3 from_tangent0;
    vnl_double_3 from_tangent1;
    // find the element with smallest magnitude
    unsigned int min_index=0;
    double min = std::abs(from_dir[0]);
    for ( unsigned int i=1; i<3; i++)
      if ( std::abs(from_dir[i]) < min ) {
        min = std::abs( from_dir[i] );
        min_index = i;
      }

    // shrink it to 2D, by removing that smallest element.
    vnl_double_2 t, n;
    for (unsigned int i=0,j=0; i<3; ++i)
      if ( i != min_index )
        n[j++] = from_dir[i];
    // 2D orthogonality constraint
    t[0] =  n[1];
    t[1] = -n[0];
    // fill it back to 3D, with the corresponding smallest
    // element set as zero
    for (unsigned int i=0,j=0; i<3; ++i)
      if ( i != min_index )
        from_tangent0[i] = t[j++];
      else
        from_tangent0[i] = 0.0;

    // it is easier for the second one
    from_tangent1 = vnl_cross_3d( vnl_double_3(from_dir), from_tangent0 );

    // transform tangent dir
    vnl_vector< double > xformed_tangent0;
    vnl_vector< double > xformed_tangent1;
    map_tangent(from_loc, from_tangent0.as_ref(), xformed_tangent0);
    map_tangent(from_loc, from_tangent1.as_ref(), xformed_tangent1);
    to_dir = vnl_cross_3d( xformed_tangent0, xformed_tangent1 );
    to_dir.normalize();
  }
}

void
rgrl_transformation::
map_normal( vnl_vector<double> const  & from_loc,
            vnl_vector<double> const  & /*from_dir*/,
            vnl_matrix< double > const& tangent_subspace,
            vnl_vector<double>        & to_dir    ) const
{
#if 0
  // Transform basis tangent vectors
  vnl_matrix< double > xform_tangent_subspace = tangent_subspace.transpose();
  vnl_vector< double > xformed_tangent;
  for ( unsigned int i = 0; i< tangent_subspace.columns(); i++ ) {
    map_tangent(from_loc, tangent_subspace.get_column(i), xformed_tangent);
    xform_tangent_subspace.set_row(i, xformed_tangent);
  }

  // It is not necessary to orthogonize bases.  The reason is
  // the null space is always orthogonal to a linear combination
  // of any number of bases
  if ( tangent_subspace.columns() == 2 ) {
    // If (m == 3), make the 2 tangent vector orthogonal
    vnl_vector< double > tangent1 = xform_tangent_subspace.get_row(0);
    vnl_vector< double > tangent2 = xform_tangent_subspace.get_row(1);
    vnl_vector< double > ortho_tangent = tangent2 - inner_product(tangent2,tangent1)* tangent1;
    xform_tangent_subspace.set_row(1, ortho_tangent.normalize());
  }

  // Get the transformed normal from the xformed tangent subspace
  vnl_svd<double> tangent_svd( xform_tangent_subspace );
  assert ( tangent_svd.nullspace().columns() == 1 );
  to_dir = tangent_svd.nullspace().get_column(0);
#endif

  unsigned int m = tangent_subspace.rows();
  if (m == 2) {//rotate the tangent by 90 degrees
    vnl_vector< double > xformed_tangent;
    map_tangent(from_loc, tangent_subspace.get_column(0), xformed_tangent);
    xformed_tangent.normalize();
    to_dir.set_size( 2 );
    to_dir[0] = -xformed_tangent[1];
    to_dir[1] =  xformed_tangent[0];
  }
  else { //m == 3, compute the normal as the cross-product of the two xformed tangents
    vnl_vector< double > xformed_tangent0;
    vnl_vector< double > xformed_tangent1;
    map_tangent(from_loc, tangent_subspace.get_column(0), xformed_tangent0);
    map_tangent(from_loc, tangent_subspace.get_column(1), xformed_tangent1);
    to_dir = vnl_cross_3d( xformed_tangent0, xformed_tangent1 );
    to_dir.normalize();
  }
}

double
rgrl_transformation::
map_intensity( vnl_vector<double> const& /*from*/,
               double intensity ) const
{
  return intensity;
}

double
rgrl_transformation::
log_det_covar() const
{
  return log_det_sym_matrix( covar_ );
}

double
rgrl_transformation::
log_det_sym_matrix( vnl_matrix<double> const& m ) const
{
  assert( m.rows() && m.cols() );
  assert( m.rows() == m.cols() );

  // when computing the log of determinant,
  // because determinant is the product of all eigen-values,
  // and because a log is taken,
  // we go around it and do a sum on the log of *each* of the
  // eigen-values.

  double result = 0;
  vnl_symmetric_eigensystem<double> eig(m);
  for ( unsigned i=0; i<m.rows(); ++i )
    result += std::log( eig.get_eigenvalue(i) );
  return result;
}

double
rgrl_transformation::
log_det_covar_deficient( int rank ) const
{
  // first, scan the matrix and eliminate
  // rows and columns containing only zeros
  std::vector<unsigned int> zero_indices;
  for ( unsigned i=0; i<covar_.rows(); ++i )
    if ( !covar_(i,i) ) {

      // scan the whole row
      bool all_zero=true;
      for ( unsigned j=0; j<covar_.cols()&&all_zero; ++j )
        if ( covar_(i,j) )
          all_zero=false;

      if ( all_zero )
        zero_indices.push_back( i );
    }

  vnl_matrix<double> m;

  if ( !zero_indices.empty() ) {

    // put together a new matrix without the rows and
    // columns of zeros
    const unsigned int num = covar_.rows() - zero_indices.size();
    m.set_size( num, num );
    for ( unsigned i=0,ic=0,iz=0; i<covar_.rows(); ++i ) {

      if ( iz<zero_indices.size() && i == zero_indices[iz] ) {
        ++iz;
        continue;
      }

      for ( unsigned j=0,jc=0,jz=0; j<covar_.cols(); ++j ) {

        if ( jz<zero_indices.size() && j == zero_indices[jz] ) {
          ++jz;
          continue;
        }
        m( ic, jc ) = covar_( i, j );

        ++jc;
      }

      ++ic;
    }
  }
  else
    m = covar_;

  // compute the log of determinant with the largest [rank] eigenvalues
  const int num = m.rows();

  // by default
  if ( rank <= 0 )
    rank = num;

  double result = 0;
  vnl_symmetric_eigensystem<double> eig(m);
  for ( int i=0; i<rank; ++i )
    result += std::log( eig.get_eigenvalue(num-1-i) );
  return result;
}

//:  Parameter covariance matrix
vnl_matrix<double>
rgrl_transformation::
covar() const
{
  assert( is_covar_set_ );
  return covar_;
}

vnl_matrix<double>
rgrl_transformation::
jacobian( vnl_vector<double> const& from_loc ) const
{
  vnl_matrix<double> jac;
  this->jacobian_wrt_loc( jac, from_loc );

  return jac;
}

//:  set parameter covariance matrix
void
rgrl_transformation::
set_covar( const vnl_matrix<double>& covar )
{
  covar_ = covar;
  if ( covar.rows() > 0 && covar.cols() > 0 ) {
    is_covar_set_ = true;
  }
}

//: set scaling factors
//  Unless the transformation is not estimated using estimators in rgrl,
//  it does not need to be set explicitly
void
rgrl_transformation::
set_scaling_factors( vnl_vector<double> const& scaling )
{
  // checking scaling
  // set it to epsilon if scaling is in fact zero
  for (double i : scaling)
    assert( vnl_math::isfinite( i ) );

  scaling_factors_ = scaling;
}


std::ostream&
operator<< (std::ostream& os, rgrl_transformation const& xform )
{
  xform.write( os );
  return os;
}

//: output transformation
void
rgrl_transformation::
write( std::ostream& os ) const
{
  if ( is_covar_set() )
  {
    // write covariance
    os << "COVARIANCE\n"
       << covar_.rows() << ' ' << covar_.cols() << '\n'
       << covar_ << std::endl;
  }

  if ( scaling_factors_.size() )
  {
    // write scaling factors
    os << "SCALING_FACTORS\n"
       << scaling_factors_.size() << '\n'
       << scaling_factors_ << std::endl;
  }
}

//: input transformation
bool
rgrl_transformation::
read( std::istream& is )
{
  std::streampos pos;
  std::string tag_str;

  // skip any empty lines
  rgrl_util_skip_empty_lines( is );
  if ( !is.good() )
    return true;   // reach the end of stream

  tag_str="";
  pos = is.tellg();
  std::getline( is, tag_str );

  if ( tag_str.find("COVARIANCE") == 0 )
  {
    // read in covariance matrix
    int m=-1, n=-1;
    vnl_matrix<double> cov;

    // get dimension
    is >> m >> n;
    if ( !is || m<=0 || n<=0 )
      return false;   // cannot get the dimension

    cov.set_size(m, n);
    cov.fill(0.0);
    is >> cov;

    if ( !is.good() )
      return false;  // cannot read the covariance matrix

    this->set_covar( cov );

    // read in the next tag
    // skip any empty lines
    rgrl_util_skip_empty_lines( is );
    if ( is.eof() )
      return true;   // reach the end of stream

    tag_str="";
    pos = is.tellg();
    std::getline( is, tag_str );
  }

  if ( tag_str.find("SCALING_FACTORS") == 0 )
  {
    // read in scaling factors
    int m=-1;

    // get dimension
    is >> m;

    if ( !is || m<=0 )
      return false;  // cannot get dimension

    scaling_factors_.set_size( m );
    is >> scaling_factors_;

    return is.good();
  }

  // reset the stream pos
  is.seekg( pos );
  return is.good();
}


class inverse_mapping_func
: public vnl_least_squares_function
{
 public:
  inverse_mapping_func( rgrl_transformation const* xform_ptr, vnl_vector<double> const& to_loc )
    : vnl_least_squares_function( to_loc.size(), to_loc.size(), use_gradient ),
      xform_( xform_ptr ), to_loc_( to_loc )
  {}

  //: obj func value
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  //: Jacobian
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian) override;

 public:
  const rgrl_transformation* xform_;
  vnl_vector<double>   to_loc_;
};


void
inverse_mapping_func::
f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  assert( xform_ );

  // x is the From location
  // just need to apply forward transformation and
  xform_->map_location( x, fx );

  // get the difference
  fx -= to_loc_;
}

void
inverse_mapping_func::
gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian)
{
  assert( xform_ );

  // jacobian is just the jacobian of transformation w.r.t location
  // x is the From location
  xform_->jacobian_wrt_loc( jacobian, x );
}

//:  Inverse map with an initial guess
void
rgrl_transformation::
inv_map( const vnl_vector<double>& to,
         bool initialize_next,
         const vnl_vector<double>& /*to_delta*/, // FIXME: unused
         vnl_vector<double>& from,
         vnl_vector<double>& from_next_est) const
{
  // use different objects for different dimension
  if ( to.size() == 2 )
  {
    static inverse_mapping_func inv_map_func( this, to );

    // set transformation and the desired
    inv_map_func.to_loc_ = to;
    inv_map_func.xform_  = this;

    // solve for from location
    static vnl_levenberg_marquardt lm( inv_map_func );

    // lm.set_trace( true );
    // lm.set_check_derivatives( 5 );
    // we don't need it to be super accurate
    lm.set_f_tolerance( 1e-4 );
    lm.set_x_tolerance( 1e-3 );
    lm.set_max_function_evals( 100 );

    // run LM
    bool ret = lm.minimize_using_gradient( from );

    if ( !ret ) {
      WarningMacro( "Levenberg-Marquardt in rgrl_transformation::inv_map has failed!!!" );
      return;
    }
  }
  else if ( to.size() == 3 )
  {
    static inverse_mapping_func inv_map_func( this, to );

    // set transformation and the desired
    inv_map_func.to_loc_ = to;
    inv_map_func.xform_  = this;

    // solve for from location
    static vnl_levenberg_marquardt lm( inv_map_func );

    // lm.set_trace( true );
    // lm.set_check_derivatives( 5 );
    // we don't need it to be super accurate
    lm.set_f_tolerance( 1e-4 );
    lm.set_x_tolerance( 1e-3 );
    lm.set_max_function_evals( 100 );

    // run LM
    bool ret = lm.minimize_using_gradient( from );

    if ( !ret ) {
      WarningMacro( "Levenberg-Marquardt in rgrl_transformation::inv_map has failed!!!" );
      return;
    }
  }
  else
  {
    assert( ! "Other dimensioin is not implemented!" );
    return;
  }

  // initialize the next
  // NOTE:
  // no need to compute the inverse of jacobian here
  // because the inverse using SVD is about expensive as LM.
  // and there is extra memory allocation
  // Therefore, just initialize it as current from
  //
  if ( initialize_next ) {
    from_next_est = from;
  }
}

//:  Inverse map based on the transformation.
//   This function only exist for certain transformations.
void
rgrl_transformation::
inv_map( const vnl_vector<double>& to,
         vnl_vector<double>& from ) const
{
  const bool initialize_next = false;
  vnl_vector<double> unused;
  inv_map( to, initialize_next, unused, from, unused );
}

//: Return an inverse transformation
//  This function only exist for certain transformations.
rgrl_transformation_sptr
rgrl_transformation::
inverse_transform() const
{
  assert( !"Should never reach rgrl_transformation::inverse_transform()" );
  return nullptr;
}

