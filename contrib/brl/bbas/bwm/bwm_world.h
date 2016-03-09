#ifndef bwm_world_h_
#define bwm_world_h_
//:
// \file

#include "bwm_observable_sptr.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_lvcs.h>

class bwm_world
{
 public:
  static bwm_world* instance();

  virtual ~bwm_world() {}

  //: A 3-d point in the world - usually where correspondences are established
  bool world_pt(vgl_point_3d<double>& world_pt)
  { if (!world_pt_valid_) return false; world_pt=world_pt_; return true; }

  bool world_pt_valid() const { return world_pt_valid_; }

  vgl_point_3d<double> world_pt() const { return world_pt_; }

  //: A horizontal plane through the world point
  vgl_plane_3d<double> world_plane() const { return world_plane_; }

  //: the number of observable objects in the world such as feature meshes
  unsigned n_objects() const { return objects_.size(); }

  //: the set of observable objects in the world such as feature meshes
  std::vector<bwm_observable_sptr> objects() { return objects_; }

  bool add(bwm_observable_sptr obj);

  //: removes the observable from world.
  //  Returns true if deletion is successful, false if it cannot be found
  bool remove(bwm_observable_sptr obj);

  //: Typically the world point for 1 pt correspondences
  void set_world_pt(vgl_point_3d<double> const& pt);

  //: Usually set to be a horizontal plane through the world point
  // Maybe this method shouldn't exist - i.e. insist world pt and world plane
  // are consistent - leave for now
  void set_world_plane(vgl_plane_3d<double> const& plane) { world_plane_=plane; }

  bool lvcs_valid() const { return lvcs_valid_; }

  void set_lvcs(double lat, double lon, double elev);

  void set_lvcs(vpgl_lvcs &lvcs) { lvcs_ = lvcs; lvcs_valid_ = true; }

  //: returns true if it can figure out a value, false otherwise
  bool get_lvcs(vpgl_lvcs &lvcs);

  void load_shape_file();
  //*********** Save methods
  void save_ply();
  void save_gml();
  void save_kml();
  void save_kml_collada();
  void save_x3d();

  // cleans up the world of objects
  void clear();

 protected:
  //: default constructor - not accessible since world is a singleton
  bwm_world(): world_pt_valid_(false), lvcs_valid_(false) {}

  //: the singleton world instance
  static bwm_world* instance_;

  //: The point common to correspondences used to construct the 3-d intersection.
  // Could also be a point defined by LIDAR or GPS
  vgl_point_3d<double> world_pt_;

  //: Is the world point valid? May not have been set
  bool world_pt_valid_;

  //: The plane through the world point. Use Euclidean plane not homg plane
  vgl_plane_3d<double> world_plane_;

  //: The set of objects in the world such as mesh and vsol
  std::vector<bwm_observable_sptr> objects_;

  vpgl_lvcs lvcs_;

  bool lvcs_valid_;
};

#endif
