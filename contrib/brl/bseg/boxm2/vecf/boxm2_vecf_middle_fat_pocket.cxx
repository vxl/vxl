#include <cstdlib>
#include <fstream>
#include "boxm2_vecf_middle_fat_pocket.h"
boxm2_vecf_middle_fat_pocket::boxm2_vecf_middle_fat_pocket(std::string const& geometry_file){

  std::ifstream istr(geometry_file.c_str());
  if(!istr){
    std::cout << "FATAL! - geometry file path not valid = " << geometry_file << '\n';
    return;
  }
  // construct paths to component geometry files
  std::map<std::string, std::string> middle_fat_pocket_map;
  std::string component, path;
  while(istr >> component >> path)
    middle_fat_pocket_map[component] = path;

  std::string base_path, knot_path, param_path;

  std::map<std::string, std::string>::iterator pit;

  pit = middle_fat_pocket_map.find("base_path");
  if(pit == middle_fat_pocket_map.end()){
    std::cout << "FATAL - base_path not defined\n";
    return;
  }else
    base_path = pit->second;

  pit = middle_fat_pocket_map.find("knot_path");
  if(pit == middle_fat_pocket_map.end()){
    std::cout << "FATAL - knot_path not defined\n";
    return;
  }else
    knot_path = base_path + pit->second;
  if(!vul_file::exists(knot_path)){
    std::cout << "FATAL - " << knot_path << " does not exist\n";
    return;
  }

  pit = middle_fat_pocket_map.find("param_path");
  if(pit == middle_fat_pocket_map.end()){
    std::cout << "FATAL - param_path not defined\n";
    return;
  }else
    param_path = base_path + pit->second;
  if(!vul_file::exists(param_path)){
    std::cout << "FATAL - " << param_path << " does not exist\n";
    return;
  }
  istr.close();

  std::ifstream kistr(knot_path.c_str());
  if(!kistr){
    std::cout << "FATAL! - can't open = " << knot_path << '\n';
    return;
  }
  std::vector<vgl_point_2d<double> > knots;
  double x, y;
  char c;
  while(kistr >> x >> c >> y){
    if(c != ','){
      std::cout << "FATAL! knot file parse error\n";
      return;
    }
    vgl_point_2d<double> knot(x, y);
    knots.push_back(knot);
  }
  kistr.close();

  std::ifstream pistr(param_path.c_str());
  if(!pistr){
    std::cout << "FATAL! - can't open = " << param_path << '\n';
    return;
  }
  pistr >> params_;
  pistr.close();
  base_ = bvgl_spline_region_3d<double>(knots, params_.normal_, params_.origin_, params_.tolerance_);
  pocket_ = bvgl_scaled_shape_3d<double>(base_, params_.max_norm_distance_, params_.scale_at_midpt_, params_.scale_at_max_, params_.tolerance_);
  if(params_.fit_to_subject_)
    this->apply_scale_params();
  else
    this->apply_deformation_params();
}

boxm2_vecf_middle_fat_pocket::boxm2_vecf_middle_fat_pocket(bvgl_scaled_shape_3d<double> const& ss3d, boxm2_vecf_middle_fat_pocket_params const& params)
{
  pocket_ = ss3d;
  base_ = (ss3d.cross_sections())[0];
  params_ = params;
  this->apply_deformation_params();
}

boxm2_vecf_middle_fat_pocket boxm2_vecf_middle_fat_pocket::deform() const{
  bvgl_scaled_shape_3d<double> sshape = pocket_.deform(params_.lambda_, params_.gamma_, params_.principal_eigenvector_);
  return boxm2_vecf_middle_fat_pocket(sshape, params_);
}

void boxm2_vecf_middle_fat_pocket::apply_scale_params(){
  pocket_.set_aniso_scale(params_.su_, params_.sv_, params_.sw_);
  pocket_.apply_parameters_to_cross_sections();
}
void boxm2_vecf_middle_fat_pocket::apply_deformation_params(){
  pocket_.set_lambda(params_.lambda_);
  pocket_.set_gamma(params_.gamma_);
  pocket_.set_principal_eigenvector(params_.principal_eigenvector_);
  pocket_.set_principal_offset(params_.principal_offset_);
  pocket_.apply_parameters_to_cross_sections();
}
bool boxm2_vecf_middle_fat_pocket::inverse_vector_field(vgl_point_3d<double> const& p, vgl_vector_3d<double>& inv_v) const{
  return pocket_.inverse_vector_field(p, inv_v);
}

//for debug purposes can be removed
void boxm2_vecf_middle_fat_pocket::print_vf_centroid_scan(double off_coef) const{
  bvgl_spline_region_3d<double> base = pocket_.base();
  vgl_point_3d<double> c = base.centroid();
  vgl_vector_3d<double> n = base.normal(), uvec, vvec;
  base.plane_coordinate_vectors(uvec, vvec);
  vgl_vector_3d<double> off = off_coef*uvec;
  c = c+ off;
  double h = pocket_.max_norm_distance();
  double dh = h/100.0;
  for(double t = h; t<2.0*h; t+=dh){
    vgl_point_3d<double> p = c + t*n;
   vgl_point_3d<double> cp = pocket_.closest_point(p);
    vgl_vector_3d<double> vf(0.0, 0.0, 0.0);
    bool valid = pocket_.vector_field(cp, vf);
    std::cout << t << ' ' << vf.x() << ' ' << vf.y() << ' ' << vf.z() << ' ' << valid << '\n';
  }
}
bool boxm2_vecf_middle_fat_pocket::closest_inverse_vector_field(vgl_point_3d<double> const& p, vgl_vector_3d<double>& inv_v) const{
  vgl_point_3d<double> cp = pocket_.closest_point(p);
  vgl_vector_3d<double> forward_vf;
  if( pocket_.vector_field(cp, forward_vf)){
    inv_v = -forward_vf;
    return true;
  }
  return false;
}
