#ifndef rgrl_invariant_single_landmark_h_
#define rgrl_invariant_single_landmark_h_
//:
// \file
// \brief  Represent a 2D bifurcation landmark associated with a set of invariant properties.
// \author Charlene Tsai
// \date   March 2004

#include <vcl_vector.h>
#include <vnl/vnl_double_2.h>

#include "rgrl_transformation_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_invariant_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_invariant.h"

//: Represent a 2D bifurcation landmark associated with a set of invariant properties.
//
//  This is the simplied implementation of the single-landmark feature
//  with invariant properties in "The Dual-Bootstrap Iterative
//  Closest Point Algorithm with Application to Retinal Image
//  Registration" by Stewart et al., TMI 2003, vol 22, no 11.
//
//  This invariant class is to serve as an example of feature with
//  invariants for initialization using invariant indexing. The
//  properties of a landmark includes the center location, 3 direction
//  vectors of the vessels that come to meet, 3 associated widths. The
//  radius of the circular region that the landmark occupies is taken
//  as the maximum of the 3 widths.
//
//  The transformation model is similarity
//  (translation+scaling+rotaion), estimated using the landmark center
//  location and 6 boundary points.
//
class rgrl_invariant_single_landmark
  : public rgrl_invariant
{
 public:
  //: Constructor, both angular_variance and width_ratio_variance are in radius
  rgrl_invariant_single_landmark(vnl_vector<double> location,
                                 vnl_vector<double> vessel_dir1,
                                 vnl_vector<double> vessel_dir2,
                                 vnl_vector<double> vessel_dir3,
                                 double width1, double width2, double width3,
                                 double angular_std = 1,
                                 double width_ratio_std = 1);

  //: Construct the constellation from another by shifting indices counterclockwise
  //
  //  \note Used if there is ambiguity in the ordering of the vessel
  //  directions. Both angular_variance and width_ratio_variance are
  //  in radius
  rgrl_invariant_single_landmark(const rgrl_invariant_single_landmark& copy,
                                 double angular_std = 1,
                                 double width_ratio_std = 1);

  //: Return the location
  const vnl_double_2& location() const;

  //: Returns the i-th boundary point location
  const vnl_double_2& boundary_point_location(int i) const;

  //: Returns the i-th boundary point normal direction
  const vnl_double_2& boundary_point_normal(int i) const;

  //: Estimate the xform mapping \a from to the current feature
  bool estimate(rgrl_invariant_sptr         from,
                rgrl_transformation_sptr&   xform,
                rgrl_scale_sptr&            scale );

  //: Returns the vector of invariants normalized by scale
  const vnl_vector<double>& cartesian_invariants() const;

  //: Returns the vector of invariants normalized by scale
  const vnl_vector<double>& angular_invariants() const;

  //: Computes the valid match region for the constellation
  rgrl_mask_box region() const;

  //: Return true if the feature has an initial ROI
  bool has_region() const {return true;}

  //: Returns the center of the points in the computation of the parameters
  //
  // The center is the average location of all landmarks and boundary
  // points. This is not necessarily the center of the bounding_box
  const vnl_double_2& center();

  //: Constellation ambiguity check
  //
  // \note constellations are ambiguous if one or more landmarks has a
  // signature angle near 0 degrees, since angles near 0 could
  // actually be near 360. The reverse is not flagged or corrected
  // since this would lead to redundancy.
  bool is_ambiguous() const {return is_ambiguous_;}

  // Defines type-related functions
  rgrl_type_macro( rgrl_invariant_single_landmark, rgrl_invariant );

 private:
  //: A helper function to compute the counterclockwise angles between two orientation vectors
  // \return angles in the range of [0,2*PI)
  double ccw_angle_between(vnl_double_2 from, vnl_double_2 to);

  //: A helper function to reorder selected indices of a landmark
  //
  //  \note The indices are passed in by reference.  The indices
  //  specified are reordered counterclockwise from the basis
  //  direction specified. The counterclockwise angles from the basis
  //  are also returned by referance.
  void reorder_vessel( vcl_vector<vnl_vector<double> >& directions,
                       vcl_vector<double>& local_widths,
                       vcl_vector<double>& angles);

 private:
  vnl_double_2 location_;
  vcl_vector<double> local_widths_;
  vcl_vector<vnl_double_2> boundary_points_;
  vcl_vector<vnl_double_2> trace_normals_;
  vnl_vector<double> cartesian_invariants_;
  vnl_vector<double> angular_invariants_;
  vnl_double_2 center_;

  double radius_;
  bool is_ambiguous_;
  bool center_set_;
  bool is_estimate_set_;
};

#endif // rgrl_invariant_single_landmark_h_
