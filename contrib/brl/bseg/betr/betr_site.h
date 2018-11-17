#ifndef betr_site_h_
#define betr_site_h_
//:
// \file
// \brief betr_site provides a collection of event objects and change detectors
// \author J.L. Mundy
// \date May 14, 2016
//
//  the coordinates are based on the standard WGS84 spheroidal reference surface
//  each event object has a local vertical coordinate system
//
#include <string>
#include <vector>
#include <map>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d.h"
#include "betr_event_trigger.h"
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vpgl/vpgl_camera.h>
class betr_site : public vbl_ref_count{
 public:
  betr_site();
  betr_site(vgl_point_3d<double> const& origin_wgs_deg_m){
    origin_ = origin_wgs_deg_m;
  }
  bool add_event_trigger(const betr_event_trigger_sptr&);
  bool add_geo_object(std::string name, double lon, double lat , double elev, const std::string& geom_path);
  void add_geo_object(const std::string& name, vgl_point_3d<double> const& location_wgs_deg_m, vsol_spatial_object_3d_sptr const& obj);
 private:
  vgl_point_3d<double> origin_;
  betr_geo_box_3d bbox_;
  std::map<std::string, betr_event_trigger_sptr> event_triggers_;
  std::map<std::string, betr_geo_object_3d_sptr> site_objects_;
  };
#endif //guard
#include "betr_site_sptr.h"
  //: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, betr_site const& site);
void vsl_b_write(vsl_b_ostream& os, const betr_site* &p);
void vsl_b_write(vsl_b_ostream& os, betr_site_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, betr_site_sptr const& sptr);

//: Binary load betr_site site from stream.
void vsl_b_read(vsl_b_istream& is, betr_site &site);
void vsl_b_read(vsl_b_istream& is, betr_site* p);
void vsl_b_read(vsl_b_istream& is, betr_site_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, betr_site_sptr const& sptr);
