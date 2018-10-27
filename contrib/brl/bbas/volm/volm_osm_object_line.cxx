#include "volm_osm_object_line.h"
//:
// \file

void volm_osm_object_line::print()
{
  std::cout << " object property: ";
  prop_.print();
  std::cout << " line: " << std::endl;
  for (auto & i : line_)
    std::cout << '(' << i.x() << ',' << i.y() << ") ";
  std::cout << '\n';
}

void volm_osm_object_line::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, this->version());
  vsl_b_write(os, prop_.id_);
  vsl_b_write(os, prop_.name_);
  vsl_b_write(os, prop_.level_);
  vsl_b_write(os, prop_.width_);
  vsl_b_write(os, line_.size());
  for (auto & i : line_) {
    vsl_b_write(os, i.x());
    vsl_b_write(os, i.y());
  }
}

void volm_osm_object_line::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is,ver);
  if (ver == 1) {
    unsigned char id, level;
    std::string name;
    unsigned cnt;
    double width;
    vsl_b_read(is, id);
    vsl_b_read(is, name);
    vsl_b_read(is, level);
    vsl_b_read(is, width);
    vsl_b_read(is, cnt);
    for (unsigned i = 0; i < cnt; i++) {
      double x, y;
      vsl_b_read(is, x);
      vsl_b_read(is, y);
      vgl_point_2d<double> pt(x,y);
      line_.push_back(pt);
    }
    if (name.compare("invalid") == 0)
      prop_ = volm_land_layer(id, name, level, width, vil_rgb<vxl_byte>(0,0,0));
    else
      prop_ = volm_land_layer(id, name, level, width);
  }
  else {
    std::cout << "volm_osm_object_line -- unknown binary io version " << ver << '\n';
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_line* rptr)
{
  if (rptr == nullptr) {
    vsl_b_write(os, false);
    return;
  }
  else
    vsl_b_write(os, true);
  auto* ptr = const_cast<volm_osm_object_line*>(rptr);
  ptr->b_write(os);
}

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_line_sptr& sptr)
{
  volm_osm_object_line* rptr = sptr.ptr();
  vsl_b_write(os, rptr);
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_line*& rptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    rptr = new volm_osm_object_line();
    rptr->b_read(is);
    return;
  }
  rptr = nullptr;
}

void vsl_b_read(vsl_b_istream& is, volm_osm_object_line_sptr& sptr)
{
  volm_osm_object_line* rptr = nullptr;
  vsl_b_read(is, rptr);
  sptr = rptr;
}
