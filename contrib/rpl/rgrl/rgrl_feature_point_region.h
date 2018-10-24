#ifndef rgrl_feature_point_region_h_
#define rgrl_feature_point_region_h_
//:
// \file
// \author Gehua yang
// \date   Oct 2004
// \verbatim
// Modifications
//      Nov 2008 J Becker: Added a clone function.
// \endverbatim

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
  transform( rgrl_transformation const& xform ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_point_region, rgrl_feature_point );

  //: get pixels coordinates within the region
  void
  generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio ) override;

  //: make a clone copy
  rgrl_feature_sptr clone() const override;

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
