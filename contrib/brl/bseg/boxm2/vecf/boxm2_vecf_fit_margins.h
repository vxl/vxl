// This is core/vgl/algo/boxm2_vecf_fit_margins.h
#ifndef boxm2_vecf_fit_margins_h_
#define boxm2_vecf_fit_margins_h_
//:
// \file
// \brief Fits the inferior and superior margin models in the X-Y plane
// \author Joseph L. Mundy
// \date June 22, 2015
//
#include <vector>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_orbit_params.h"

class boxm2_vecf_fit_margins
{
  // Data Members--------------------------------------------------------------
 protected:
  boxm2_vecf_orbit_params opr_;
  double dphi_rad_;
  std::vector<vgl_point_2d<double> > inferior_margin_pts_;
  std::vector<vgl_point_2d<double> > superior_margin_pts_;
  std::vector<vgl_point_2d<double> > superior_crease_pts_;
  vgl_point_2d<double> lateral_canthus_;
  vgl_point_2d<double> medial_canthus_;
  bool is_right_;
  bool estimate_t_;
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  boxm2_vecf_fit_margins():is_right_(false),dphi_rad_(0.0),estimate_t_(false) {}

 boxm2_vecf_fit_margins(std::vector<vgl_point_3d<double> >const& inferior_margin_pts,
                        std::vector<vgl_point_3d<double> >const& superior_margin_pts,
                        std::vector<vgl_point_3d<double> >const& superior_crease_pts,
                        vgl_point_3d<double> const& lateral_canthus,
                        vgl_point_3d<double> const& medial_canthus,
                        bool is_right=false,
                        bool estimate_t = false):
   is_right_(is_right),dphi_rad_(0.0),estimate_t_(estimate_t){
    for(const auto & inferior_margin_pt : inferior_margin_pts)
      inferior_margin_pts_.emplace_back(inferior_margin_pt.x(), inferior_margin_pt.y());
    for(const auto & superior_margin_pt : superior_margin_pts)
      superior_margin_pts_.emplace_back(superior_margin_pt.x(), superior_margin_pt.y());
    for(const auto & superior_crease_pt : superior_crease_pts)
      superior_crease_pts_.emplace_back(superior_crease_pt.x(), superior_crease_pt.y());
    lateral_canthus_ = vgl_point_2d<double>(lateral_canthus.x(),lateral_canthus.y());
    medial_canthus_ = vgl_point_2d<double>(medial_canthus.x(),medial_canthus.y());
  }
  ~boxm2_vecf_fit_margins() = default;

  // Operations---------------------------------------------------------------

  //: clear internal data
  void clear();


  void set_initial_guess(boxm2_vecf_orbit_params const& params){
    opr_ = params;}

  //: find the skew-free affine transform that minimizes the squared error with
  // respect to the margin points.
  // returns the average distance from the points to the margin curves
  // error conditions are reported on outstream
  double fit(std::ostream* outstream=nullptr, bool verbose=false);

// Data Access---------------------------------------------------------------

  std::vector<vgl_point_2d<double> > inferior_points() const {return inferior_margin_pts_;}
  std::vector<vgl_point_2d<double> > superior_points() const {return superior_margin_pts_;}
  std::vector<vgl_point_2d<double> > crease_points() const {return superior_crease_pts_;}
  vgl_point_2d<double> lateral_canthus()  {return lateral_canthus_;}
  vgl_point_2d<double> medial_canthus()  {return medial_canthus_;}
  boxm2_vecf_orbit_params orbit_parameters() const{return opr_;}

  // data ploting for debug purposes
  bool plot_orbit(std::ostream& ostr) const;
};
#endif // boxm2_vecf_fit_margins_h_
