#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_util.h>
#include <vnl/vnl_math.h>
//:
// \file
// \brief Base class for transformation representation, estimations and application in generalized registration library
// \author Chuck Stewart
// \date 15 Nov 2002

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>

rgrl_transformation::
rgrl_transformation( const vnl_matrix<double>& cov )
  : is_covar_set_(false)
{
  set_covar( cov );
}

rgrl_transformation::
~rgrl_transformation()
{
}

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
  vnl_matrix<double> J = jacobian( from_loc );
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
    double min = vcl_abs(from_dir[0]);
    for ( unsigned int i=1; i<3; i++)
      if ( vcl_abs(from_dir[i]) < min ) {
        min = vcl_abs( from_dir[i] );
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

//:  Parameter covariance matrix
vnl_matrix<double>
rgrl_transformation::
covar() const
{
  assert( is_covar_set_ );
  return covar_;
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
  // set it to epsison if scaling is in fact zero
  for ( unsigned int i=0; i<scaling.size(); ++i )
    assert( vnl_math_isfinite( scaling[i] ) );

  scaling_factors_ = scaling;
}


vcl_ostream&
operator<< (vcl_ostream& os, rgrl_transformation const& xform )
{
  xform.write( os );
  return os;
}

//: output transformation
void 
rgrl_transformation::
write( vcl_ostream& os ) const
{
  if( is_covar_set() ) {
    
    // write covariance
    os << "COVARIANCE" << vcl_endl;
    os << covar_.rows() << ' ' << covar_.cols() << vcl_endl;
    os << covar_ << vcl_endl;
  }
  
  if( scaling_factors_.size() ) {
    
    // write scaling factors
    os << "SCALING_FACTORS" << vcl_endl;
    os << scaling_factors_.size() << vcl_endl;
    os << scaling_factors_ << vcl_endl;
  }
    
}

//: input transformation
void 
rgrl_transformation::
read( vcl_istream& is )
{
  vcl_string tag_str;
  vcl_streampos pos;

  // skip any empty lines
  rgrl_util_skip_empty_lines( is );
  if( is.eof() ) 
    return;   // reach the end of stream
  
  tag_str="";
  vcl_getline( is, tag_str );

  if( tag_str.find("COVARIANCE") == 0 ) {
  
    // read in covariance matrix
    int m=-1, n=-1;
    vnl_matrix<double> cov;
    
    // get dimension
    is >> m >> n;
    if( !is || m<=0 || n<=0 )
      return;   // cannot get the dimension
    
    cov.set_size(m, n);
    cov.fill(0.0);
    is >> cov;
    
    if( !is )
      return;  // cannot read the covariance matrix
    
    this->set_covar( cov );
    
    // read in the next tag
    // skip any empty lines
    rgrl_util_skip_empty_lines( is );
    if( is.eof() ) 
      return;   // reach the end of stream
    
    tag_str="";
    vcl_getline( is, tag_str );
  } 
  
  if( tag_str.find("SCALING_FACTORS") == 0 ) {
    
    // read in scaling factors
    int m=-1;
    
    // get dimension
    is >> m;
    
    if( !is || m<=0 )
      return;  // cannot get dimension
    
    scaling_factors_.set_size( m );
    is >> scaling_factors_;
  }
}
