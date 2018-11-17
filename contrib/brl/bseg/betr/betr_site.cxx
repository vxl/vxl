#include "betr_site.h"
#include "betr_geo_object_3d.h"
#include "betr_event_trigger.h"
#include "vsol_mesh_3d.h"
#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>

#include <utility>
betr_site::betr_site()= default;

bool betr_site::add_geo_object(std::string name, double lon, double lat , double elev, const std::string& geom_path){
  bmsh3d_mesh* mesh = new bmsh3d_mesh_mc();
  bool good = bmsh3d_load_ply(mesh, geom_path.c_str());
  if(!good){
    std::cout << "invalid bmesh3d ply file - " << geom_path << '\n';
    return false;
  }
  auto* vmesh = new vsol_mesh_3d();
  auto* mesh_mc = dynamic_cast< bmsh3d_mesh_mc*>(mesh);
  vmesh->set_mesh(mesh_mc);
  vsol_spatial_object_3d_sptr so = vmesh;
  vgl_point_3d<double> geo_loc(lon, lat, elev);
  this->add_geo_object(std::move(name), geo_loc, so);
  return good;
}
void betr_site::add_geo_object(const std::string& name, vgl_point_3d<double> const& geo_location_deg_m, vsol_spatial_object_3d_sptr const& obj){
  vpgl_lvcs lvcs(geo_location_deg_m.y(), geo_location_deg_m.x(), geo_location_deg_m.z(), vpgl_lvcs::wgs84, vpgl_lvcs::DEG);
  betr_geo_object_3d_sptr go = new betr_geo_object_3d(obj, lvcs);
  site_objects_[name]=go;
  // add to site bounding box
  // get min and max geo points
  betr_geo_box_3d obox = go->bounding_box();
  vgl_point_3d<double> min_pt = obox.min_point();
  vgl_point_3d<double> max_pt = obox.max_point();
  vgl_point_3d<double> aux_pt = obox.aux_point();
  bbox_.add(min_pt);   bbox_.add(max_pt);   bbox_.add(aux_pt);
}
bool betr_site::add_event_trigger(const betr_event_trigger_sptr& etr){
  if(!etr){
    std::cout << "null event trigger \n";
    return false;
  }
  event_triggers_[etr->name()] = etr;
  const std::map<std::string, betr_geo_object_3d_sptr>& ref_objs = etr->ref_objects();
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objs = etr->evt_objects();;
  auto roit  = ref_objs.begin();
  for(; roit != ref_objs.end(); ++roit){
    site_objects_[roit->first] =  roit->second;
    betr_geo_box_3d obox = (roit->second)->bounding_box();
    vgl_point_3d<double> min_pt = obox.min_point();
    vgl_point_3d<double> max_pt = obox.max_point();
    vgl_point_3d<double> aux_pt = obox.aux_point();
    bbox_.add(min_pt);   bbox_.add(max_pt);   bbox_.add(aux_pt);
  }
  auto eoit  = evt_objs.begin();
  for(; eoit != ref_objs.end(); ++eoit){
    site_objects_[eoit->first] =  eoit->second;
    betr_geo_box_3d obox = (eoit->second)->bounding_box();
    vgl_point_3d<double> min_pt = obox.min_point();
    vgl_point_3d<double> max_pt = obox.max_point();
    vgl_point_3d<double> aux_pt = obox.aux_point();
    bbox_.add(min_pt);   bbox_.add(max_pt);   bbox_.add(aux_pt);
  }
  return true;
}
  //: Binary write boxm2_site to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_site const& /*bit_site*/) {}
//: Binary write betr_site pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_site* const& /*ph*/) {}
//: Binary write betr_site smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_site_sptr&) {}
//: Binary write betr_site smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_site_sptr const&) {}

//: Binary load boxm site from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_site& /*bit_site*/) {}
//: Binary load boxm site pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_site* /*ph*/) {}
//: Binary load boxm site smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_site_sptr&) {}
//: Binary load boxm site smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_site_sptr const&) {}
