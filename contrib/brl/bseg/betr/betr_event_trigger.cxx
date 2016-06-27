#include <vcl_compiler.h>
#include "betr_event_trigger.h"
#include "betr_geo_object_3d.h"
#include "vsol_mesh_3d.h"
#include <vsol/vsol_region_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol_polygon_2d.h>
#include <vsol_polygon_3d.h>
#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
void betr_event_trigger::update_local_bounding_box(){
  if(global_bbox_.is_empty())
    return;
  if(!lvcs_valid_){
    std::cout << "lvcs for trigger region not defined - fatal\n";
    return;
  }
  if(!local_bbox_)
    local_bbox_ = new vsol_box_3d();
  // global geographic coordinates (lon, lat, elev)
  vgl_point_3d<double> min_pt = global_bbox_.min_point();
  vgl_point_3d<double> max_pt = global_bbox_.max_point();
  // convert to local Cartesian coordinates
  double lx=0.0, ly=0.0, lz=0.0;
  lvcs_.global_to_local(min_pt.x(), min_pt.y(), min_pt.z(), vpgl_lvcs::wgs84,
                        lx, ly, lz);
  local_bbox_->add_point(lx, ly, lz);

  lvcs_.global_to_local(max_pt.x(), max_pt.y(), max_pt.z(), vpgl_lvcs::wgs84,
                        lx, ly, lz);
  local_bbox_->add_point(lx, ly, lz);
}
//The object in general will have a different lvcs from the event trigger origin
//but assume that X and Y translations are just given in the tangent plane
// That is tx = dlon_rads*Rearth, ty = dlat_rads*Rearth, tz is the elevation difference
void betr_event_trigger::add_geo_object(std::string const& obj_name, betr_geo_object_3d_sptr const& geo_object){
  betr_geo_box_3d box = geo_object->bounding_box();
  global_bbox_.add(box);
  this->update_local_bounding_box();
  double Rearth = 6378135.0;//meters
  //assume local transform is identity and lvcs is in degrees and meters
  vpgl_lvcs obj_lvcs = geo_object->lvcs();
  double obj_lat = 0.0, obj_lon = 0.0, obj_elev =0.0;
  obj_lvcs.get_origin(obj_lat, obj_lon, obj_elev);
  double trig_lat = 0.0, trig_lon = 0.0, trig_elev =0.0;
  lvcs_.get_origin(trig_lat, trig_lon, trig_elev);
  // translation to shift obj coordinates to trigger coordinates
  double tx = (obj_lat - trig_lat)*Rearth;
  double ty = (obj_lon - trig_lon)*Rearth;
  double tz = obj_elev - trig_elev;
  vgl_vector_3d<double> transl(tx, ty, tz);
  trigger_objects_[obj_name]=geo_object;
  local_trans_[obj_name]=transl;
}
bool betr_event_trigger::project_object(std::string const& obj_name, vsol_polygon_2d_sptr& poly_2d){
  //for now only handle the case where the object is a 3-d polygon
  betr_geo_object_3d_sptr obj = trigger_objects_[obj_name];
  if(!obj){
    std::cout << "no geo object with name " << obj_name << '\n';
    return false;
  }
  vgl_vector_3d<double> transl = local_trans_[obj_name];
  vsol_spatial_object_3d_sptr so_ptr = obj->obj();
  if(!so_ptr){
   std::cout << "null spatial object on  " << obj_name << '\n';
    return false;
  } 
  vsol_region_3d* reg_ptr = VXL_NULLPTR;
  vsol_volume_3d* vol_ptr = VXL_NULLPTR;
  if(reg_ptr = so_ptr->cast_to_region())
    {
      vsol_polygon_3d* poly_3d = reg_ptr->cast_to_polygon();
      if(!poly_3d){
        std::cout << "only handle polygonal regions for now " << obj_name << " is not a vsol_polygon_3d\n";
        return false;
      } 
      poly_2d = project_poly(poly_3d, transl);
      return true;
    }else if(vol_ptr = so_ptr->cast_to_volume()){
      vsol_mesh_3d* mesh_3d = vol_ptr->cast_to_mesh();
      if(!mesh_3d){
        std::cout << "only handle vsol_mesh_3d for now " << obj_name << " is not a vsol_mesh_3d\n";
        return false;
      } 
      std::vector<vsol_point_3d_sptr> verts = mesh_3d->vertices();
      std::vector<vgl_point_2d<double> > pts_2d;
      for(std::vector<vsol_point_3d_sptr>::iterator vit = verts.begin();
          vit != verts.end(); ++vit){
        double u = 0, v = 0;
        lcam_.project((*vit)->x()+transl.x(),
                      (*vit)->y()+transl.y(),
                      (*vit)->z()+transl.z(), u, v);
        pts_2d.push_back(vgl_point_2d<double>(u, v));
      }
      vgl_convex_hull_2d<double> conv_hull(pts_2d);
      vgl_polygon<double> h = conv_hull.hull();
      std::vector<vsol_point_2d_sptr> sverts;
      std::vector<vgl_point_2d<double> > hverts = h[0];
      for(std::vector<vgl_point_2d<double> >::iterator vit = hverts.begin();
          vit != hverts.end(); ++vit)
        sverts.push_back(new vsol_point_2d(*vit));
      poly_2d = new vsol_polygon_2d(sverts);
      return true;
  }
 return false;
}
vsol_polygon_2d_sptr  betr_event_trigger::project_poly(vsol_polygon_3d_sptr poly_3d,
                                                       vgl_vector_3d<double> const& transl){
 std::vector<vsol_point_2d_sptr> vertices;
  for (unsigned i=0; i<poly_3d->size(); i++) {
    double u = 0,v = 0;
#ifdef DEBUG
    std::cout << "3d point " << *(poly_3d->vertex(i)) << std::endl;
#endif
    vgl_point_3d<double> p(poly_3d->vertex(i)->x(),
                           poly_3d->vertex(i)->y(),
                           poly_3d->vertex(i)->z());
    p += transl;
    lcam_.project(p.x(), p.y(), p.z(), u,v);
    vsol_point_2d_sptr vp = new vsol_point_2d(u,v);
    vertices.push_back(vp);
  }
  vsol_polygon_2d_sptr poly_2d = new vsol_polygon_2d (vertices);
  return poly_2d;
}

//: Binary write boxm2_event_trigger to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger const& /*bit_event_trigger*/) {}
//: Binary write betr_event_trigger pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger* const& /*ph*/) {}
//: Binary write betr_event_trigger smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger_sptr&) {}
//: Binary write betr_event_trigger smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger_sptr const&) {}

//: Binary load boxm event_trigger from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger& /*bit_event_trigger*/) {}
//: Binary load boxm event_trigger pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger* /*ph*/) {}
//: Binary load boxm event_trigger smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger_sptr&) {}
//: Binary load boxm event_trigger smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger_sptr const&) {}
