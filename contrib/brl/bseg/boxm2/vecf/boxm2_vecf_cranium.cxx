#include "boxm2_vecf_cranium.h"
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vcl_map.h>

boxm2_vecf_cranium::boxm2_vecf_cranium(vcl_string const& geometry_file, unsigned nbins): nbins_(nbins){
  vcl_ifstream istr(geometry_file.c_str());
  if(!istr){
    vcl_cout << " invalid path for cranium geometry " << geometry_file << '\n';
    return;
  }
  vcl_map<vcl_string, vcl_string> cranium_paths;
  vcl_string component, path;
  while(istr >> component >> path)
    cranium_paths[component] = path;

  vcl_map<vcl_string, vcl_string>::iterator pit;
  pit = cranium_paths.find("cranium");
  if(pit != cranium_paths.end()){
    vcl_ifstream cstr((pit->second).c_str());
    this->read_cranium(cstr);
  }
}
void boxm2_vecf_cranium::read_cranium(vcl_istream& cstr){
  cstr >> ptset_;
  double surface_dist_thresh = 1.0;
  index_ = bvgl_grid_index_3d(nbins_, nbins_, nbins_, ptset_, surface_dist_thresh);
}


void boxm2_vecf_cranium::display_vrml(vcl_ofstream& ostr) const{
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
