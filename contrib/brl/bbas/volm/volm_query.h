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
             volm_spherical_shell_container_sptr const& sph_shell);

  // === accessors ===
  vcl_vector<vcl_vector<unsigned char> >& min_dist() {return min_dist_;}
  vcl_vector<vcl_vector<unsigned char> >& max_dist() {return max_dist_;}
  vcl_vector<vcl_vector<unsigned char> >& order() { return order_; }
  vcl_vector<unsigned>& valid_ray_num() { return ray_count_; }
  depth_map_scene_sptr depth_scene() const { return dm_; }
  vcl_vector<double>& top_fovs()   { return top_fov_;  }
  vcl_vector<double>& headings()  { return headings_; }
  vcl_vector<double>& tilts()     { return tilts_; }
  vcl_vector<double>& rolls()     { return rolls_; }
  unsigned get_cam_num()          { return (unsigned)cameras_.size(); }
  unsigned get_query_size()       { return query_size_; }
  unsigned get_valid_ray_num(unsigned const& cam_idx)    { return ray_count_[cam_idx]; }

  //: write vrml for spherical container and camera hypothesis
  void draw_template(vcl_string const& vrml_fname);
  //: write query image showing the depth map geometry and the penetrating ray
  void draw_query_images(vcl_string const& out_dir);
  //: visualized the query camera using the spherical shell geometry
  void visualize_query(vcl_string const& prefix);

protected:
  //: image size
  unsigned ni_, nj_;
  //: image category
  vcl_string img_category_;
  //: depth map scene
  depth_map_scene_sptr dm_;
  volm_spherical_container_sptr sph_depth_;
  volm_spherical_shell_container_sptr sph_;
  //: vector of 2d polygons for fetch minimum depth, in order of the object order defined in labelme xml
  vcl_vector<vgl_polygon<double> > dm_poly_;
  vcl_vector<double> min_depths_;
  vcl_vector<double> max_depths_;
  vcl_vector<unsigned> order_depths_;
  vcl_vector<unsigned> ray_count_;
  //: camera parameters --- use even number later to ensure the init_value and init_value +/- conf_value is covered
  unsigned Ntf_;
  unsigned Nrf_;
  unsigned Nh_;
  unsigned Nt_;
  unsigned Nr_;

  double init_focal_;
  double head_, head_d_;
  double tilt_, tilt_d_;
  double roll_, roll_d_;
  double tfov_, tfov_d_;

  vcl_vector<double> top_fov_;
  vcl_vector<double> headings_;
  vcl_vector<double> tilts_;
  vcl_vector<double> rolls_;
  vcl_vector<vpgl_perspective_camera<double> > cameras_;
  vcl_vector<vcl_vector<unsigned char> > min_dist_;
  vcl_vector<vcl_vector<unsigned char> > max_dist_;
  vcl_vector<vcl_vector<unsigned char> > order_;
  unsigned int query_size_;
  vcl_vector<vgl_point_3d<double> > query_points_;

  unsigned char fetch_depth(double const& u, double const& v, unsigned char& order, unsigned char& max_dist);
  bool query_ingest();
  void create_cameras();
  void generate_depth_poly_order();
  void draw_viewing_volume(vcl_string const& fname, vpgl_perspective_camera<double> cam, float r, float g, float b);
  void draw_rays(vcl_string const& fname);
  void draw_polygon(vil_image_view<vxl_byte>& img, vgl_polygon<double> const& poly);
  void draw_dot(vil_image_view<vxl_byte>& img, vgl_point_3d<double> const& world_point, unsigned char const& depth, vpgl_perspective_camera<double> const& cam);
};

#endif  // volm_query_h_
