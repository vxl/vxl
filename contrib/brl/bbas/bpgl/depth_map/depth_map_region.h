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
// \endverbatim
// units are in meters
#include <vbl/vbl_ref_count.h>
#include <vcl_limits.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vcl_string.h>
class depth_map_region : public vbl_ref_count
{
 public:
  enum orientation { GROUND_PLANE, VERTICAL, INFINITE, NOT_DEF };
  //: default constructor
  depth_map_region();
  //: standard constructor for a plane that can be moved along the camera ray
  depth_map_region(vsol_polygon_2d_sptr const& region, 
                   vgl_plane_3d<double> const& region_plane,
                   double min_depth, double max_depth,
                   vcl_string name,
                   depth_map_region::orientation orient);

  //: constructor for a fixed plane, e.g. the ground plane
  depth_map_region(vsol_polygon_2d_sptr const& region, 
                   vgl_plane_3d<double> const& region_plane,
                   vcl_string name,
                   depth_map_region::orientation orient);

  //: constructor for a region of infinite distance
  depth_map_region(vsol_polygon_2d_sptr const& region,
                   vcl_string name = "sky");

  //: unique name
  vcl_string name() const {return name_;}
  //: region orientation
  orientation orient_type() const{return orient_type_;}

 
  void set_region_3d(vpgl_perspective_camera<double> const& cam);
  void set_region_3d(double depth, vpgl_perspective_camera<double> const& cam);

  void set_min_depth(double min_depth){min_depth_ = min_depth;}
  void set_max_depth(double max_depth){max_depth_ = max_depth;}

  //accessors
  double min_depth() const {return min_depth_;}
  double max_depth() const {return max_depth_;}
  vsol_polygon_3d_sptr region_3d() const {return region_3d_;}
  vsol_polygon_2d_sptr region_2d() const {return region_2d_;}
  double depth() const {return depth_;}
  //utility function for case where region plane is fixed
  static vsol_polygon_3d_sptr 
    region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d, 
                    vgl_plane_3d<double> const& region_plane,
                    double d,// as in a, b, c, d
                    vpgl_perspective_camera<double> const& cam);

  static vsol_polygon_3d_sptr 
    region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d, 
                    vgl_vector_3d<double> const& region_normal,
                    double depth, // plane is moved along ray to depth
                    vpgl_perspective_camera<double> const& cam);

  static vsol_polygon_3d_sptr 
    region_2d_to_3d(vsol_polygon_2d_sptr const& region_2d, 
                    vgl_plane_3d<double> const& region_plane, //fixed plane
                    vpgl_perspective_camera<double> const& cam);

  //: clip ground plane region by horizon line. tolerance is 
  // Returns false if:
  // 1) region is not a ground plane
  // 2) region needs splittng, since part lies above or on the horizon
  // 3) throws an assertion if the region is entirely above the horizon
  bool set_ground_plane_max_depth(double max_depth, 
                                  vpgl_perspective_camera<double> const& cam,
                                  double tolerance);

  bool update_depth_image(vil_image_view<double>& depth_image,
                          vpgl_perspective_camera<double> const& cam) const;

 protected:
  orientation orient_type_;
  vcl_string name_;
  // depth value for region centroid
  double depth_; //current depth estimate
  double min_depth_; // closest possible centroid depth
  double max_depth_; // furthest possible centroid depth
  vgl_plane_3d<double> region_plane_;
  vsol_polygon_2d_sptr region_2d_;
  vsol_polygon_3d_sptr region_3d_;//:cached
};
#endif //depth_map_region_h_
