// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_feature_trace_region.h"
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_util.h>
#include <rgrl/rgrl_cast.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

rgrl_feature_trace_region::
rgrl_feature_trace_region( vnl_vector<double> const& loc,
                       vnl_vector<double> const& tangent )
  : rgrl_feature_trace_pt( loc, tangent ),
    region_length_( 0 ), region_radius_( 0 )
{
}

rgrl_feature_trace_region::
rgrl_feature_trace_region( vnl_vector<double> const& loc,
		       vnl_vector<double> const& tangent,
		       double                    region_length, 
		       double                    region_radius )
  : rgrl_feature_trace_pt( loc, tangent ),
    region_length_( region_length ), region_radius_( region_radius )
{
}


rgrl_feature_trace_region::
rgrl_feature_trace_region( )
  : rgrl_feature_trace_pt(),
    region_length_( 0 ), region_radius_( 0 )
{
}


unsigned int
rgrl_feature_trace_region::
num_constraints() const
{
  return location_.size() - 1;
}

rgrl_feature_sptr
rgrl_feature_trace_region::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_trace_region* result = new rgrl_feature_trace_region();

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

  result -> region_radius_ = this -> region_radius_;
  result -> region_length_ = this -> region_length_;

  return result_sptr;
}


rgrl_feature_sptr
rgrl_feature_trace_region::
transform_region( rgrl_transformation const& xform ) const
{
  //  Transform the location and direction
  rgrl_feature_sptr result_sptr = this -> transform( xform );

  //  Cast down the pointer so that we can set the specific variables.
  rgrl_feature_trace_region * trace_ptr 
    = rgrl_cast<rgrl_feature_trace_region *> ( result_sptr );

  //  Determine the length along the tangent direction.  Map a point
  //  that is half length units from the mapped center location along
  //  tangent direction and compute the double distance between this
  //  point and the mapped location.

  vnl_vector< double > end_point( this -> location_ . size() );
  xform . map_location( this -> location_ + this -> region_length_ / 2.0
			* this -> tangent_, end_point );
  trace_ptr -> region_length_ = ( end_point - trace_ptr -> location_ ) . magnitude() * 2.0;

  //  The radius is tougher.  First, find the basis of the tangent
  //  subspace from the null space of the single row matrix
  //  containing just the tangent direction.

  vnl_matrix<double> one_row( 1, this -> tangent_.size() );
  one_row.set_row( 0, this -> tangent_ );
  vnl_svd<double> tangent_svd( one_row );
  vnl_matrix<double> nullspace = tangent_svd.nullspace();
  assert( nullspace . columns() == this -> tangent_ . size() - 1 );

  //  Now, for each basis vector, map a point radius units away from
  //  the location along the vector.  Compute the distance of the
  //  resulting point from the mapped location.  Average these to
  //  come up with the radius.

  vnl_vector< double > point_along_dir( this -> location_ . size() );
  double sum_radii = 0;

  double this_region_radius = this->region_radius_; // Work-around for Borland C++ 5.
  for( unsigned int i=0; i < this -> location_ . size() - 1; ++i )
  {
    point_along_dir = this -> location();
    point_along_dir += this_region_radius * nullspace . get_column( i );
    xform . map_location( point_along_dir, end_point );
    sum_radii += ( end_point - trace_ptr -> location_ ) . magnitude();
  }

  trace_ptr -> region_radius_ = sum_radii / ( this -> location_ . size() - 1 );

  return result_sptr;
}
  
// Return region(neighboring) pixels in "pixel" coordinates.
void 
rgrl_feature_trace_region ::
generate_pixel_coordinates( vnl_vector< double > const&  spacing_ratio )
{
  //  Create the oriented rectangular solid.  Form the set of
  //  orthogonal directions and radii.  The directions are combined
  //  from the tangent direction and the basis for the normal
  //  subspace.  The first radius is half the length of the region.
  //  The others are all equal to the radius of the trace region.

  unsigned dim = this -> location_ . size();
  vnl_matrix< double > normals = this -> normal_subspace();
  vcl_vector< vnl_vector<double> > directions;
  directions . reserve( dim );

  vnl_vector< double > radii_in_pixel( dim );
  vnl_vector< double > location_in_pixel( dim );
  vnl_vector< double > direction_in_pixel( dim );

  // convert directions and location to the pixel coordinates
  for( unsigned i = 0; i < dim; ++i ) 
    {
      direction_in_pixel[ i ] = this->tangent_[ i ] / spacing_ratio[ i ];
      location_in_pixel[ i ] = this->location_[ i ] / spacing_ratio[ i ];
    }
  directions . push_back( direction_in_pixel );

  radii_in_pixel[ 0 ] = this -> region_length_ / spacing_ratio[ 0 ] / 2.0 ;    
  for ( unsigned i = 0; i < dim-1; ++i )
    {
      direction_in_pixel = normals.get_column( i );
      for( unsigned j = 0; j < dim; ++j )
        direction_in_pixel[ j ] /= spacing_ratio[ j ];

      directions.push_back( direction_in_pixel );
      radii_in_pixel[ i+1 ] = this -> region_radius_ / spacing_ratio[ i+1 ];
    }

  //  Call the utility function to extract the pixel locations,
  //  record the caching and return the vector.

  rgrl_util_extract_region_locations( location_in_pixel, directions,
				      radii_in_pixel, pixel_coordinates_ );
  pixel_coordinates_cached_ = true;
}
