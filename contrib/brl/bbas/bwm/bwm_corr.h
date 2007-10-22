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
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vbl/vbl_ref_count.h>

#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

#include "bwm_observer_cam.h"

class bwm_corr : public vbl_ref_count
{
 public:
   bwm_corr() { mode_= true; }
  //: image to image correspondence
  //bwm_corr(const int n_cams);
  //: image to world correspondence
  //bwm_corr(const int n_cams, vgl_point_3d<double> const& wp);
  
   bwm_corr(bwm_corr const& x) : vbl_ref_count() {}

  ~bwm_corr() {}

  bool match(bwm_observer_cam* obs, vgl_point_2d<double> &pt);
  bool mode() const { return mode_;}
  int num_matches() { return matches_.size(); }
  vgl_point_3d<double> world_pt() const { return world_pt_;}
  bool valid(bwm_observer_cam* obs, const double x, const double y);
  void erase(bwm_observer_cam* obs);
  vcl_vector<bwm_observer_cam*> observers();

  //:Mutators
  bool update_match(bwm_observer_cam* obs, vgl_point_2d<double> old_pt, vgl_point_2d<double> new_pt);
  void set_match(bwm_observer_cam* obs, const double x, const double y);
  void set_mode(const bool mode){mode_ = mode;}
  void set_world_pt(vgl_point_3d<double> const& wp){world_pt_ = wp;}
  //: Utility functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, bwm_corr const& c);

 protected:
  
  //members
  bool mode_; //true if image_to_image
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> > matches_;//match in each camera
  vgl_point_3d<double> world_pt_;
  
};

#endif // bwm_corr_h_
