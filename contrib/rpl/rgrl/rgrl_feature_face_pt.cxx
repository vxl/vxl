
#include "rgrl_feature_face_pt.h"
#include "rgrl_transformation.h"
#include <vnl/algo/vnl_svd.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include "rgrl_util.h"

rgrl_feature_face_pt ::
rgrl_feature_face_pt( vnl_vector< double > const& location, 
		      vnl_vector< double > const& normal )
  : location_( location ), normal_( normal ), 
    error_proj_( normal.size(), normal.size() ),
    subspace_cached_( false )
{
  normal_.normalize();
  error_proj_ = outer_product( normal, normal ) ;
}

//  private constructor for transformed face points
rgrl_feature_face_pt ::
rgrl_feature_face_pt()
  : subspace_cached_( false )
{
}


vnl_vector<double> const& 
rgrl_feature_face_pt :: 
location() const
{
  return location_;
}

vnl_matrix<double> const& 
rgrl_feature_face_pt :: 
error_projector() const
{
  return error_proj_;
}

unsigned int
rgrl_feature_face_pt::
num_constraints() const
{
  return 1;
}

vnl_vector<double> const& 
rgrl_feature_face_pt :: normal() const
{
  return normal_;
}

vnl_matrix<double> const&
rgrl_feature_face_pt ::
tangent_subspace()
{
  if ( subspace_cached_ )
    return tangent_subspace_;

  vnl_matrix<double> one_row( 1, this -> normal_.size() );
  one_row.set_row( 0, this -> normal_ );
  vnl_svd<double> normal_svd( one_row );
  tangent_subspace_ = normal_svd.nullspace();
  assert( tangent_subspace_ . columns() == this -> normal_ . size() - 1 );
  subspace_cached_ = true;
  return tangent_subspace_;
}

rgrl_feature_sptr 
rgrl_feature_face_pt :: 
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_face_pt* face_ptr = new rgrl_feature_face_pt();

  // Capture the allocation into a smart pointer for exception safety.
  rgrl_feature_sptr result_sptr = face_ptr;

  xform.map_location( this->location_, face_ptr->location_ );
  xform.map_normal( this->location_, this->normal_, face_ptr->normal_ );

  face_ptr->error_proj_ = outer_product( face_ptr->normal_, face_ptr->normal_ );

  return result_sptr;
}
