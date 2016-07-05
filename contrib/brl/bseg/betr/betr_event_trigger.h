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
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d.h"
#include "betr_event_trigger_sptr.h"
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
class betr_event_trigger : public vbl_ref_count{
 public:
 betr_event_trigger():lvcs_valid_(false), local_bbox_(VXL_NULLPTR){}

 betr_event_trigger(vpgl_lvcs const& lvcs): lvcs_(lvcs), lvcs_valid_(true), local_bbox_(VXL_NULLPTR){
  }
 betr_event_trigger(vpgl_lvcs const& lvcs, vpgl_local_rational_camera<double> const& lcam):
  lvcs_(lvcs), lvcs_valid_(true), lcam_(lcam), local_bbox_(VXL_NULLPTR){
  }
  void set_lvcs(vpgl_lvcs const& lvcs){lvcs_=lvcs; lvcs_valid_=true;}
  void set_local_rat_cam(vpgl_local_rational_camera<double> const& lcam){lcam_ = lcam;}
  // Add object and find translation vector from trigger origin to object origin.
  void add_geo_object(std::string const& obj_name, betr_geo_object_3d_sptr const& geo_object);
  //: projected 2-d polygon for the 3-d trigger object
  bool project_object(std::string const& obj_name, vsol_polygon_2d_sptr& poly);
 private:
  void update_local_bounding_box();
  vsol_polygon_2d_sptr project_poly(vsol_polygon_3d_sptr poly_3d,
                                    vgl_vector_3d<double> const& transl);
  vpgl_lvcs lvcs_;
  bool lvcs_valid_;
  betr_geo_box_3d global_bbox_;//trigger bounding box in global WGS84
  vsol_box_3d_sptr local_bbox_;//trigger bounding box in local Cartesian coordinates
  vpgl_local_rational_camera<double> lcam_;//camera for entire trigger region
  std::map<std::string, betr_geo_object_3d_sptr> trigger_objects_;
  std::map<std::string, vgl_vector_3d<double> > local_trans_;//translation to each object
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
