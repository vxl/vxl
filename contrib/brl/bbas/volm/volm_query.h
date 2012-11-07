//This is brl/bbas/volm/volm_query.h
#ifndef volm_query_h_
#define volm_query_h_
//:
// \file
// \brief  A class to represent a volumetric matching query through an image mark-up and estimated camera parameters
//				The container is numbers of points distributed on a unit spherecial surface
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
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

class volm_query : public vbl_ref_count
{
public:
  //: default consturctor
  volm_query() {}
  //: constructor
  //: specify the initial focal value and its confidence
  volm_query(depth_map_scene_sptr dm,
             volm_spherical_container_sptr sph,
			 volm_spherical_shell_container_sptr container,
			 unsigned ni,         unsigned nj,
			 double init_focal,   double conf_focal,
			 double init_heading, double conf_heading,
			 double init_tilt,    double conf_tilt,
			 double init_roll,    double conf_roll,
			 double altitude);

  //: specify the right fov and top fov and its confidence
  volm_query(depth_map_scene_sptr dm,
             volm_spherical_container_sptr sph,
			 volm_spherical_shell_container_sptr container,
			 unsigned ni,		       unsigned nj,
			 double init_right_fov,    double conf_right_fov,
			 double init_top_fov,      double conf_top_fov,
			 double init_heading, double conf_heading,
			 double init_tilt,    double conf_tilt,
			 double init_roll,    double conf_roll,
			 double altitude);

  // === accessors ===
  vcl_vector<vcl_vector<unsigned char> >& query() {return query_;}
  vcl_vector<double>& focals()   { return focals_; }
  vcl_vector<double>& headings() { return headings_; }
  vcl_vector<double>& tilts()    { return tilts_; }
  vcl_vector<double>& rolls()    { return rolls_; }
  unsigned get_cam_num() { return (Nf_+1)*(Nh_+1)*(Nt_+1)*(Nr_+1); }
  unsigned get_query_size() { return query_size_; }

  //: write vrml for spherical container and camera hypothesis
  void draw_template(vcl_string const& vrml_fname, volm_spherical_shell_container_sptr const& sph_shell, depth_map_scene_sptr const& dm);
  //: write query image showing the depth map geometry and the penetrating ray
  void draw_query_images(vcl_string const& out_dir, depth_map_scene_sptr const& dm);
  //: visualized the query camera using the spherical shell geometry
  void visualize_query(vcl_string const& prefix, volm_spherical_shell_container_sptr const& sph_shell);

protected:
  // image size
  unsigned ni_, nj_;
  // camera parameters --- use even number later to ensure the init_value and init_value +/- conf_value is covered
  unsigned Nf_;
  unsigned Nh_;
  unsigned Nt_;
  unsigned Nr_; 
  vcl_vector<double> focals_;
  vcl_vector<double> headings_;
  vcl_vector<double> tilts_;
  vcl_vector<double> rolls_;
  double altitude_;
  double init_focal_;
  double conf_focal_;
  vcl_vector<vpgl_perspective_camera<double> > cameras_;
  vcl_vector<vcl_vector<unsigned char> > query_;
  unsigned int query_size_;
  vcl_vector<vgl_point_3d<double> > query_points_;

  unsigned char fetch_depth(double const& u, double const& v, depth_map_scene_sptr const& dm, volm_spherical_container_sptr const& sph);
  bool query_ingest(depth_map_scene_sptr const& dm, volm_spherical_container_sptr const& sph);
  void draw_viewing_volume(vcl_string const& fname, depth_map_scene_sptr const& dm, vpgl_perspective_camera<double> cam, float r, float g, float b);
  void draw_rays(vcl_string const& fname);
  void draw_polygon(vil_image_view<vxl_byte>& img, vgl_polygon<double> const& poly);
  void draw_dot(vil_image_view<vxl_byte>& img, vgl_point_3d<double> const& world_point, vpgl_perspective_camera<double> const& cam);
};

#endif  // volm_query_h_
