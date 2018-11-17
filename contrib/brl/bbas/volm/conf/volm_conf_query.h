//This is brl/bbas/volm/conf/volm_conf_query.h
#ifndef volm_conf_query_h_
#define volm_conf_query_h_
//:
// \file A class to represent to an annotated query image through an 2d overhead map and estimated camera parameters
// Given one camera estimate, that is tilt, roll, focal, the query stores the coordinates (nearest points to camera center of labeled objects), orientation, height etc.
// and can be visualized by a 2-d overhead image
// Note that the coordinates are relative to camera center and represented under cylindrical coordinate system.  Also sky object is not considered in this matcher
//
// \author Yi Dong
// \date July 16, 2014
// \verbatim
//   Modifications
//    Yi Dong     SEP--2014    added height attribute
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vil/vil_save.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_image_view.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_category_io.h>
#include <volm/conf/volm_conf_object.h>


class volm_conf_query;
typedef vbl_smart_ptr<volm_conf_query> volm_conf_query_sptr;

class volm_conf_query : public vbl_ref_count
{
public:
  //: default constructor
  volm_conf_query();
  //: constructor from depth map scene
  volm_conf_query(const volm_camera_space_sptr& cam_space, const depth_map_scene_sptr& depth_scene, int const& tol_in_pixel = 4);

  //: destructor
  ~volm_conf_query() override = default;

  //: access
  std::vector<vpgl_perspective_camera<double> >& cameras()  { return cameras_; }
  std::vector<std::string>& camera_strings() { return camera_strings_; }
  std::vector<cam_angles>& camera_angles()  { return camera_angles_; }
  depth_map_scene_sptr depth_scene() const { return dm_; }
  unsigned ni() const { return ni_; }
  unsigned nj() const { return nj_; }
  unsigned ncam() const { return ncam_; }
  unsigned nobj() const { return nobj_; }
  unsigned nref() const { return nref_; }
  double altitude() const { return altitude_; }
  std::vector<std::string>& ref_obj_name() { return ref_obj_name_; }
  std::vector<std::map<std::string, volm_conf_object_sptr> >& conf_objects() { return conf_objects_; }
  std::vector<std::map<std::string, std::pair<float, float> > >& conf_objects_d_tol() { return conf_objects_d_tol_; }

  //: plot the configuration
  bool visualize_ref_objs(std::string const& in_img_file, std::string const& out_folder);
  //bool generate_top_views(std::string const& out_folder,  std::string const& filename_pre = "top_view");
  //: IO

private:
  //: image size
  unsigned ni_;
  unsigned nj_;
  //: depth map scene
  depth_map_scene_sptr   dm_;
  //: number of cameras
  unsigned ncam_;
  //: number of objects
  unsigned nobj_;
  //: number of reference objects
  unsigned nref_;
  //: camera altitude
  double altitude_;
  //: distance tolerance in image domain
  int tol_in_pixel_;

  //: vector of cameras that satisfies ground plane constraint among all possible camera calibrations
  std::vector<vpgl_perspective_camera<double> > cameras_;
  std::vector<std::string> camera_strings_;
  std::vector<cam_angles> camera_angles_;
  //: list of reference objects specified in depth scene
  std::vector<std::string> ref_obj_name_;
  //: list of configurational objects for each camera
  std::vector<std::map<std::string, volm_conf_object_sptr> > conf_objects_;
  std::vector<std::map<std::string, std::pair<float, float> > > conf_objects_d_tol_;
  std::vector<std::map<std::string, std::pair<unsigned, unsigned> > > conf_objects_pixels_  ;

  //: construct reference objects list
  bool parse_ref_object(const depth_map_scene_sptr& depth_scene);
  //: construct cameras -- also prune the camera
  bool create_perspective_cameras(const volm_camera_space_sptr& cam_space);
  //: construct the configurational objects for each camera
  bool create_conf_object();
  //: project all ground vertices of a polygon from image plane to world coordinates, return the nearest point distance and its angle relative to camera
  //  x axis (East if camera heading is zero)
  void project(vpgl_perspective_camera<double> const& cam,
               vgl_polygon<double> const& poly,
               float& min_dist, float& phi,
               unsigned& i, unsigned& j);
  //: project an image pixel to world coordinates, distance and its angle are relative to camera center
  //  return -1 for image pixel that is above horizon or out of image plane
  void project(vpgl_perspective_camera<double> const& cam,
               double const& pixel_i, double const& pixel_j,
               float& dist, float& phi);
  //: return the y coordinates given x and a line function
  double line_coord(vgl_line_2d<double> const& line, double const& x);
  //: plot a line into image
  void plot_line_into_image(vil_image_view<vil_rgb<vxl_byte> >& image, std::vector<vgl_point_2d<double> > const& line,
                            unsigned char const& r = 255, unsigned char const& g = 255, unsigned char const& b = 255,
                            double const& width = 3.0);
  void plot_dot_into_image(vil_image_view<vil_rgb<vxl_byte> >& image, vgl_point_2d<double> const& pt,
                           unsigned char const& r = 255, unsigned char const& g = 255, unsigned char const& b = 255,
                           double const& radius = 3.0);
};

#endif // volm_conf_query
