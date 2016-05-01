#include "betr_geo_box_3d.h"

void betr_geo_box_3d::add(double lon, double lat, double el){
  if(this->is_empty()){
    min_elv_ = el;
    max_elv_ = el;
    sph_box_.add(lat, lon, false); // false == degrees
    return;
  }
  if(el<min_elv_)
    min_elv_ = el;
  if(el>max_elv_)
    max_elv_ = el;
  sph_box_.add(lat, lon, false);// false == degrees
}


void betr_geo_box_3d::add(vgl_point_3d<double> const& geo_pt){
  double lon = geo_pt.x(), lat = geo_pt.y(), el = geo_pt.z();
  this->add(lon, lat, el);
}


bool betr_geo_box_3d::contains(vgl_point_3d<double> const& geo_pt) const{
  double lon = geo_pt.x(), lat = geo_pt.y(), el = geo_pt.z();
  return this->contains(lon, lat, el);
}

bool betr_geo_box_3d::contains(double lon, double lat, double el) const{
  if(this->is_empty())
    return false;
  bool el_inside = el>=min_elv_ && el<=max_elv_;
  bool lat_lon_inside = sph_box_.contains(lat, lon, false);
  return el_inside && lat_lon_inside;
}
