//:
// \file

#include "bvgl_changes.h"
#include "bvgl_change_obj.h"
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon_scan_iterator.h>

vil_image_view_base_sptr
bvgl_changes::create_mask_from_objs(unsigned ni, unsigned nj, const std::string& change_type)
{
  auto* mask = new vil_image_view<vxl_byte>(ni, nj);
  mask->fill(0);

  //index through the polygons and create the boolean mask image
  for (auto & obj : objs_)
  {
    vgl_polygon<double> v_poly =  obj->poly();
    vgl_polygon_scan_iterator<double> psi(v_poly, false);
    for (psi.reset(); psi.next();){
      int y = psi.scany();
      for (int x = psi.startx(); x<=psi.endx(); ++x)
      {
        auto u = static_cast<unsigned>(x);
        auto v = static_cast<unsigned>(y);
        if (u >= ni || v >= nj)
          continue;
        if (obj->type().compare(change_type)==0)
          (*mask)(u,v) = 255;
      }
    }
  }

  return mask;
}

vil_image_view_base_sptr
bvgl_changes::create_mask_from_objs_all_types(unsigned ni, unsigned nj)
{
  auto* mask = new vil_image_view<vxl_byte>(ni, nj);
  mask->fill(0);

  //index through the polygons and create the boolean mask image
  for (auto & obj : objs_)
  {
    vgl_polygon<double> v_poly =  obj->poly();
    vgl_polygon_scan_iterator<double> psi(v_poly, false);
    for (psi.reset(); psi.next();){
      int y = psi.scany();
      for (int x = psi.startx(); x<=psi.endx(); ++x)
      {
        auto u = static_cast<unsigned>(x);
        auto v = static_cast<unsigned>(y);
        if (obj->type().compare("dont_care")!=0)
          (*mask)(u,v) = 255;
        else  // don't care areas
          (*mask)(u,v) = 125;
      }
    }
  }

  return mask;
}

void bvgl_changes::add_obj(const bvgl_change_obj_sptr& obj)
{
  objs_.push_back(obj);
}

void bvgl_changes::remove_obj(const bvgl_change_obj_sptr& obj)
{
  auto iter = objs_.begin();
  while (iter!=objs_.end()) {
    if (*iter == obj) {
      objs_.erase(iter);
      std::cout << "DELETED!" << std::endl;
      return;
    }
    iter++;
  }
  std::cout << "Object is no FOUND!" << std::endl;
}

#if 0
void bvgl_changes::xml_read()
{
}

void bvgl_changes::xml_write()
{
}
#endif // 0

//: Return IO version number;
unsigned char
bvgl_changes::version(  ) const
{
  return 1;
}

//: binary IO write
void bvgl_changes::b_write(vsl_b_ostream& os)
{
  // first write the version number;
  unsigned char ver = version();
  vsl_b_write(os, ver);

  vsl_b_write(os, img_name_);
  vsl_b_write(os, objs_.size());
  for (auto & obj : objs_) {
    obj->b_write(os);
  }
}


//: binary IO read
void bvgl_changes::b_read(vsl_b_istream& is)
{
  // first read the version number;
  unsigned char ver;
  vsl_b_read(is, ver);

  switch (ver)
  {
   case 1:
   {
    vsl_b_read(is, img_name_);
    unsigned size;
    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; ++i) {
      bvgl_change_obj o;
      o.b_read(is);
      objs_.push_back(new bvgl_change_obj(o));
    }
    break;
   }
   default:
    std::cout << "In bvgl_changes::b_read() -- Unrecognized version number " << ver << std::endl;
    break;
  }

  return;
}

bvgl_change_obj_sptr
bvgl_changes::obj(unsigned int i)
{
  if (i<size())
    return objs_[i];
  return nullptr;
}
