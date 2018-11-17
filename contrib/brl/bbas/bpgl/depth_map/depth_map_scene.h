//This is brl/bbas/bpgl/depth_map/depth_map_scene.h
#ifndef depth_map_scene_h_
#define depth_map_scene_h_
//:
// \file
// \brief A class to represent a symbolic depth map
//
// \author J. L. Mundy
// \date July 31, 2012
// \verbatim
//  Modifications
//     Yi Dong     NOV--2012    added an order argument in add_region method, with default 0
//     Yi Dong     NOV--2012    added an method to set the depth image size
//     Yi Dong     NOV--2012    modify sky, ground_plane as vectors and methods
//                              associated to treat mutliple region defined as ground/sky in the query img.
//                              Therefore when creating depth_map, need to ensure all ground/sky are active.
// \endverbatim
// The idea is that the absolute depth map for a scene cannot often
// be extracted from a single image since there can be unknown parameters.
// For example, an object depth in the image cannot be known unless the
// camera is known as well as the 3-d dimensions of the object. Moreover,
// the object can have surface depth variations that may not be able to
// be recovered, e.g. shape from shading is fragile.
//
// Instead it is necessary to express the depth map analytically in terms
// of regions whose depth is a parameter. For a given specification of depth,
// the lateral extent of the region in 3-d is determined. In this
// initial version, the scene is represented by a ground plane and
// regions perpendicular to the ground plane and the viewing direction,
//
// A camera is required to specify the depth map, which is rendered
// for that camera view.
//
#include <vector>
#include <iostream>
#include <limits>
#include <map>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include "depth_map_region_sptr.h"
#include "depth_map_region.h"
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

class scene_depth_iterator;
class depth_map_scene : public vbl_ref_count
{
 public:
  depth_map_scene()
  : ni_(0), nj_(0), image_path_("") {}

  //: ni and nj are the required image dimensions
  depth_map_scene(unsigned ni, unsigned nj)
  : ni_(ni), nj_(nj),image_path_("") {}

  depth_map_scene(unsigned ni, unsigned nj,
                  std::string  image_path,
                  vpgl_perspective_camera<double> const& cam,
                  depth_map_region_sptr const& ground_plane,
                  depth_map_region_sptr const& sky,
                  std::vector<depth_map_region_sptr> const& scene_regions);

  //: accessors
  unsigned ni() const {return ni_;}
  unsigned nj() const {return nj_;}
  std::string image_path() const {return image_path_;}
  std::vector<depth_map_region_sptr> ground_plane() const {return ground_plane_;}
  std::vector<depth_map_region_sptr> sky() const {return sky_;}
  std::vector<depth_map_region_sptr> scene_regions() const;
  vpgl_perspective_camera<double> cam() const{return cam_;}
  //: set members
  void set_image_path(std::string const& path){image_path_ = path;}
  void set_image_size(unsigned const& ni, unsigned const& nj) { ni_ = ni;  nj_ = nj; }
  void set_camera(vpgl_perspective_camera<double> const& cam) {cam_ = cam;}
  void set_ground_plane(const vsol_polygon_2d_sptr& ground_plane);
  void set_sky(const vsol_polygon_2d_sptr& ground_plane);

  //: set the scene depth of a movable plane. returns false if plane is fixed
  bool set_depth(double depth, std::string const& name);

  //: set the maximum depth of the ground plane
  // For example the depth can be limited by the curvature of the Earth
  // proximity scale factor is with respect to the closest ground plane
  // point to the horizon. That is, points less than scale * distance to
  // closest point are moved to max_depth
  void set_ground_plane_max_depth(double max_depth,
                                  double proximity_scale_factor = 3.0);

  //:add a region orthogonal to the ground plane and perpendicular to the plane containing the principal ray and the z axis
  void add_ortho_perp_region(vsol_polygon_2d_sptr const& region,
                             double min_distance, double max_distance,
                             std::string name);

  //: add a region with an arbitrary orientation (not currently used)
  void add_region(vsol_polygon_2d_sptr const& region,
                  vgl_vector_3d<double> plane_normal,
                  double min_distance,
                  double max_distance,
                  const std::string& name,
                  depth_map_region::orientation orient,
                  unsigned order = 0,
                  unsigned land_id = 40,
                  double height = -1.0,
                  bool is_ref = false);

