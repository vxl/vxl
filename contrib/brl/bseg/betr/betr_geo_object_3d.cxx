#include "betr_geo_object_3d.h"
#include <vsol/vsol_box_3d.h>

void betr_geo_object_3d::update_box_from_from_vsol_box(vsol_box_3d_sptr sbox, betr_geo_box_3d& box) const{
  if(!sbox)
    return;
  // derive four points from the box (three is minimum for geo box)
  double xmin = sbox->get_min_x(), xmax = sbox->get_max_x();
  double ymin = sbox->get_min_y(), ymax = sbox->get_max_y();
  double zmin = sbox->get_min_z(), zmax = sbox->get_max_z();
  double width = sbox->width(), height = sbox->height(), depth = sbox->depth();
  double xc = xmin + 0.5*width, yc = ymin+0.5*height, zc = zmin+0.5*depth;
  betr_geo_object_3d* gobj_nconst = const_cast<betr_geo_object_3d*>(this);
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
#if 0
betr_geo_box_3d betr_geo_object_3d::bounding_box() const{
  betr_geo_box_3d box;
  if(!so_)
    return box;//empty box
  this->update_box_from_vsol_box(so_->get_bounding_box(), box);
  return box;
}
#endif
