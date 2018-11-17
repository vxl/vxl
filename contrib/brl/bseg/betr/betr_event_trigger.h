#ifndef betr_event_trigger_h_
#define betr_event_trigger_h_
//:
// \file
// \brief betr_event_trigger defines reference and change geo_object(s) and algorithm information
// \author J.L. Mundy
// \date June 26, 2016
//
//  the coordinates are based on the standard WGS84 spheroidal reference surface
//  each event object has a local vertical coordinate system  as well as the trigger region
//  it is assumed that the trigger region can be described by a single LVCS
#include <string>
#include <utility>
#include <vector>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d.h"
#include "betr_event_trigger_sptr.h"
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include "betr_algorithm.h"
class betr_event_trigger : public vbl_ref_count{
 public:
 betr_event_trigger(): name_("no_name"), lvcs_valid_(false),
    local_bbox_(nullptr), verbose_(false){register_algorithms();}

  betr_event_trigger(vpgl_lvcs const& lvcs): name_("no_name"), lvcs_(lvcs), lvcs_valid_(true),
    local_bbox_(nullptr), verbose_(false){
    register_algorithms();
  }
  betr_event_trigger(std::string  name, vpgl_lvcs const& lvcs): name_(std::move(name)), lvcs_(lvcs), lvcs_valid_(true),
    local_bbox_(nullptr), verbose_(false){
    register_algorithms();
  }
  void set_lvcs(vpgl_lvcs const& lvcs){lvcs_=lvcs; lvcs_valid_=true;}
  // the mask is for the case of RGBA images and it is desired to apply the alpha value
  // by setting the intensity to zero when alpha = 0.
  void set_ref_image(const vil_image_resource_sptr& ref_imgr, bool apply_mask = false, bool keep_data = false);
  void set_ref_images(std::vector<vil_image_resource_sptr> const& ref_rescs, bool apply_mask = false);
  void set_evt_image(const vil_image_resource_sptr& evt_imgr, bool apply_mask = false);

  void set_ref_camera(vpgl_camera_double_sptr const& camera, bool keep_data = false);
  void set_ref_cameras(std::vector<vpgl_camera_double_sptr> const& cameras);
  void set_evt_camera(vpgl_camera_double_sptr const& camera);

  // Add object and find translation vector from trigger origin to object origin.
  void add_geo_object(std::string const& obj_name, betr_geo_object_3d_sptr const& geo_object, bool is_ref_obj);
  bool add_geo_object(std::string const& name, double lon, double lat ,
                      double elev, std::string const& geom_path, bool is_ref_obj);
  bool add_gridded_event_poly(std::string const& name, double lon, double lat ,
                              double elev, std::string const& geom_path, double grid_spacing);
  //: execute change algorithm one event object
  bool process(std::string alg_name, double& prob_change, std::string const& params_json = "{}");

  //: execute change algorithm multiple event objects
  bool process(std::string alg_name, std::vector<double>& prob_change, std::string const& params_json = "{}");

  //: execute change algorithm one event object and change image with offset in event image
  bool process(std::string alg_name, double& prob_change,
               vil_image_resource_sptr change_img, vgl_point_2d<unsigned> offset, std::string const& params_json = "{}");

  //: execute change algorithm multiple event objects with change images and offsets return
  bool process(std::string alg_name, std::vector<double>& prob_change,
               std::vector<vil_image_resource_sptr>& change_images,
               std::vector<vgl_point_2d<unsigned> >& offsets,
               std::string const& params_json = "{}");
  //: execute change algorithm multiple event objects with event names, scores, change images and offsets return
  // note that event objects can be retrieved using the name as a key (see evt_trigger_objects_ below)
  bool process(const std::string& alg_name, std::vector<double>& prob_change,
               std::vector<std::string>& event_region_names,
               std::vector<vil_image_resource_sptr>& change_images,
               std::vector<vgl_point_2d<unsigned> >& offsets,
               std::string const& params_json = "{}");
  //: acessors
  std::string name() const {return name_;}
  const std::map<std::string, betr_geo_object_3d_sptr>& ref_objects() const {return ref_trigger_objects_;}
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objects() const {return evt_trigger_objects_;}
  vpgl_lvcs lvcs() const {return lvcs_;}
  std::vector<std::string> algorithms() const;

  //utilities
  //: projected 2-d polygon for the 3-d trigger object
  bool project_object(const vpgl_camera_double_sptr& cam, std::string const& obj_name, vsol_polygon_2d_sptr& poly);
  bool save_projected_polys(std::string const& path, std::vector<vsol_polygon_2d_sptr> const& polys);
  // debug
  void set_verbose(bool verbose){verbose_ = verbose;}
  void set_ref_path(std::string path){ref_path_ = path;}
  void set_evt_path(std::string path){evt_path_ = path;}
 private:
  void update_local_bounding_box();
  vsol_polygon_2d_sptr project_poly(vpgl_camera_double_sptr const& camera,
                                    const vsol_polygon_3d_sptr& poly_3d,
                                    vgl_vector_3d<double> const& transl);
  void register_algorithms();
  vpgl_camera_double_sptr cast_camera(vpgl_camera_double_sptr const& camera);
  std::string name_;
  vpgl_lvcs lvcs_;
  bool lvcs_valid_;
  static unsigned process_counter_; //unique id for different involcations of an algorithm
  betr_geo_box_3d global_bbox_;//trigger bounding box in global WGS84
  vsol_box_3d_sptr local_bbox_;//trigger bounding box in local Cartesian coordinates
  std::vector<vil_image_resource_sptr> ref_rescs_; //ref image resources
  vil_image_resource_sptr evt_imgr_; //event image resouce
  std::vector<vpgl_camera_double_sptr> ref_cameras_;// ref image camera for entire trigger region
  vpgl_camera_double_sptr evt_camera_;// evt image camera for entire trigger region
  std::string ref_path_;
  std::string evt_path_;
  std::map<std::string, betr_geo_object_3d_sptr> evt_trigger_objects_;
  std::map<std::string, betr_geo_object_3d_sptr> ref_trigger_objects_;
  std::map<std::string, vgl_vector_3d<double> > local_trans_;//translation to each object
  std::map<std::string, betr_algorithm_sptr> algorithms_;//available change algorithms
  bool verbose_;
};
#endif //guard
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, betr_event_trigger const& event_trigger);
void vsl_b_write(vsl_b_ostream& os, const betr_event_trigger* &p);
void vsl_b_write(vsl_b_ostream& os, betr_event_trigger_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, betr_event_trigger_sptr const& sptr);

//: Binary load betr_event_trigger event_trigger from stream.
void vsl_b_read(vsl_b_istream& is, betr_event_trigger &event_trigger);
void vsl_b_read(vsl_b_istream& is, betr_event_trigger* p);
void vsl_b_read(vsl_b_istream& is, betr_event_trigger_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, betr_event_trigger_sptr const& sptr);
