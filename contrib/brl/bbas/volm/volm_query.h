//This is brl/bbas/volm/volm_query.h
#ifndef volm_query_h_
#define volm_query_h_
//:
// \file
// \brief  A class to represent a volumetric matching query through an image mark-up and estimated camera parameters
// The container is numbers of points distributed on a unit spherecial surface
// The camera parameters, heading, tilt, roll, right_fov, top_fov, have resolution one degree, and their
// default values are chosen based on query image category (desert/coast)
// Use top viewing angle to define the viewing volume
//
// \author Yi Dong
// \date October 23, 2012
// \verbatim
//   Modifications
//    Yi Dong         Jan-2013   added functions to generate object based query infomation for object based volm_matcher
//    Yi Dong         Jan-2013   added object orientation and object NLCD land classification
//    JLM             Jan 20, 2013 Added constructor from depth_map_scene
//    Ozge C. Ozcanli Jan 30, 2013 -- replacing old camera space construction functionality with the use of volm_camera_space class
//    Yi Dong         Feb-2013   added binary i/o
// \endverbatim
//

#include <iostream>
#include <set>
#include <vbl/vbl_ref_count.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vgl/vgl_polygon.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_camera_space.h>
#include <vsl/vsl_binary_io.h>
#include <volm/volm_category_io.h>

class volm_query : public vbl_ref_count
{
 public:
  //: default consturctor
  volm_query() = default;
  //: constructor from files
  volm_query(const volm_camera_space_sptr& cam_space,
             std::string const& label_xml_file,
             std::string const& category_file,
             volm_spherical_container_sptr const& sph,
             volm_spherical_shell_container_sptr const& sph_shell);

  //: constructor from depth map scene
  volm_query(const volm_camera_space_sptr& cam_space,
             std::string const& depth_map_scene_file,
             volm_spherical_shell_container_sptr const& sph_shell,
             volm_spherical_container_sptr const& sph);

  //: constructor from a binary file of data members
  volm_query(std::string const& query_file, const volm_camera_space_sptr& cam_space,
             std::string const& depth_map_scene_file,
             volm_spherical_shell_container_sptr const& sph_shell,
             volm_spherical_container_sptr const& sph);


  // === accessors ===
  std::vector<std::vector<unsigned char> >& min_dist()                         { return min_dist_;}
  std::vector<std::vector<unsigned char> >& max_dist()                         { return max_dist_;}
  std::vector<std::vector<unsigned char> >& order()                            { return order_; }
  std::set<unsigned>& order_set()                                             { return order_set_; }
  std::vector<std::vector<std::vector<unsigned> > >& order_index()              { return order_index_; }
  std::vector<std::vector<std::vector<unsigned> > >& dist_id()                  { return dist_id_; }
  std::vector<unsigned>& dist_offset()                                        { return dist_offset_; }
  std::vector<unsigned char>& max_obj_dist()                                  { return max_obj_dist_; }
  std::vector<unsigned char>& min_obj_dist()                                  { return min_obj_dist_; }
  std::vector<unsigned char>& obj_orient()                                    { return obj_orient_; }
  std::vector<std::vector<unsigned char> >& obj_land_id()                      { return obj_land_id_; }
  std::vector<std::vector<float> >& obj_land_wgt()                             { return obj_land_wgt_; }
  std::vector<unsigned char>& order_obj()                                     { return order_obj_; }
  std::vector<std::vector<unsigned> >& ground_id()                             { return ground_id_; }
  std::vector<std::vector<unsigned char> >& ground_dist()                      { return ground_dist_; }
  std::vector<unsigned>& ground_offset()                                      { return ground_offset_; }
  std::vector<std::vector<std::vector<unsigned char> > >& ground_land_id()      { return ground_land_id_; }
  std::vector<std::vector<std::vector<float> > >& ground_land_wgt()             { return ground_land_wgt_; }
  unsigned char ground_orient()                                              { return ground_orient_; }
  std::vector<std::vector<unsigned> >& sky_id()                                { return sky_id_; }
  std::vector<unsigned>& sky_offset()                                         { return sky_offset_; }
  unsigned char sky_orient()                                                 { return sky_orient_; }
  depth_map_scene_sptr depth_scene() const                                   { return dm_; }
  std::vector<depth_map_region_sptr>& depth_regions()                         { return depth_regions_; }
  volm_spherical_shell_container_sptr sph_shell() const                      { return sph_; }
  unsigned get_cam_num() const                                               { return (unsigned)cam_space_->valid_indices().size(); }
  unsigned get_obj_order_num() const                                         { return (unsigned)order_index_[0].size(); }
  unsigned get_query_size() const                                            { return query_size_; }

