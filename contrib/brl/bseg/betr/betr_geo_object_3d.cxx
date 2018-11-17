#include "betr_geo_object_3d.h"
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_region_3d.h>
#include <vsol/vsol_volume_3d.h>
#include <betr/vsol_mesh_3d.h>

  void betr_geo_object_3d::update_box_from_from_vsol_box(const vsol_box_3d_sptr& sbox, betr_geo_box_3d& box) const{
  if(!sbox)
    return;
  // derive four points from the box (three is minimum for geo box)
  double xmin = sbox->get_min_x(), xmax = sbox->get_max_x();
  double ymin = sbox->get_min_y(), ymax = sbox->get_max_y();
  double zmin = sbox->get_min_z(), zmax = sbox->get_max_z();
  double width = sbox->width(), height = sbox->height(), depth = sbox->depth();
  double xc = xmin + 0.5*width, yc = ymin+0.5*height, zc = zmin+0.5*depth;
  auto* gobj_nconst = const_cast<betr_geo_object_3d*>(this);
  // convert to geographic coordinates using the lvcs
  double lon = 0.0, lat = 0.0, elev = 0.0;
  (gobj_nconst->lvcs_).local_to_global(xmin, ymin, zmin, vpgl_lvcs::wgs84,
                        lon, lat, elev);
  box.add(lon, lat, elev);
  (gobj_nconst->lvcs_).local_to_global(xmax, ymax, zmax, vpgl_lvcs::wgs84,
                        lon, lat, elev);
  box.add(lon, lat, elev);

  (gobj_nconst->lvcs_).local_to_global(xc, yc, zc, vpgl_lvcs::wgs84,
                        lon, lat, elev);
  box.add(lon, lat, elev);
}
vsol_polygon_3d_sptr betr_geo_object_3d::base_polygon() {
  vsol_polygon_3d_sptr local_base_poly;
  vsol_region_3d* reg_ptr = nullptr;
  vsol_volume_3d* vol_ptr = nullptr;
  if( ( reg_ptr = so_->cast_to_region() ) ){
    vsol_polygon_3d* poly_3d = reg_ptr->cast_to_polygon();
      if(!poly_3d){
        std::cout << "only handle polygonal regions for now. Geo object is not a vsol_polygon_3d" << std::endl;
        return nullptr;
      }
      local_base_poly = poly_3d;
  }else if( ( vol_ptr = so_->cast_to_volume() ) ){
    vsol_mesh_3d* mesh = vol_ptr->cast_to_mesh();
    if(!mesh){
        std::cout << "only handle mesh volumes for now. Geo object is not a vsol_mesh_3d" << std::endl;
        return nullptr;
      }
    local_base_poly =  mesh->extract_bottom_face();
  }else{
    std::cout << "In base_polygon - spatial object is unknown " << std::endl;
    return nullptr;
  }
  // convert coordinates to global
  std::vector<vsol_point_3d_sptr> gverts;
  unsigned n = local_base_poly->size();
  for(unsigned i = 0; i<n; ++i){
    vsol_point_3d_sptr v = local_base_poly->vertex(i);
    double lon, lat, elev;
    lvcs_.local_to_global(v->x(), v->y(), v->z(), vpgl_lvcs::wgs84, lon, lat, elev);
    vsol_point_3d_sptr gv = new vsol_point_3d(lon, lat, elev);
    gverts.push_back(gv);
  }
  return new vsol_polygon_3d(gverts);
}
