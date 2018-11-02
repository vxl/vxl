#include <iostream>
#include <map>
#include "boxm2_vecf_cranium.h"
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_bounding_box.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_vecf_cranium::boxm2_vecf_cranium(std::string const& geometry_file, unsigned nbins): nbins_(nbins){
  std::ifstream istr(geometry_file.c_str());
  if(!istr){
    std::cout << " invalid path for cranium geometry " << geometry_file << '\n';
    return;
  }
  std::map<std::string, std::string> cranium_paths;
  std::string component, path;
  while(istr >> component >> path)
    cranium_paths[component] = path;

  std::map<std::string, std::string>::iterator pit;
  pit = cranium_paths.find("cranium");
  if(pit != cranium_paths.end()){
    std::ifstream cstr((pit->second).c_str());
    this->read_cranium(cstr);
  }
}
void boxm2_vecf_cranium::read_cranium(std::istream& cstr){
  cstr >> ptset_;
  double surface_dist_thresh = 1.0;
  index_ = bvgl_grid_index_3d<double>(nbins_, nbins_, nbins_, ptset_, surface_dist_thresh);
}

bool boxm2_vecf_cranium::inverse_vector_field(vgl_point_3d<double> const&  /*p*/, vgl_vector_3d<double>& inv_vf) const{
  inv_vf.set(-params_.offset_.x(), -params_.offset_.y(), -params_.offset_.z());
  return true;
}

void boxm2_vecf_cranium::display_vrml(std::ofstream& ostr) const{
  bvrml_write::write_vrml_header(ostr);
  unsigned n = ptset_.npts();
  float r = 3.0f, h = 0.1f;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i);
    vgl_vector_3d<double> n = ptset_.n(i);
    bvrml_write::write_vrml_cylinder(ostr, p, n, r, h, 0.0f, 1.0f, 0.3f, 1);
  }
  ostr.close();
}