  //: return number of voxels having ground properties
  unsigned get_ground_id_size() const                           { return ground_offset_[ground_offset_.size()-1]; }

  //: return stored distance for all ground voxels
  unsigned get_ground_dist_size() const                         { return ground_offset_[ground_offset_.size()-1]; }

  //: return number of voxels having non-ground, non-sky properties
  unsigned get_dist_id_size() const                             { return dist_offset_[dist_offset_.size()-1]; }

  //: return number of voxels having sky properties
  unsigned get_sky_id_size() const                              { return sky_offset_[sky_offset_.size()-1]; }

  //: return number of voxels for all non-ground objects (order_index)
  unsigned get_order_size() const;

  //: return the total query size in byte(object based)
  unsigned obj_based_query_size_byte() const;

  //: write vrml for spherical container and camera hypothesis
  void draw_template(std::string const& vrml_fname);

  //: write query image showing the depth map geometry and the penetrating ray
  void draw_query_images(std::string const& out_dir);
  void draw_query_image(unsigned i, std::string const& out_name);

  //: get camera string
  std::string get_cam_string(unsigned i) const
  {
    return cam_space_->camera_angles(i).get_string();
    //return cam_space_->get_string(cam_space_->valid_indices()[i]);
  }

  //: get the number of camera having the input top_fov value
  unsigned get_num_top_fov(double const& top_fov) const;

  //: extract the top_fov value from cam_id
  double get_top_fov(unsigned const& i) const;

  //: return valid top_fov from camera vector
  std::vector<double> get_valid_top_fov() const;

  //: visualized the query camera using the spherical shell geometry
  void visualize_query(std::string const& prefix);

  //: generate rgb depth image for given camera id and given depth value
  void depth_rgb_image(std::vector<unsigned char> const& values, unsigned const& cam_id, vil_image_view<vil_rgb<vxl_byte> >& out_img, const std::string& value_type = "depth");

  //: draw the polygons of regions on top of an rgb image
  void draw_depth_map_regions(vil_image_view<vil_rgb<vxl_byte> >& out_img);
  void draw_query_regions(std::string const& out_name);

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write  -- does not write all of self, not a regular b_write
  void write_data(vsl_b_ostream& os);

  //: binary IO read   -- does not read all of self, not a regular b_read
  void read_data(vsl_b_istream& is);

  //: CAUTION: not all fields are checked for equality, only the fields which are saved in write_data() are checked
  bool operator == (const volm_query &other) const;

  static void draw_polygon(vil_image_view<vil_rgb<vxl_byte> >& img, vgl_polygon<double> const& poly, unsigned char const& depth);

#if NO_CAM_SPACE
  //: initial camera parameters read from camera kml
  double init_focal_;
  double head_, head_d_, head_inc_;
  double tilt_, tilt_d_, tilt_inc_;
  double roll_, roll_d_, roll_inc_;
  double tfov_, tfov_d_, tfov_inc_;
#endif
  double altitude_;

 protected:
#if NO_CAM_SPACE_CLASS
  //: a check whether use the viewing volume values provided by camera kml
  bool use_default_;
#endif
  volm_camera_space_sptr cam_space_;

  //:
  unsigned char invalid_;
  //: image size
  unsigned ni_, nj_;
  unsigned log_downsample_ratio_;  // 0,1,2 or 3 (ni-->ni/2^ratio_), to generate downsampled depth maps for ground regions

