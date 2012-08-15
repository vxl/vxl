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
#include <vcl_vector.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include "depth_map_region_sptr.h"
#include "depth_map_region.h"
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_limits.h>
#include <vcl_map.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

class scene_depth_iterator;
class depth_map_scene : public vbl_ref_count
{
 public:
  depth_map_scene() : ni_(0), nj_(0), ground_plane_(0)
  {}

  //: ni and nj are the required image dimensions
  depth_map_scene(unsigned ni, unsigned nj) :
    ni_(ni), nj_(nj), ground_plane_(0) {}

  depth_map_scene(unsigned ni, unsigned nj,
                  vpgl_perspective_camera<double> const& cam,
                  depth_map_region_sptr const& ground_plane,
                  depth_map_region_sptr const& sky,
                  vcl_vector<depth_map_region_sptr> const& scene_regions);
  //: accessors
  unsigned ni() const {return ni_;}
  unsigned nj() const {return nj_;}
  depth_map_region_sptr ground_plane() const {return ground_plane_;}
  depth_map_region_sptr sky() const {return sky_;}
  vcl_vector<depth_map_region_sptr> scene_regions() const;
  //: set members
  void set_camera(vpgl_perspective_camera<double> const& cam){cam_ = cam;}
  void set_ground_plane(vsol_polygon_2d_sptr ground_plane);
  void set_sky(vsol_polygon_2d_sptr ground_plane);

  //: set the scene depth of a movable plane. returns false if plane is fixed
  bool set_depth(double depth, vcl_string const& name);

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
                             vcl_string name);

  //: add a region with an arbitrary orientation (not currently used)
  void add_region(vsol_polygon_2d_sptr const& region,
                  vgl_vector_3d<double> plane_normal,
                  double min_distance,
                  double max_distance,
                  vcl_string name,
                  depth_map_region::orientation orient);

  //: return a depth map of distance from the camera
  vil_image_view<float> depth_map();

  //: the iterator at the start of depth search
  scene_depth_iterator begin();  

  //: the iterator at the end of depth search
  scene_depth_iterator end();  

  //: move vert regions to next depth configuration. returns false if done
  bool next_depth();
  //: initialize the movable depth configuration
  void init_depths();
  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);
  //: debug utilities
  void print_depth_states();
 protected:
  unsigned ni_, nj_; //: depth map dimensions
  vcl_map<vcl_string, depth_map_region_sptr> scene_regions_;
  depth_map_region_sptr ground_plane_;
  depth_map_region_sptr sky_;
  vpgl_perspective_camera<double> cam_;
  vcl_vector<depth_map_region_sptr> depth_states_;
};

class scene_depth_iterator
{
 public:
  scene_depth_iterator(depth_map_scene* scene = 0): scene_(scene),
    end_(false){}

  ~scene_depth_iterator(){}

  depth_map_scene& operator*(){
    return *scene_;
  }

  depth_map_scene* operator->(){
    return scene_;
  }
  scene_depth_iterator& operator++(){
    if(!scene_)
      return *this;
    if(!scene_->next_depth()){
      end_ = true;
      return *this;
    }
  }
  scene_depth_iterator& operator+=(unsigned inc){
    if(!scene_)
      return *this;
    for(unsigned k =0; k<inc; ++k){
      if(!scene_->next_depth()){
        end_ = true;
      return *this;
      }
    }
    return *this;
  }
      

  bool operator==(const scene_depth_iterator& it){
    return end_ == it.end();
  }
  bool operator!=(const scene_depth_iterator& it){
    return !(end_ == it.end());
  }
  void set_end(){end_ = true;}
  bool end() const{return end_;}
 private:
  bool end_;
  depth_map_scene* scene_;
};

struct compare_order {
  bool operator ()(depth_map_region_sptr ra, depth_map_region_sptr rb)
  { return ra->order() < rb->order(); }
};

#include <depth_map/depth_map_scene_sptr.h>

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene* sptr);
void vsl_b_read(vsl_b_istream &is, depth_map_scene*& sptr);
void vsl_b_write(vsl_b_ostream& os, const depth_map_scene_sptr& sptr);
void vsl_b_read(vsl_b_istream &is, depth_map_scene_sptr& sptr);

#endif //depth_map_scene_h_
