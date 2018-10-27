#include "volm_osm_object_point.h"
//:
// \file

void volm_osm_object_point::print()
{
  std::cout << " object property ";
  prop_.print();
  std::cout << " loc: " << loc_ << std::endl;
}

void volm_osm_object_point::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, prop_.id_);
  vsl_b_write(os, prop_.name_);
  vsl_b_write(os, prop_.level_);
  vsl_b_write(os, prop_.width_);
  vsl_b_write(os, loc_.x());
  vsl_b_write(os, loc_.y());
}

void volm_osm_object_point::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    unsigned char id, level;
    std::string name;
    double width, x, y;
    vsl_b_read(is, id);
    vsl_b_read(is, name);
    vsl_b_read(is, level);
    vsl_b_read(is, width);
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    if (name.compare("invalid") == 0)
      prop_ = volm_land_layer(id, name, level, width, vil_rgb<vxl_byte>(0,0,0));
    else
      prop_ = volm_land_layer(id, name, level, width);
    loc_ = vgl_point_2d<double>(x,y);
  }
  else {
    std::cout << "volm_osm_object_point -- unknown binary io version " << ver << '\n';
    return;
  }
}


void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_point* rptr)
{
  if (rptr == nullptr) {
    vsl_b_write(os, false);
    return;
  }
  else
    vsl_b_write(os, true);
  auto* ptr = const_cast<volm_osm_object_point*>(rptr);
  ptr->b_write(os);
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_point*& rptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    rptr = new volm_osm_object_point();
    rptr->b_read(is);
    return;
  }
  rptr = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_point_sptr& sptr)
{
  volm_osm_object_point* rptr = sptr.ptr();
  vsl_b_write(os, rptr);
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_point_sptr& sptr)
{
  volm_osm_object_point* rptr = nullptr;
  vsl_b_read(is, rptr);
  sptr = rptr;
}