  //: depth map scene
  depth_map_scene_sptr                           dm_;
  //: voxel array used to get voxel index
  volm_spherical_container_sptr           sph_depth_;
  //: a unit sphere
  volm_spherical_shell_container_sptr           sph_;
  //: upper bound on depth
  double d_threshold_;
  //: vector of depth_map_region sorted by depth order
  std::vector<depth_map_region_sptr>   depth_regions_;

  // === camera parameters --- use even number later to ensure the init_value and init_value +/- conf_value is covered ===

#if 0
  //: vectors store the space of camera hypotheses
  std::vector<double>  top_fov_;
  std::vector<double> headings_;
  std::vector<double>    tilts_;
  std::vector<double>    rolls_;
#endif
  std::vector<vpgl_perspective_camera<double> > cameras_;
  std::vector<std::string> camera_strings_;
  //: ingested query information
  std::vector<std::vector<unsigned char> > min_dist_;
  std::vector<std::vector<unsigned char> > max_dist_;
  std::vector<std::vector<unsigned char> >    order_;
  //: number of spherical shell rays
  unsigned query_size_;
  //: the order index assigned to sky
  unsigned order_sky_;
  //: the set of Cartesian points on the unit sphere
  std::vector<vgl_point_3d<double> > query_points_;
  //: order vector to store the index id associated with object order
  std::set<unsigned> order_set_;  // store the non-ground order, using set to ensure objects having same order are put together
  std::vector<std::vector<std::vector<unsigned> > > order_index_;
  //: ground plane distance, id, and fallback land category
  std::vector<std::vector<unsigned> >                             ground_id_;
  std::vector<std::vector<unsigned char> >                      ground_dist_;
  std::vector<std::vector<std::vector<unsigned char> > >      ground_land_id_;
  std::vector<std::vector<std::vector<float> > >             ground_land_wgt_;
  std::vector<unsigned>                                      ground_offset_;
  unsigned char                                             ground_orient_;  // always horizontal
  //: sky distance
  std::vector<std::vector<unsigned> >                                sky_id_;
  std::vector<unsigned>                                         sky_offset_;
  unsigned char                                                sky_orient_;  // always 100 (100 is the label for uncertain or ambiguous cells)
  //: object id based on min_dist (since objects may have different min_dist but same order)
  std::vector<std::vector<std::vector<unsigned> > >                  dist_id_;
  std::vector<unsigned>                                        dist_offset_;
  //: min and max distance, object orders, orientation and land clarifications for different objects, based on object orders
  std::vector<unsigned char>                                  min_obj_dist_;
  std::vector<unsigned char>                                  max_obj_dist_;
  std::vector<unsigned char>                                     order_obj_;
  std::vector<unsigned char>                                    obj_orient_;
  std::vector<std::vector<unsigned char> >                      obj_land_id_;
  std::vector<std::vector<float> >                             obj_land_wgt_;
#if 0
  //: weight parameters
  std::vector<float> weight_obj_;
  float             weight_grd_;
  float             weight_sky_;
#endif
  //: functions
  bool query_ingest();
  bool offset_ingest();
  bool order_ingest();
  bool weight_ingest();
  unsigned char fetch_depth(double const& u,
                            double const& v,
                            unsigned char& order,
                            unsigned char& max_dist,
                            unsigned& object_id,
                            std::vector<unsigned char>& grd_fallback_id,
                            std::vector<float>& grd_fallback_wgt,
                            bool& is_ground,
                            bool& is_sky,
                            bool& is_object,
                            vil_image_view<float> const& depth_img);
  void create_cameras();
  void generate_regions();
  void draw_viewing_volume(std::string const& fname,
                           vpgl_perspective_camera<double> cam,
                           float r,
                           float g,
                           float b);
  void draw_rays(std::string const& fname);

  void draw_dot(vil_image_view<vil_rgb<vxl_byte> >& img,
                vgl_point_3d<double> const& world_point,
                vil_rgb<vxl_byte>,
                vpgl_perspective_camera<double> const& cam);
};

#endif  // volm_query_h_
