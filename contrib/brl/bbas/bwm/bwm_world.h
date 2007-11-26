#ifndef bwm_world_h_
#define bwm_world_h_

#include "bwm_observable_sptr.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

class bwm_world {
public:
  static bwm_world* instance();

  virtual ~bwm_world() {}

  vgl_point_3d<double> world_pt() { return world_pt_; }
  vgl_homg_plane_3d<double> world_plane() {return world_plane_; }
  vcl_vector<bwm_observable_sptr> objects() { return objects_; }

  void add(bwm_observable_sptr obj) { objects_.push_back(obj); }
  void set_world_pt(vgl_point_3d<double> pt) { world_pt_ = pt; }
  void set_world_plane(vgl_homg_plane_3d<double> plane) { world_plane_ = plane; }

protected:
  bwm_world(){} 
  static bwm_world* instance_;

  vgl_point_3d<double> world_pt_;
  vgl_homg_plane_3d<double> world_plane_;
  vcl_vector<bwm_observable_sptr > objects_;
};

#endif