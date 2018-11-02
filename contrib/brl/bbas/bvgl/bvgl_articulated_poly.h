#ifndef bvgl_articulated_poly_h_
#define bvgl_articulated_poly_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A polyline with articulated joints
//
// \author
//  J.L. Mundy - January 13, 2007
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include "bvgl_articulated_poly_sptr.h"

//-----------------------------------------------------------------------------
class bvgl_articulated_poly : public vsol_polyline_2d
{
 public:
  bvgl_articulated_poly(const unsigned n_joints);
  bvgl_articulated_poly(const unsigned n_joints, std::vector<double> const& link_lengths);
  bvgl_articulated_poly(const bvgl_articulated_poly& poly);
  ~bvgl_articulated_poly() override = default;

  vgl_h_matrix_2d<double> joint_transform(unsigned joint) const
    {return joint_transforms_[joint];}

  //: angle between joint and joint+1
  double joint_angle(unsigned joint) const;

  //: link from joint to joint+1
  double link_length(unsigned joint) const;

  vsol_point_2d_sptr joint_position(const unsigned joint) const;

  //:Transform the articulation
  void transform(std::vector<double > const& delta_joint_angle);

  void sub_manifold_transform(const double t,
                              std::vector<double > const& basis_angles);


  //:Compute the Lie distance between two articulations
  static double lie_distance(bvgl_articulated_poly const& ap1,
                             bvgl_articulated_poly const& ap2) ;

  //: Projection onto the sub manifold
  static bvgl_articulated_poly_sptr
    projection(bvgl_articulated_poly_sptr const& target,
               std::vector<double > const& manifold_basis);

  //:debug support
  void print();
  void print_xforms();

 private:
  //: called when the state of the polygon changes
  void update();
  bvgl_articulated_poly() = default; //not available
  //:the inverse joint transforms for the current state of the articulation
  std::vector<vgl_h_matrix_2d<double> > joint_transforms_;
};

#endif // bvgl_articulated_poly_h_
