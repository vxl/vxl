#ifndef rgrl_feature_trace_pt_h_
#define rgrl_feature_trace_pt_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_feature.h"
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

//: Represent a point along a trace (of a vessel, neuron, etc.)
//
// A trace point is characterized by a location and a tangent along
// the trace.
//
class rgrl_feature_trace_pt
  : public rgrl_feature
{
 public:
  typedef vcl_vector<rgrl_feature_sptr >  feature_vector;

 public:
  //: Constructor
  //  should not be used by anything other than the reader.
  //  use the other constructors insead.
  // rgrl_feature_trace_pt();

  //:  Constructor to initialize feature_trace_pt location.
  rgrl_feature_trace_pt( vnl_vector<double> const& loc,
                         vnl_vector<double> const& tangent );

  //:  Constructor to initialize feature_trace_pt location that has a length along the tangent and an normal.
  rgrl_feature_trace_pt( vnl_vector<double> const& loc,
                         vnl_vector<double> const& tangent,
                         double                    length,
                         double                    radius );

  //: read in feature
  virtual
  bool read( vcl_istream& is, bool skip_tag=false );

  //: write out feature
  virtual
  void write( vcl_ostream& os ) const;

  virtual vnl_vector<double> const&
  location() const;

  //: Provide the scale level at which this feature is detected
  virtual
  double scale() const { return scale_; }

  //: Set the scale level at which this feature is detected
  virtual
  void set_scale( double scale ) { scale_ = scale; }

  virtual vnl_vector<double> const&
  tangent() const;

  virtual vnl_matrix<double> const&
  error_projector() const;

  //: The result is a rgrl_feature_trace_pt, without transforming the radius/length parameters
  virtual rgrl_feature_sptr
  transform( rgrl_transformation const& xform ) const;

  //:
  // The result is a vector of boundary locations in the direction of the normal
  // in the plane defined by the tangent and in_direction.
  //
  //  CAVEAT: This design is not good enough for 3D trace points, since it only
  //          produces 2 boundary constraints. This function should be revised
  //          later for 3D.
  //
  //  Chuck's comment:  I'm not sure this should be here.  It can
  //  easily be extracted in an arbitrary set of dimensions from a
  //  normal subspace and the radius.
  virtual feature_vector
  boundary_points(vnl_vector<double> const& in_direction) const;

  virtual unsigned int
  num_constraints() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_trace_pt, rgrl_feature );

  //: Return a matrix whose columns form the subspace normal to the trace tangent.
  virtual vnl_matrix<double> const&
  normal_subspace();

  double length() const { return length_; }
  double radius() const { return radius_; }

  //  Chuck's note:  I am beginning to wonder if we are trying to do
  //  too much here.  Perhaps we should be make a subclass for the
  //  region-based estimator.

 protected:
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature. The error projection matrix is initialized
  // to the identity.
  //
  rgrl_feature_trace_pt();

  // to be able to use the protected constructor
  friend rgrl_feature_sptr
         rgrl_feature_reader( vcl_istream& is );

  vnl_vector<double> location_;
  vnl_vector<double> tangent_;
  vnl_matrix<double> error_proj_;
  // scale level at which this feture is detected
  double scale_;
 private:

  //: The basis for the subspace of vectors normal to the tangent direction.
  //  This is normal subspace.  It is computed once, when first needed, and cached.
  //  This is because the feature location and normal are fixed.
  bool subspace_cached_;
  vnl_matrix< double > normal_subspace_;

  //  Chuck's note:  We'll have to be careful with the meaning of
  //  these.  For example, in aligning extracted vessel boundaries,
  //  the radius_ might mean the half-width of the vessel, whereas in
  //  the pseudo-feature-based registration application, you might
  //  want the radius to be slightly larger...

  double length_;
  double radius_;
};

#endif
