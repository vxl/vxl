// \file
// \author Gehua Yang
// \date   Oct 2004

#include "rgrl_feature_point_region.h"
#include <rgrl/rgrl_util.h>

rgrl_feature_point_region::
rgrl_feature_point_region( vnl_vector<double> const& loc, double radius )
  : rgrl_feature_point( loc ), 
    radius_(radius)
{     }

rgrl_feature_point_region::
rgrl_feature_point_region( unsigned dim )
  : rgrl_feature_point( dim ),
    radius_( 0 )
{
}

rgrl_feature_sptr
rgrl_feature_point_region::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_point_region* result = new rgrl_feature_point_region( this->location().size() );

  // capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = result;

  // Transform the location
  //
  xform.map_location( this->location_, result->location_ );

  //  Set the radius and length.  If these values truly must be
  //  transformed, then the function transform_region should used.
  result -> radius_ = this -> radius_;

  return result_sptr;
}

// Return region(neighboring) pixels in "pixel" coordinates.
void
rgrl_feature_point_region ::
generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio )
{
  //  Create the oriented rectangular solid.  Form the set of
  //  orthogonal directions and radii.  The directions are combined
  //  from the normal direction and the basis for the tangent
  //  subspace.  The first radius is half the fatness of the region.
  //  The others are all equal to the radius of the trace region.

  unsigned int dim = this -> location_ . size();
  vcl_vector< vnl_vector<double> > directions;
  directions.reserve( dim );
//    directions.push_back( this -> normal_ );

  // compute in the pixel coordinates
  // convert the location to the pixel coordinates
  vnl_vector< double > location_in_pixel( dim );
  vnl_vector< double > radii_in_pixel( dim, radius_ );
  vnl_vector< double > directions_in_pixel( dim );

  vnl_vector< double > direction_in_pixel( dim );
  for( unsigned int i = 0; i < dim; ++i )
  {
    location_in_pixel[ i ] = this->location_[ i ] / spacing_ratio[ i ];
    radii_in_pixel[i] = radii_in_pixel[i] / spacing_ratio[i];
  }

  // creat indentity matrix
  vnl_matrix<double> tangent( dim, dim, vnl_matrix_identity );

  for ( unsigned int i = 0; i < dim; ++i )
  {
    direction_in_pixel = tangent.get_column( i );
    for( unsigned j = 0; j < dim; ++j )
      direction_in_pixel[ j ] /= spacing_ratio[ j ];

    directions.push_back( direction_in_pixel );
  }

  //  Call the utility function to extract the pixel locations,
  //  record the caching and return the vector.
  
  rgrl_util_extract_region_locations( location_in_pixel, directions,
				      radii_in_pixel, pixel_coordinates_ );

  pixel_coordinates_cached_ = true;

}
