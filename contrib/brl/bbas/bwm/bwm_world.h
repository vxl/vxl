#ifndef bwm_world_h_
#define bwm_world_h_

#include "bwm_observable_sptr.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>

class bwm_world {
public:
  static bwm_world* instance();

  virtual ~bwm_world() {}

  //: A 3-d point in the world - usually where correspondences are established
  bool world_pt(vgl_point_3d<double>& world_pt)
    {if(!world_pt_valid_) return false; world_pt=world_pt_;
    return true;}

  bool world_pt_valid(){return world_pt_valid_;}

  vgl_point_3d<double> world_pt() { return world_pt_; }

  //: A horizontal plane through the world point
  vgl_plane_3d<double> world_plane() {return world_plane_; }

  //: the set of observable objects in the world such as feature meshes
  vcl_vector<bwm_observable_sptr> objects() { return objects_; }

  void add(bwm_observable_sptr obj) { objects_.push_back(obj); }

  //: removes the observable from world, returns true if deletion is successful,
  // false if it cannot be found
  bool remove(bwm_observable_sptr obj);

  //: Typically the world point for 1 pt correspondences
  void set_world_pt(vgl_point_3d<double> const& pt);

  //: Usually set to be a horizontal plane through the world point
  //maybe this method shouldn't exist - i.e. insist world pt and world plane
  //are consistent - leave for now
  void set_world_plane(vgl_plane_3d<double> const& plane) { world_plane_ = plane; }

protected:
  //: default constructor - not accessable since world is a singleton
  bwm_world(): world_pt_valid_(false){} 

  //: the singleton world instance
  static bwm_world* instance_;

  //: The point common to correspondences used construct the 3-d intersection
  // could also be a point defined by LIDAR or GPS
  vgl_point_3d<double> world_pt_;

  //: Is the world point valid? May not have been set
  bool world_pt_valid_;
  
  //: The plane through the world point. Use Euclidean plane not homg plane
  vgl_plane_3d<double> world_plane_;

  //: The set of objects in the world such as mesh and vsol
  vcl_vector<bwm_observable_sptr> objects_;
};

#endif