  //: add a ground region into ground_plane_
  void add_ground(vsol_polygon_2d_sptr const& ground_plane,
                  double min_depth = 0,
                  double max_depth = 0,
                  unsigned order = 0,
                  std::string name = "ground_plane",
                  unsigned land_id = 40,
                  double height=-1.0);

  //: add a sky region into sky_
  void add_sky(vsol_polygon_2d_sptr const& sky,
               unsigned order = 0,
               std::string name = "sky");

  //: return a depth map of distance from the camera. Downsample accordingly
  vil_image_view<float> depth_map(unsigned log2_downsample_ratio);

  //: return a depth map of the specified region, use the 'ground_plane' and 'sky' strings to specify those two regions. downsample accordingly.
  vil_image_view<float> depth_map(const std::string& region_name, unsigned log2_downsample_ratio, double gp_dist_cutoff = 20000);

  //: the iterator at the start of depth search. resets the depth_states_.
  scene_depth_iterator begin();

  //: the iterator at the end of depth search.
  scene_depth_iterator end();

  //: move vert regions to next depth configuration. returns false if done
  bool next_depth();

  //: initialize the movable depth configuration
  void init_depths();

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  //: binary io version no
  unsigned version(){return 3;}

  //: debug utilities
  void print_depth_states();

  //: match to a given continuous depth image
  bool match(vil_image_view<float> const& depth_img, vil_image_view<float> const& vis_img, unsigned level, float& score);

  //: match to a given continuous depth image, use the ground_plane constraint as well
  bool match_with_ground(vil_image_view<float> const& depth_img, vil_image_view<float> const& vis_img, unsigned level, float ground_depth_std_dev, float& score);

 protected:
  unsigned ni_, nj_; //: depth map dimensions
  std::string image_path_;
  std::map<std::string, depth_map_region_sptr> scene_regions_;
  std::vector<depth_map_region_sptr> ground_plane_;
  std::vector<depth_map_region_sptr> sky_;
  vpgl_perspective_camera<double> cam_;
  //: a vector of regions with assigned depths
  std::vector<depth_map_region_sptr> depth_states_;
};

//: An iterator for parameterized depth maps
//  Scans the vertical movable regions in depth, while respecting
//  depth ordering constraints, e.g. region A is closer than region B.
class scene_depth_iterator
{
 public:
  scene_depth_iterator(depth_map_scene* scene = nullptr)
  : end_(false), scene_(scene) {}

  ~scene_depth_iterator() = default;

  //: returns a reference to the scene to enable access to scene methods
  depth_map_scene& operator*() {
    return *scene_;
  }

  //: returns a pointer to the scene to enable access to scene methods
  depth_map_scene* operator->() {
    return scene_;
  }

  //: increments the depth arrangement of vertical regions
  scene_depth_iterator& operator++() {
    if (!scene_) // if scene_ is null, there is no effect
      return *this;
    if (!scene_->next_depth()) end_ = true;
    return *this;
  }

  //: increments the depth arrangement of vertical regions n_inc times
  scene_depth_iterator& operator+=(unsigned n_inc) {
    if (!scene_)
      return *this;

    for (unsigned k =0; k<n_inc; ++k) {
      if (!scene_->next_depth()) { end_ = true; break; }
    }
    return *this;
  }

  //: Only considers the state of end_ in determining equality.
  //  enables the test for the end of depth region arrangements
  bool operator==(const scene_depth_iterator& it) {
    return end_ == it.end();
  }
  bool operator!=(const scene_depth_iterator& it) {
    return !(end_ == it.end());
  }

  //: defines the state of completing all the depth arrangements
  void set_end(){end_ = true;}
  bool end() const{return end_;}
 private:
  bool end_;
  depth_map_scene* scene_;
};

//: a functor to compare region depth order
struct compare_order {
  bool operator ()(depth_map_region_sptr ra, depth_map_region_sptr rb)
  { return ra->order() < rb->order(); }
};

#include "depth_map_scene_sptr.h"

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene* sptr);
void vsl_b_read(vsl_b_istream &is, depth_map_scene*& sptr);
void vsl_b_write(vsl_b_ostream& os, const depth_map_scene_sptr& sptr);
void vsl_b_read(vsl_b_istream &is, depth_map_scene_sptr& sptr);

#endif //depth_map_scene_h_
