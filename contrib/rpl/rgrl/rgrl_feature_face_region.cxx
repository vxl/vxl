
#include "rgrl_feature_face_region.h"
#include <rgrl/rgrl_transformation.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <rgrl/rgrl_cast.h>
#include <rgrl/rgrl_util.h>

rgrl_feature_face_region ::
rgrl_feature_face_region( vnl_vector< double > const& location, 
                          vnl_vector< double > const& normal )
  : rgrl_feature_face_pt( location, normal ),
    thickness_( 0.0 ), radius_( 0.0 ) 
{
}


rgrl_feature_face_region :: 
rgrl_feature_face_region( vnl_vector< double > const& location,
                          vnl_vector< double > const& normal,
                          double thickness,
                          double radius )
  : rgrl_feature_face_pt( location, normal ), 
    thickness_( thickness ), radius_( radius )
{
}


//  private constructor for transformed face points
rgrl_feature_face_region ::
rgrl_feature_face_region()
  : rgrl_feature_face_pt(),
    thickness_( 0 ), radius_( 0 )
{
}

unsigned int
rgrl_feature_face_region::
num_constraints() const
{
  return 1;
}

rgrl_feature_sptr 
rgrl_feature_face_region:: 
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_face_region* face_ptr = new rgrl_feature_face_region();

  // Capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = face_ptr;

  xform.map_location( this->location_, face_ptr->location_ );
  xform.map_normal( this->location_, this->normal_, face_ptr->normal_ );

  face_ptr->error_proj_ = outer_product( face_ptr->normal_, face_ptr->normal_ );
  face_ptr->thickness_ = this->thickness_;
  face_ptr->radius_ = this->radius_;
  return result_sptr;
}


rgrl_feature_sptr 
rgrl_feature_face_region :: 
transform_region( rgrl_transformation const& xform ) const
{
  //  Transform the location and direction, and form the new error projector.
  rgrl_feature_sptr result_sptr = this -> transform( xform );

  //  Cast down the pointer so that we can get / set the specific variables.
  rgrl_feature_face_region * face_ptr
    = rgrl_cast<rgrl_feature_face_region *> ( result_sptr );

  //  Determine the thickness along the normal direction.  Map a point
  //  that is half thickness units from the mapped center location
  //  along normal direction and compute the double distance between
  //  this point and the mapped location.

  vnl_vector< double > end_point( this -> location_ . size() );
  xform . map_location( this -> location_ + this->thickness_ / 2.0
			* this -> normal_, end_point );
  face_ptr -> thickness_ = ( end_point - face_ptr -> location_) . magnitude() * 2.0;

  //  The radius is tougher.  First, find the basis of the subspace of
  //  tangent vectors from the null space of the single row matrix
  //  containing just the normal direction.

  vnl_matrix<double> one_row( 1, this -> normal_.size() );
  one_row.set_row( 0, this -> normal_ );
  vnl_svd<double> normal_svd( one_row );
  vnl_matrix<double> nullspace = normal_svd.nullspace();
  assert( nullspace . columns() == this -> normal_ . size() - 1 );

  //  Now, for each basis vector, map a point radius units away from
  //  the location along the vector.  Compute the distance of the
  //  resulting point from the mapped location.  Average these to
  //  come up with the radius.

  vnl_vector< double > point_along_dir( this->location_.size() );
  double sum_radii = 0;

  for( unsigned int i=0; i< this -> location_ . size() - 1; ++i )
  {
    point_along_dir = this -> location_;
    point_along_dir += this->radius_ * nullspace . get_column( i );
    xform . map_location( point_along_dir, end_point );
    sum_radii += ( end_point - face_ptr -> location_ ) . magnitude();
  }

  face_ptr -> radius_ = sum_radii / ( this -> location_ . size() - 1 );

  return result_sptr;
}


// Return region(neighboring) pixels in "pixel" coordinates.
void 
rgrl_feature_face_region ::
generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio )
{
  //  Create the oriented rectangular solid.  Form the set of
  //  orthogonal directions and radii.  The directions are combined
  //  from the normal direction and the basis for the tangent
  //  subspace.  The first radius is half the fatness of the region.
  //  The others are all equal to the radius of the trace region.

  unsigned int dim = this -> location_ . size();
  vnl_matrix< double > tangents = this -> tangent_subspace();
  vcl_vector< vnl_vector<double> > directions;
  directions.reserve( dim );
//    directions.push_back( this -> normal_ );

  // compute in the pixel coordinates
  // convert the location to the pixel coordinates
  vnl_vector< double > location_in_pixel( dim );
  vnl_vector< double > radii_in_pixel( dim );
  vnl_vector< double > directions_in_pixel( dim );

  vnl_vector< double > direction_in_pixel( dim );
  for( unsigned int i = 0; i < dim; ++i )
    {
      direction_in_pixel[ i ] = this->normal_[ i ] / spacing_ratio[ i ];
      location_in_pixel[ i ] = this->location_[ i ] / spacing_ratio[ i ];
    }
  directions.push_back( direction_in_pixel );

  radii_in_pixel[ 0 ] = this -> thickness_ / spacing_ratio[ 0 ] / 2.0;
  for ( unsigned int i = 0; i < dim-1; ++i )
    {
      direction_in_pixel = tangents.get_column( i );
      for( unsigned j = 0; j < dim; ++j )
        {
          direction_in_pixel[ j ] /= spacing_ratio[ j ];
        }
      directions.push_back( direction_in_pixel );
      radii_in_pixel[ i+1 ] = this -> radius_ / spacing_ratio[ i+1 ];
    }

  //  Call the utility function to extract the pixel locations,
  //  record the caching and return the vector.
  
  rgrl_util_extract_region_locations( location_in_pixel, directions,
				      radii_in_pixel, pixel_coordinates_ );

  pixel_coordinates_cached_ = true;

}

/*
  Keep this for mapping --- move to different code
  
void
rgrl_feature_face_region :: set_tangents()
{
  vnl_matrix<double> one_row( 1, normal_.size() );
  one_row.set_row( 0, normal_ );
  vnl_svd<double> normal_svd( one_row );

  tangent_directions_.clear();
  for( unsigned int i=1; i<normal_.size(); ++i )
    {
      tangent_directions_.push_back( normal_svd.V().get_column( i ) );
    }
}

*/
