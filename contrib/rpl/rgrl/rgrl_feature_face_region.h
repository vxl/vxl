
#ifndef rgrl_feature_face_region_h_
#define rgrl_feature_face_region_h_

//:
// \file
// \brief Class to represent a N-d face edge element.  It has 1 normal
//    direction and N-1 tangent directions.  
// \author Chuck Stewart
// \date 16 Sep 2003

#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_feature_region.h>
#include <vcl_vector.h>

class rgrl_feature_face_region : public rgrl_feature_face_pt, public rgrl_feature_region {
public:
  rgrl_feature_face_region( vnl_vector< double > const& location,
                            vnl_vector< double > const& normal );

  rgrl_feature_face_region( vnl_vector< double > const& location,
                            vnl_vector< double > const& normal,
                            double                      thickness,
                            double                      radius );

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_face_region, rgrl_feature_face_pt )

  //:  Return the thickness ("fatness") of the surface (or the transition
  //   region)  
  double thickness() const { return thickness_; }

  //:  Return the radius on the tangent surface
  double radius() const { return radius_; }

  //: Result is a rgrl_feature_face_region, but the region is not transformed.
  rgrl_feature_sptr transform( rgrl_transformation const& xform ) const;

  //: Result is a rgrl_feature_face_region with a transformed region as well.
  rgrl_feature_sptr transform_region( rgrl_transformation const& xform ) const;

  //:  Extract the pixel coordinates within the oriented rectangular
  //   solid defined by the feature.
  virtual void generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio );

  unsigned int num_constraints() const;
private:

  //:
  // Create an uninitialized face_region of dimension dim
  //
  rgrl_feature_face_region();

  //: The normal width, and the radius on the tangent surface in
  //physical coordinates.
  double thickness_;
  double radius_;

};


#endif
