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
#include <vector>
#include <map>
#include <vcl_compiler.h>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource_sptr.h>
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
    local_bbox_(VXL_NULLPTR), verbose_(false){register_algorithms();}

  betr_event_trigger(vpgl_lvcs const& lvcs): name_("no_name"), lvcs_(lvcs), lvcs_valid_(true),
    local_bbox_(VXL_NULLPTR), verbose_(false){
    register_algorithms();
  }
  betr_event_trigger(std::string const& name, vpgl_lvcs const& lvcs): name_(name), lvcs_(lvcs), lvcs_valid_(true),
    local_bbox_(VXL_NULLPTR), verbose_(false){
    register_algorithms();
  }
  void set_lvcs(vpgl_lvcs const& lvcs){lvcs_=lvcs; lvcs_valid_=true;}
  void set_ref_image(vil_image_resource_sptr ref_imgr){ref_imgr_ = ref_imgr;}
  void set_evt_image(vil_image_resource_sptr evt_imgr){evt_imgr_ = evt_imgr;}
  void set_ref_camera(vpgl_camera_double_sptr const& camera){ref_camera_ = camera;}
  void set_evt_camera(vpgl_camera_double_sptr const& camera){evt_camera_ = camera;}

  // Add object and find translation vector from trigger origin to object origin.
  void add_geo_object(std::string const& obj_name, betr_geo_object_3d_sptr const& geo_object, bool is_ref_obj);
  bool add_geo_object(std::string const& name, double lon, double lat ,
                      double elev, std::string const& geom_path, bool is_ref_obj);
  //: execute change algorithm
  bool process(std::string alg_name, double& prob_change);

  //: acessors
  std::string name() const {return name_;}
  const std::map<std::string, betr_geo_object_3d_sptr>& ref_objects() const {return ref_trigger_objects_;}
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objects() const {return evt_trigger_objects_;}
  vpgl_lvcs lvcs() const {return lvcs_;}
  std::vector<std::string> algorithms() const;

  //utilities
  //: projected 2-d polygon for the 3-d trigger object
  bool project_object(vpgl_camera_double_sptr cam, std::string const& obj_name, vsol_polygon_2d_sptr& poly);
  bool save_projected_polys(std::string const& path, std::vector<vsol_polygon_2d_sptr> const& polys);  
  // debug
  void set_verbose(bool verbose){verbose_ = verbose;}
 private:
  void update_local_bounding_box();
  vsol_polygon_2d_sptr project_poly(vpgl_camera_double_sptr const& camera,
                                    vsol_polygon_3d_sptr poly_3d,
                                    vgl_vector_3d<double> const& transl);
  void register_algorithms();
  bool verbose_;
  std::string name_;
  vpgl_lvcs lvcs_;
  bool lvcs_valid_;
  betr_geo_box_3d global_bbox_;//trigger bounding box in global WGS84
  vsol_box_3d_sptr local_bbox_;//trigger bounding box in local Cartesian coordinates
  vil_image_resource_sptr ref_imgr_; //ref image resouce
  vil_image_resource_sptr evt_imgr_; //event image resouce
  vpgl_camera_double_sptr ref_camera_;// ref image camera for entire trigger region
  vpgl_camera_double_sptr evt_camera_;// evt image camera for entire trigger region
  std::map<std::string, betr_geo_object_3d_sptr> evt_trigger_objects_;
  std::map<std::string, betr_geo_object_3d_sptr> ref_trigger_objects_;
  std::map<std::string, vgl_vector_3d<double> > local_trans_;//translation to each object
  std::map<std::string, betr_algorithm_sptr> algorithms_;//available change algorithms
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
