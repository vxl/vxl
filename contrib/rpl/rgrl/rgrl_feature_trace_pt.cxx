#include "rgrl_feature_trace_pt.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_transformation.h"
#include "rgrl_util.h"
#include <vnl/algo/vnl_svd.h>

#include <vcl_cassert.h>

rgrl_feature_trace_pt ::
rgrl_feature_trace_pt()
  : scale_(0), 
    subspace_cached_( false ),
    length_( 0 ), radius_( 0 )
{
  
}

rgrl_feature_trace_pt::
rgrl_feature_trace_pt( vnl_vector<double> const& loc,
                       vnl_vector<double> const& tangent )
  : location_( loc ),
    tangent_( tangent ),
    error_proj_( loc.size(), loc.size(), vnl_matrix_identity ),
    scale_( 0 ),
    subspace_cached_(false),
    length_( 0 ), radius_( 0 )
{
  tangent_.normalize();
  error_proj_ -= outer_product( tangent_, tangent_ );
}

rgrl_feature_trace_pt::
rgrl_feature_trace_pt( vnl_vector<double> const& loc,
                       vnl_vector<double> const& tangent,
                       double                    length,
                       double                    radius )
  : location_( loc ),
    tangent_( tangent ),
    error_proj_( loc.size(), loc.size(), vnl_matrix_identity ),
    scale_( 0 ),
    subspace_cached_(false),
    length_( length ), radius_( radius )
{
  tangent_.normalize();
  error_proj_ -= outer_product( tangent_, tangent_ );
}



unsigned int
rgrl_feature_trace_pt::
num_constraints() const
{
  return location_.size()-1;
}

vnl_vector<double> const&
rgrl_feature_trace_pt::
location() const
{
  return location_;
}


vnl_vector<double> const&
rgrl_feature_trace_pt::
tangent() const
{
  return tangent_;
}


vnl_matrix<double> const&
rgrl_feature_trace_pt::
error_projector() const
{
  return error_proj_;
}


rgrl_feature_sptr
rgrl_feature_trace_pt::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_trace_pt* result = new rgrl_feature_trace_pt( );

  // capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = result;

  // Transform the location and tangent
  //
  xform.map_location( this->location_, result->location_ );
  xform.map_tangent( this->location_, this->tangent_, result->tangent_ );

  // The constructor above created an identity projection matrix
  //
  result->error_proj_.set_size( this->location_.size(), this->location_.size() );
  result->error_proj_.set_identity();
  result->error_proj_ -= outer_product( result->tangent_, result->tangent_ );

  //  Set the radius and length.  If these values truly must be
  //  transformed, then the function transform_region should used.

  result -> radius_ = this -> radius_;
  result -> length_ = this -> length_;

  return result_sptr;
}

vnl_matrix<double> const&
rgrl_feature_trace_pt ::
normal_subspace()
{
  if ( subspace_cached_ )
    return normal_subspace_;

  //  Find the basis of the normal subspace from the null space of the
  //  single row matrix containing just the tangent direction.

  vnl_matrix<double> one_row( 1, this -> tangent_.size() );
  one_row.set_row( 0, this -> tangent_ );
  vnl_svd<double> tangent_svd( one_row );
  normal_subspace_ = tangent_svd.nullspace();
  assert( normal_subspace_ . columns() == this -> tangent_ . size() - 1 );
  subspace_cached_ = true;
  return normal_subspace_;
}

rgrl_feature_trace_pt::feature_vector
rgrl_feature_trace_pt::
boundary_points(vnl_vector<double> const& in_direction) const
{
  //1. Compute the vector normal to the tangent lying in the same plane as the tangent
  //   and in_direction.
  //
  vnl_vector<double> normal(location_.size());
  if (location_.size() == 2) { //for 2D, just rotate the tangent_ by pi/2
    normal[0] = -tangent_[1];
    normal[1] =  tangent_[0];
  }
  else { // Gram-Schmidt Orthogonalization
    normal = in_direction -
      dot_product(tangent_, in_direction)*tangent_;
    normal.normalize();
  }

  //2. find the 2 boundary points in the direction of the normal,
  //   and create 2 rgrl_feature_trace_pt, with the centers shifted to the boundaries,
  //   tangent_ the same and no radius and length.
  //
  feature_vector bdy_feature_points;
  rgrl_feature_sptr bd_pt = new rgrl_feature_trace_pt( location_+(normal*radius_), tangent_ );
  bdy_feature_points.push_back(bd_pt);
  bd_pt = new rgrl_feature_trace_pt( location_-(normal*radius_), tangent_ );
  bdy_feature_points.push_back(bd_pt);

  return bdy_feature_points;
}
//: write out feature
void
rgrl_feature_trace_pt::
write( vcl_ostream& os ) const
{
  // tag
  os << "TRACE" << vcl_endl;
  
  // dim
  os << location_.size() << vcl_endl;
  
  // atributes
  os << location_ << '    ' << scale_ << "\n"
     << tangent_ << "\n" 
     << error_proj_ << vcl_endl;
}

//: read in feature
bool 
rgrl_feature_trace_pt::
read( vcl_istream& is, bool skip_tag )
{
  if( !skip_tag ) {

    // skip empty lines
    rgrl_util_skip_empty_lines( is );
    
    vcl_string str;
    vcl_getline( is, str );
    
    // The token should appear at the beginning of line
    if ( str.find( "TRACE" ) != 0 ) {
      WarningMacro( "The tag is not TRACE. reading is aborted.\n" );
      return false;
    }
  }

  // get dim
  int dim=-1;
  is >> dim;
  
  if( !is || dim<=0 ) 
    return false;    // cannot get dimension
    
  // get location
  location_.set_size( dim );
  is >> location_;
  if( !is )
    return false;   // cannot read location
    
  // get scale
  is >> scale_; 
  if( !is )
    return false;   // cannot read scale

  // get tangent
  tangent_.set_size( dim );
  is >> tangent_;
  if( !is ) 
    return false; 
    
  // get error projector
  error_proj_.set_size( dim, dim );
  is >> error_proj_;
  if( !is ) 
    return false; 
  
  //reset flag
  subspace_cached_ = false;
  
  return true;
}
