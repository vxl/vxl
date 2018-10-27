#include "volm_osm_object_polygon.h"
//:
// \file

void volm_osm_object_polygon::print()
{
  std::cout << " object property: ";
  prop_.print();
  std::cout << " polygon: ";
  poly_.print(std::cout);
}

void volm_osm_object_polygon::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, prop_.id_);
  vsl_b_write(os, prop_.name_);
  vsl_b_write(os, prop_.level_);
  vsl_b_write(os, prop_.width_);
  vsl_b_write(os, poly_.num_sheets());
  for (unsigned i = 0; i < poly_.num_sheets(); i++) {
    unsigned n_vert = poly_[i].size();
    vsl_b_write(os, n_vert);
    for (unsigned j = 0; j < n_vert; j++) {
      vsl_b_write(os, poly_[i][j].x());
      vsl_b_write(os, poly_[i][j].y());
    }
  }
}

void volm_osm_object_polygon::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    unsigned char id, level;
    std::string name;
    double width;
    unsigned n_sheet;
    vsl_b_read(is, id);
    vsl_b_read(is, name);
    vsl_b_read(is, level);
    vsl_b_read(is, width);
    vsl_b_read(is, n_sheet);
    for (unsigned i = 0; i < n_sheet; i++) {
      poly_.new_sheet();
      unsigned n_vert;
      vsl_b_read(is, n_vert);
      for (unsigned j = 0; j < n_vert; j++) {
        double x, y;
        vsl_b_read(is, x);
        vsl_b_read(is, y);
        poly_.push_back(vgl_point_2d<double>(x, y));
      }
    }
    if (name.compare("invalid") == 0)
      prop_ = volm_land_layer(id, name, level, width, vil_rgb<vxl_byte>(0,0,0));
    else
      prop_ = volm_land_layer(id, name, level, width);
  }
  else {
    std::cout << "volm_osm_object_polygon -- unknown binary io version " << ver << '\n';
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_polygon* rptr)
{
  if (rptr == nullptr) {
    vsl_b_write(os, false);
    return;
  }
  else
    vsl_b_write(os, true);
  auto* ptr = const_cast<volm_osm_object_polygon*>(rptr);
  ptr->b_write(os);
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_polygon*& rptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    rptr = new volm_osm_object_polygon();
    rptr->b_read(is);
    return;
  }
  rptr = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_polygon_sptr& sptr)
{
  volm_osm_object_polygon* rptr = sptr.ptr();
  vsl_b_write(os, rptr);
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_polygon_sptr& sptr)
{
  volm_osm_object_polygon* rptr = nullptr;
  vsl_b_read(is, rptr);
  sptr = rptr;
}
