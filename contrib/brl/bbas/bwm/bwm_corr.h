#ifndef bwm_corr_h_
#define bwm_corr_h_
//:
// \file
// \brief A class to store image correspondences
// \author J.L. Mundy
// \date Initial version Feb. 2004
//
// \verbatim
// Initial version Feb. 2004
// May 1, 2004
// modified to make more generic using a correspondence base class - JLM
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
//   22-may-2011 Peter Vanroose moved #include bwm_observer_cam.h to .cxx
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <iosfwd>
#include <vbl/vbl_ref_count.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

// forward declarations:
class bwm_observer_cam;
class vsol_point_2d;

class bwm_corr : public vbl_ref_count
{
 public:
   bwm_corr() { mode_= true; }

   bwm_corr(bwm_corr const& x) : vbl_ref_count(), mode_(x.mode()), world_pt_(x.world_pt()) {}

  ~bwm_corr() {}

  bool match(bwm_observer_cam* obs, vgl_point_2d<double> &pt);
  bool mode() const { return mode_; }
  int num_matches() const { return matches_.size(); }
  vgl_point_3d<double> world_pt() const { return world_pt_; }
  void erase(bwm_observer_cam* obs);
  std::vector<bwm_observer_cam*> observers();

  //: returns true, if the observer contributes to the correspondence
  bool obs_in(bwm_observer_cam* obs, vgl_point_2d<double> &corr);

  //: returns the match list as tableau name mapped to the 2D points
  std::vector<std::pair<std::string, vsol_point_2d> > match_list();

  //: Mutators
  bool update_match(bwm_observer_cam* obs, vgl_point_2d<double> old_pt, vgl_point_2d<double> new_pt);
  void set_match(bwm_observer_cam* obs, const double x, const double y);
  void set_mode(const bool mode) { mode_ = mode; }
  void set_world_pt(vgl_point_3d<double> const& wp) { world_pt_ = wp; }

  //: Utility functions
  friend std::ostream&  operator<<(std::ostream& s, bwm_corr const& c);

  //: XML write
  void x_write(std::ostream &os);

 protected:

  //members
  bool mode_; //!< true if image_to_image
  std::map<bwm_observer_cam*, vgl_point_2d<double> > matches_;//!< match in each camera
  vgl_point_3d<double> world_pt_;
};

#endif // bwm_corr_h_
