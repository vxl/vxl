//This is brl/bbas/bpgl/depth_map/depth_map_region.h
#ifndef depth_map_region_h_
#define depth_map_region_h_
//:
// \file
// \brief A class to represent a symbolic depth region
//
// \author J. L. Mundy
// \date July 31, 2012
// \verbatim
//  Modifications
//     Yi Dong     NOV--2012    added an method to check whether the given camera hypothesis is consistent with defined 2-d ground plane in the image
//     Yi Dong     JAN--2013    added attributed land_id for land classification
//     Yi Dong     SEP--2014    added height of the region
// \endverbatim
// units are in meters
#include <iostream>
#include <limits>
#include <string>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsl/vsl_binary_io.h>


class depth_map_region : public vbl_ref_count
{
 public:
  enum orientation{HORIZONTAL, FRONT_PARALLEL, SLANTED_RIGHT, SLANTED_LEFT, POROUS, INFINT, NON_PLANAR, GROUND_PLANE, VERTICAL};
  //: default constructor
  depth_map_region();
  //: standard constructor for a plane that can be moved along the camera ray
  depth_map_region(vsol_polygon_2d_sptr const& region,
                   vgl_plane_3d<double> const& region_plane,
                   double const& min_depth, double const& max_depth,
                   std::string  name,
                   depth_map_region::orientation orient,
                   unsigned const& land_id = 0,
                   double const& height = -1.0,
                   bool const& is_ref = false);


  //: constructor for a fixed plane, e.g. the ground plane
  depth_map_region(vsol_polygon_2d_sptr const& region,
                   vgl_plane_3d<double> const& region_plane,
                   std::string  name,
                   depth_map_region::orientation orient,
                   unsigned const& land_id = 0);

  //: constructor for a region of infinite distance
  depth_map_region(vsol_polygon_2d_sptr const& region,
                   std::string name);

  void set_region_3d(vpgl_perspective_camera<double> const& cam);
  void set_region_3d(double depth, vpgl_perspective_camera<double> const& cam);

  void set_min_depth(double min_depth){min_depth_ = min_depth;}
  void set_max_depth(double max_depth){max_depth_ = max_depth;}
  void set_height(double height){height_ = height;}
  void set_depth_inc(double depth_inc){depth_inc_ = depth_inc;}
  void set_order(unsigned order){order_ = order;}
  void set_active(bool active){active_ = active;}
  void set_ref(bool is_ref){is_ref_ = is_ref;}
  void set_orient_type(orientation type){orient_type_ = type;}
  void set_orient_type(unsigned ori_code);
  void set_orient_type(unsigned char ori_code)  { this->set_orient_type(static_cast<unsigned int>(ori_code)); }
  void set_land_type(unsigned land_id) { land_id_ = land_id; }
  //:accessors
  double min_depth() const {return min_depth_;}
  double max_depth() const {return max_depth_;}
  double height()    const {return height_;}
  //: unique name
  std::string name() const {return name_;}
  //: region orientation
  orientation orient_type() const{return orient_type_;}
  //: orientation color
  static std::vector<float> orient_color(unsigned char orient_code);
  //: string name for orientation
  static std::string orient_string(unsigned char orient_code);
  //: region land land classfication id
  unsigned land_id() const { return land_id_; }
  vsol_polygon_3d_sptr region_3d() const {return region_3d_;}
  vsol_polygon_2d_sptr region_2d() const {return region_2d_;}
  double depth() const {return depth_;}
  double depth_inc() const {return  depth_inc_;}
  bool active() const { return active_;}
  bool is_ref() const { return is_ref_;}
  unsigned order() const {return order_;}
  vsol_point_2d_sptr centroid_2d() const;
  //:
  //  depth is defined as distance on  the x-y plane in the direction
  //  of the camera ray through the region centroid. An assertion is
  //  thrown if the camera ray is perpendicular to the 3-d region normal
  static vsol_polygon_3d_sptr
    region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d,
                    vgl_vector_3d<double> const& region_normal,
                    double depth, // plane is moved along ray to depth
                    vpgl_perspective_camera<double> const& cam);

  //: The 3-d region is fixed as in the case of the ground plane.
  // The 3-d vertices are the intersection of the camera rays with the plane.
  static vsol_polygon_3d_sptr
    region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d,
                    vgl_plane_3d<double> const& region_plane,
                    vpgl_perspective_camera<double> const& cam);

  //: The 3-d region is fixed as in the case of the ground plane.
  //  it returns false when the input camera can NOT project the defined 2-d ground to 3-d ground
  bool region_ground_2d_to_3d(vpgl_perspective_camera<double> const& cam);

  //: The direction vector that is parallel to the ground plane and lies in the plane of the camera principal ray and the z axis.
  //  Provides the normal to the plane perpendicular to the gound plane
  //  and orthogonal to the camera principal ray.
  static vgl_vector_3d<double>
    perp_ortho_dir(vpgl_perspective_camera<double> const& cam);

  //: changes the 2-d region so that points near the horizon move to the specified max depth.
  //  The proximity scale factor defines
  //  a threshold on "near" as a factor times the distance to the
  //  point closest to the horizon. This approach is likely to be
  //  fragile if the ground plane region has a convex shape.
  bool set_ground_plane_max_depth(double max_depth,
                                  vpgl_perspective_camera<double> const& cam,
                                  double proximity_scale_factor);

  //: compute the homography between the image and 3d region plane coords
  bool img_to_region_plane(vpgl_perspective_camera<double> const& cam,
                           vgl_h_matrix_2d<double>& H) const;

  //: update the input depth image with *this* region
  // Currently assumes disjoint regions so that sorting on depth
  // is not required.
  bool update_depth_image(vil_image_view<float>& depth_image,
                          vpgl_perspective_camera<double> const& cam,
                          double downsample_ratio = 1.0) const;
  //: version
  unsigned version() const {return 4;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

 protected:
  bool active_;      // if active is true then inserted into the depth map
  bool is_ref_;      // if is_ref is true then this object is set to be a reference object for further usage (default value is false)
  unsigned order_;   // depth order
  unsigned land_id_; // land classification id defined in volm_label_table (in volm_io)
  orientation orient_type_;
  std::string name_;
  // depth value for region centroid
  double depth_; //current depth estimate
  double min_depth_; // closest possible centroid depth
  double max_depth_; // furthest possible centroid depth
  double depth_inc_; // step size from min to max depth
  // height value for region
  double height_;
  vgl_plane_3d<double> region_plane_;
  vsol_polygon_2d_sptr region_2d_;
  vsol_polygon_3d_sptr region_3d_;//:cached
};

#include "depth_map_region_sptr.h"

void vsl_b_write(vsl_b_ostream& os, depth_map_region const*);
void vsl_b_read(vsl_b_istream &is, depth_map_region*&);

void vsl_b_write(vsl_b_ostream& os, depth_map_region_sptr const&);
void vsl_b_read(vsl_b_istream &is, depth_map_region_sptr&);

#endif //depth_map_region_h_
