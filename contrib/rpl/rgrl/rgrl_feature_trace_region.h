#ifndef rgrl_feature_trace_region_h_
#define rgrl_feature_trace_region_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_region.h>
#include <vcl_vector.h>

//: Represent a point along a trace (of a vessel, neuron, etc.)
//
// A trace point is characterized by a location and a tangent along
// the trace.
//
class rgrl_feature_trace_region
  : public rgrl_feature_trace_pt, public rgrl_feature_region
{
 public:
  typedef vcl_vector<rgrl_feature_sptr >  feature_vector;

 public:
  //:  Constructor to initialize feature_trace_region location.
  rgrl_feature_trace_region( vnl_vector<double> const& loc,
                         vnl_vector<double> const& tangent );

  //:  Constructor to initialize feature_trace_region location that has a length along the tangent and an normal.
  rgrl_feature_trace_region( vnl_vector<double> const& loc,
                         vnl_vector<double> const& tangent,
                         double                    region_length,
                         double                    region_radius );

  //: The result is a rgrl_feature_trace_region, without transforming the radius/length parameters
  rgrl_feature_sptr
  transform( rgrl_transformation const& xform ) const;

  //: The result is a rgrl_feature_trace_region, with a transformation of the radius/length parameters
  rgrl_feature_sptr
  transform_region( rgrl_transformation const& xform ) const;

  //: Result is a vector of boundary locations in the direction of the normal in the plane defined by the tangent and in_direction.
//    //  CAVEAT: This design is not good enough for 3D trace points, since it only
//    //          produces 2 boundary constraints. This function should be revised
//    //          later for 3D.
//    //
//    //  Chuck's comment:  I'm not sure this should be here.  It can
//    //  easily be extracted in an arbitrary set of dimensions from a
//    //  normal subspace and the radius.
//    feature_vector
//    boundary_points(vnl_vector<double> const& in_direction) const;

  unsigned int num_constraints() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_trace_region, rgrl_feature_trace_pt );

  double region_length() const { return region_length_; }
  double region_radius() const { return region_radius_; }

  //:  Extract the pixel coordinates within the oriented rectangular solid defined by the feature.
  virtual void generate_pixel_coordinates( vnl_vector< double > const& spacing_ratio );

  //  Chuck's note:  I am beginning to wonder if we are trying to do
  //  too much here.  Perhaps we should be make a subclass for the
  //  region-based estimator.

 private:
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature. The error projection matrix is initialized
  // to the identity.
  //
  rgrl_feature_trace_region();

  //  Chuck's note:  We'll have to be careful with the meaning of
  //  these.  For example, in aligning extracted vessel boundaries,
  //  the radius_ might mean the half-width of the vessel, whereas in
  //  the pseudo-feature-based registration application, you might
  //  want the radius to be slightly larger...

  //  For pseudo matcher method, length_ is the length along the
  //  tangent direction, centered at the feature point's location.
  double region_length_;
  //  For pseudo matcher method, radius_ is half of the length along
  //  the normal directions.
  double region_radius_;
};

#endif
