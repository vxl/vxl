// This is contrib/brl/bseg/boxm2/vecf/boxm2_vecf_orbicularis_oris.h
#ifndef boxm2_vecf_orbicularis_oris_h_
#define boxm2_vecf_orbicularis_oris_h_
//:
// \file
// \brief  The orbicularis oris represented as a generalized cylinder.
//
// \author Yi Dong
// \date January, 2016
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <bvgl/bvgl_cross_section.h>
#include <bvgl/bvgl_gen_cylinder.h>
#include "boxm2_vecf_orbicularis_oris_params.h"
#include "boxm2_vecf_geometry_base.h"

class boxm2_vecf_orbicularis_oris : public bvgl_gen_cylinder, public boxm2_vecf_geometry_base
{
 public:
  boxm2_vecf_orbicularis_oris()
  {
    fill_boundary_map();
  }
  //: construct orbicularis oris from a geometry file that stores all point cloud datasets
  boxm2_vecf_orbicularis_oris(std::string const& geometry_file, double const& cross_section_interval=0.5);
  //: construct from given central axis, cross sections
  boxm2_vecf_orbicularis_oris(vgl_cubic_spline_3d<double> const& axis, std::vector<bvgl_cross_section> const& cross_sects, double const& cross_section_interval=0.5):
    bvgl_gen_cylinder(axis, cross_sects, cross_section_interval) {}

  ~boxm2_vecf_orbicularis_oris() override = default;

  //: set deformation parameters
  void set_params(boxm2_vecf_orbicularis_oris_params const& params) { params_ = params; }

  //: tilt the lower part of the muscle to mimic mouth open
  //  The open angle is defined in params -- tilt_angle_in_deg (0.0 by default)
  boxm2_vecf_orbicularis_oris tilt() const;

  //: adjust the size of the entire muscle
  //  The scaling size is controlled by the scale_factor_ in params (1.0 by default, i.e., no size changes)
  boxm2_vecf_orbicularis_oris scale() const;

  //: returns a deformed orbicularis oris (controlled by eigenvector and eigenvalue defined in parameters)
  boxm2_vecf_orbicularis_oris deform() const;

  //: returns a deformed orbicularis oris (try to mimic pucker deformation)
  // an ankle point is defined on center line with certain distance to orbicularis oris.  The the entire muscle is contracted
  // toward this ankle point.  Note that the volume is not conserved in this case...
  boxm2_vecf_orbicularis_oris circular_deform() const;

  //: the functor operator for surface distance. dist_thresh is the distance a closest point on the normal plane
  // can be away from the closest point in the cross-section pointset.
  double operator() (vgl_point_3d<double> const& p) const override
  { return bvgl_gen_cylinder::distance(p, params_.planar_surface_dist_thresh_); }

  //: compute the inverse vector field for a target point
  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;

  //: display -- for debug purpose
  void display_axis_spline_ascii(std::ofstream& ostr, unsigned const& r = 0, unsigned const& g = 255, unsigned const& b = 0) const;
  void display_cross_section_pointsets_ascii(std::ofstream& ostr, unsigned const& r = 0, unsigned const& g = 255, unsigned const& b = 0) const;

 private:
  //: orbicularis oris parameter
  boxm2_vecf_orbicularis_oris_params params_;
  //: knots that defines the upper part and lower part of the orbicularis oris
  std::map<std::string, double> boundary_knots_;
  // function hard-coded to define the boundary_knots_ based on input point cloud structure
  void fill_boundary_map();
  // function to construct a rotation from three principal axis
  void rotation_matrix(vgl_vector_3d<double> const& l1, vgl_vector_3d<double> const& l2, vgl_vector_3d<double> const& l3,
                       vnl_matrix_fixed<double, 3, 3>& rot_matrix) const;
};

#endif // boxm2_vecf_orbicularis_oris_h_
