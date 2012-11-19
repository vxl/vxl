//This is brl/bbas/volm/volm_query.h
#ifndef volm_query_h_
#define volm_query_h_
//:
// \file
// \brief  A class to represent a volumetric matching query through an image mark-up and estimated camera parameters
//              The container is numbers of points distributed on a unit spherecial surface
//              The camera parameters, heading, tilt, roll, right_fov, top_fov, have resolution one degree, and their
//              default values are chosen based on query image category (desert/coast)
//              Use top viewing angle to define the viewing volume
//
// \author
// \date October 23, 2012
// \verbatim
//   Modifications
//
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vgl/vgl_polygon.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

class volm_query : public vbl_ref_count
{
 public:
  //: default consturctor
  volm_query() {}
  //: constructor
  volm_query(vcl_string const& cam_kml_file,
             vcl_string const& label_xml_file,
             volm_spherical_container_sptr const& sph,
             volm_spherical_shell_container_sptr const& sph_shell,
             bool const& use_default = true);

  // === accessors ===
  vcl_vector<vcl_vector<unsigned char> >& min_dist() {return min_dist_;}
  vcl_vector<vcl_vector<unsigned char> >& max_dist() {return max_dist_;}
  vcl_vector<vcl_vector<unsigned char> >& order() { return order_; }
  vcl_vector<unsigned>& valid_ray_num() { return ray_count_; }
  depth_map_scene_sptr depth_scene() const { return dm_; }
  vcl_vector<depth_map_region_sptr>& depth_regions() { return depth_regions_; }
  unsigned get_cam_num()   const  { return (unsigned)cameras_.size(); }
  unsigned get_query_size() const { return query_size_; }
  unsigned get_valid_ray_num(unsigned const& cam_idx) const { return ray_count_[cam_idx]; }

  //: write vrml for spherical container and camera hypothesis
  void draw_template(vcl_string const& vrml_fname);
  //: write query image showing the depth map geometry and the penetrating ray
  void draw_query_images(vcl_string const& out_dir);
  void draw_query_image(unsigned i, vcl_string const& out_name);
  vcl_string get_cam_string(unsigned i) { return camera_strings_[i]; }
  //: visualized the query camera using the spherical shell geometry
  void visualize_query(vcl_string const& prefix);
  //: generate rgb depth image for given camera id and depth value
  void depth_rgb_image(vcl_vector<unsigned char> const& values, unsigned const& cam_id, vil_image_view<vil_rgb<vxl_byte> >& out_img);
  //: initial camera parameters read from camera kml
  double init_focal_;
  double head_, head_d_, head_inc_;
  double tilt_, tilt_d_, tilt_inc_;
  double roll_, roll_d_, roll_inc_;
  double tfov_, tfov_d_, tfov_inc_;
  double altitude_;

 protected:
  //: a check whether use the viewing volume values provided by camera kml
  bool use_default_;
  //: image size
  unsigned ni_, nj_;
  //: image category
  vcl_string img_category_;
  //: depth map scene
  depth_map_scene_sptr dm_;
  volm_spherical_container_sptr sph_depth_;
  volm_spherical_shell_container_sptr sph_;
  //: vector of depth_map_region which arranged by their orders
  double d_threshold_;
  vcl_vector<depth_map_region_sptr> depth_regions_;
  vcl_vector<vgl_polygon<double> > dm_poly_;
  vcl_vector<unsigned> ray_count_;
  //: camera parameters --- use even number later to ensure the init_value and init_value +/- conf_value is covered
  vcl_vector<vpgl_perspective_camera<double> > cameras_;
  vcl_vector<vcl_string> camera_strings_;
  //: ingested query information
  vcl_vector<vcl_vector<unsigned char> > min_dist_;
  vcl_vector<vcl_vector<unsigned char> > max_dist_;
  vcl_vector<vcl_vector<unsigned char> > order_;
  unsigned int query_size_;
  vcl_vector<vgl_point_3d<double> > query_points_;

  unsigned char fetch_depth(double const& u, double const& v, unsigned char& order, unsigned char& max_dist, vil_image_view<float> const& depth_img);
  bool query_ingest();
  void create_cameras();
  void generate_regions();
  void draw_viewing_volume(vcl_string const& fname, vpgl_perspective_camera<double> cam, float r, float g, float b);
  void draw_rays(vcl_string const& fname);
  void draw_polygon(vil_image_view<vil_rgb<vxl_byte> >& img, vgl_polygon<double> const& poly, unsigned char const& depth);
  void draw_dot(vil_image_view<vil_rgb<vxl_byte> >& img, 
                vgl_point_3d<double> const& world_point,
                unsigned char const& depth, 
                vpgl_perspective_camera<double> const& cam);
};

#endif  // volm_query_h_
