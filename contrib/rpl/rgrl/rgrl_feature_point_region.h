#ifndef rgrl_feature_point_region_h_
#define rgrl_feature_point_region_h_
//:
// \file
// \author Gehua yang
// \date   Oct 2004

#include <rgrl/rgrl_feature_region.h>
#include <rgrl/rgrl_feature_point.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class rgrl_feature_point_region
  : public rgrl_feature_region, public rgrl_feature_point
{
 public:
  //:  Constructor to initialize feature_point_region location that has an associated radius.
  rgrl_feature_point_region( vnl_vector<double> const& loc, double radius = 0);

  //: The result is a rgrl_feature_point_region, without transforming the radius
  rgrl_feature_sptr
  transform( rgrl_transformation const& xform ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_point_region, rgrl_feature_point );

  //: get pixels coordinates within the region
  virtual void 
  generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio );

private:
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature. The error projection matrix is initialized
  // to the identity.
  //
  rgrl_feature_point_region( unsigned dim );

 protected:
  double radius_;
};

#endif // rgrl_feature_point_region_h_
