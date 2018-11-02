#ifndef msm_cubic_bezier_h_
#define msm_cubic_bezier_h_
//:
// \file
// \brief Basic implementation of a cubic poly-bezier
// \author Tim Cootes

#include <iostream>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vgl/vgl_point_2d.h>

//: Store start point and two control points for Bezier poly-curve
//  End point is assumed to be the start point of the next node.
struct msm_cubic_bezier_node
{
  // Start point
  vgl_point_2d<double> p;

  // Control points
  vgl_point_2d<double> c1,c2;

  //: Return position at t (in [0,1]) given end point q
  vgl_point_2d<double> point(double t, const vgl_point_2d<double>& q) const;

  //: Return tangent to curve at t (in [0,1]) given end point q
  vgl_vector_2d<double> tangent(double t, const vgl_point_2d<double>& q) const;

  //: Estimate approximate length of curve by piece-wise linear curve with k extra points
  double approx_length(const vgl_point_2d<double>& q, unsigned k=3) const;

  //: True if sufficiently straight, assuming q is end point.
  bool is_straight(const vgl_point_2d<double>& q, double thresh=0.01) const;

  //: Set to a straight line from p to q
  void set_to_line(const vgl_point_2d<double>& q);

};

//: Basic implementation of a cubic poly-bezier
//  Allows generation of a smooth curve through a set of points
//  Implemented as a set of bezier nodes.
//  Can either be open (so has n points, and n-1 bezier segments),
//  or closed (n points, n segments, with point n-1 linked to point 0).
//
class msm_cubic_bezier
{
 private:
  //: List of nodes making up the curve
  std::vector<msm_cubic_bezier_node> bnode_;

  //: Compute control points so as to generate a smooth open curve
  void smooth_open();

  //: Compute control points so as to generate a smooth closed curve
  void smooth_closed();

  //: True for closed curves
  bool closed_;
 public:
  // Default Constructor
  msm_cubic_bezier();

  //: Construct from set of points. Curve will pass through these.
  msm_cubic_bezier(const std::vector<vgl_point_2d<double> >&pts, bool closed=false);

  //: Construct from set of points. Curve will pass through these.
  void set_points(const std::vector<vgl_point_2d<double> >&pts, bool closed=false);

  // Destructor
  ~msm_cubic_bezier() = default;

  //: Number of points defining the curve
  std::size_t size() const { return bnode_.size(); }

    //: True for closed curves
  bool is_closed() const { return closed_; }


  //: Return point for node i
  const vgl_point_2d<double>& point(unsigned i) const
  { return bnode_[i].p; }

  //: Return i-th curve segment description
  const msm_cubic_bezier_node& segment(unsigned i) const { return bnode_[i]; }

  //: Return position at t (in [0,1]) in segment i of curve
  vgl_point_2d<double> point(unsigned i, double t) const;

  //: Return tangent to curve at t (in [0,1]) in segment i of curve
  vgl_vector_2d<double> tangent(unsigned i, double t) const;

  //: Return normal to curve at t (in [0,1]) in segment i of curve
  //  tangent(i,t) rotated by 90 degrees, using (-t.y(),t.x());
  vgl_vector_2d<double> normal(unsigned i, double t) const
  {
    vgl_vector_2d<double> v=tangent(i,t);
    return {-v.y(),v.x()};
  }

  //: Create n_pts points equally spaced between start and end nodes (inclusive)
  //  new_pts[0]=point(start), new_pts[n_pts-1]=point(end)
  //  For closed curves, use wrap-around (so if end<=start, assume it wraps round)
  //  To do the integration, each curve approximated by pieces of length no more than ~min_len
  void equal_space(unsigned start, unsigned end, unsigned n_pts, double min_len,
                   std::vector<vgl_point_2d<double> >& new_pts) const;

  //: Generate set of points along the curve, retaining control points.
  //  Creates sufficient intermediate points so that their spacing is approx_sep.
  // \param new_normals[i] the normal to the curve at new_pts[i]
  // \param control_pt_index[i] gives element of new_pts for control point i
  void get_extra_points(double approx_sep,
                        std::vector<vgl_point_2d<double> >& new_pts,
                        std::vector<vgl_vector_2d<double> >& new_normals,
                        std::vector<unsigned>& control_pt_index) const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_cubic_bezier& c);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_cubic_bezier& c);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_cubic_bezier& c);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_cubic_bezier& c);


#endif // msm_cubic_bezier_h_
