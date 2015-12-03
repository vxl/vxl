#include "boxm2_vecf_mandible.h"
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vcl_cmath.h>
//==========================================
//: mandible class member implementations
void boxm2_vecf_mandible::fill_boundary_map(){
  boundary_knots_["right_ramus_end"] = 44;
  boundary_knots_["right_angle_center"] = 35;
  boundary_knots_["right_body_end"] = 34;
  boundary_knots_["mid_jaw"] = 21;
  boundary_knots_["left_body_start"] = 10;
  boundary_knots_["left_angle_center"] =9 ;
  boundary_knots_["left_ramus_start"] = 0;
}
boxm2_vecf_mandible::boxm2_vecf_mandible(vcl_string const& geometry_file){
  vcl_ifstream istr(geometry_file.c_str());
  if(!istr){
    vcl_cout << " invalid path for manidble geometry " << geometry_file << '\n';
    fill_boundary_map();
    return;
  }
  vcl_map<vcl_string, vcl_string> mandible_paths;
  vcl_string component, path;
  while(istr >> component >> path)
    mandible_paths[component] = path;
  vcl_map<vcl_string, vcl_string>::iterator pit;
  pit = mandible_paths.find("axis_spline");
  if(pit != mandible_paths.end()){
    vcl_ifstream sstr((pit->second).c_str());
    this->read_axis_spline(sstr);
  }
  pit = mandible_paths.find("cross_section_pointset");
  if(pit != mandible_paths.end()){
      vcl_ifstream cstr((pit->second).c_str());
      this->load_cross_section_pointsets(cstr);
  }
}
boxm2_vecf_spline_field boxm2_vecf_mandible::translate(vgl_vector_3d<double> const& tr){
  vcl_vector<vgl_point_3d<double> > knots = axis_.knots();
  vcl_vector<vgl_vector_3d<double> > ret(axis_.n_knots(), tr);
  boxm2_vecf_spline_field field(axis_, ret);
  return field;
}

// adjust the x coordinates with respect to the symmetry plane
boxm2_vecf_spline_field boxm2_vecf_mandible::offset_axis(double offset) {
  vcl_vector<vgl_point_3d<double> > knots = axis_.knots();
  vcl_vector<vgl_vector_3d<double> > ret;
  for(unsigned i = 0; i<axis_.n_knots(); ++i){
    vgl_point_3d<double> p = knots[i];
    // the tangent unit vector to the curve at knot i
    double t = static_cast<double>(i);
    vgl_vector_3d<double> dpdt = axis_.tangent(t);
    dpdt /= dpdt.length();
    // the normal to the curve in the X-Z plane
    vgl_vector_3d<double> unit_y(0.0, 1.0, 0.0);
    vgl_vector_3d<double> curve_normal = cross_product(unit_y,dpdt);
    curve_normal/=curve_normal.length();
    vgl_vector_3d<double> motion = curve_normal*offset;
    ret.push_back(motion);
  }
  boxm2_vecf_spline_field field(axis_, ret);
  return field;
}

boxm2_vecf_spline_field boxm2_vecf_mandible::tilt_ramus(double delta_z_at_condyle){
  vcl_vector<vgl_point_3d<double> > knots = axis_.knots();
  vcl_vector<vgl_vector_3d<double> > ret;
  for(unsigned i = 0; i<axis_.n_knots(); ++i){
    vgl_point_3d<double> p = knots[i];
    double t = static_cast<double>(i);
    bool is_right = p.x()<0.0;
    double dz = 0.0;
    if(is_right){
      double tstart = static_cast<double>(boundary_knots_["right_ramus_end"]);
      double tend = static_cast<double>(boundary_knots_["right_angle_center"]);
      dz = (tend-t)/(tend-tstart);
      dz *= delta_z_at_condyle;
    }else{
      double tstart = static_cast<double>(boundary_knots_["left_ramus_end"]);
      double tend = static_cast<double>(boundary_knots_["left_angle_center"]);
      dz = (tend-t)/(tend-tstart);
      dz *= delta_z_at_condyle;
    }
    if(dz<0.0) dz = 0.0;
    // the tangent unit vector to the curve at knot i
    vgl_vector_3d<double> dpdt = axis_.tangent(t);
    dpdt /= dpdt.length();

    vgl_vector_3d<double> unit_x(1.0, 0.0, 0.0), unit_z(0.0, 0.0, 1.0);
    if(is_right)
      unit_x *= -1.0;
    vgl_vector_3d<double> curve_normal = cross_product(dpdt,unit_x);
    curve_normal/=curve_normal.length();
    
    vgl_vector_3d<double> motion = curve_normal*dz/vcl_fabs(dot_product(curve_normal, unit_z));
    ret.push_back(motion);
  }
  boxm2_vecf_spline_field field(axis_, ret);
  return field;
}

boxm2_vecf_spline_field  boxm2_vecf_mandible::tilt_body(double delta_y_at_chin) {
  vcl_vector<vgl_point_3d<double> > knots = axis_.knots();
  vcl_vector<vgl_vector_3d<double> > ret;
  for(unsigned i = 0; i<axis_.n_knots(); ++i){
    vgl_point_3d<double> p = knots[i];
    double t = static_cast<double>(i);
    double dy = 0.0;
    bool is_right = p.x()<0.0;
    // find the magnitude of the motion vector, which falls off linearly from the chin
    // domain body for either left or right of the symmetry plane
    if(is_right){
      double tstart = static_cast<double>(boundary_knots_["mid_jaw"]);
      double tend = static_cast<double>(boundary_knots_["right_angle_center"]);
      dy = (tend-t)/(tend-tstart);
      dy *= delta_y_at_chin;
    }else{
      double tstart = static_cast<double>(boundary_knots_["mid_jaw"]);
      double tend = static_cast<double>(boundary_knots_["left_angle_center"]);
      dy = (tend-t)/(tend-tstart);
      dy *= delta_y_at_chin;
    }
    // turn off the motion at the boundary of body and angle
    if(dy<0.0) dy = 0.0;
    // the tangent unit vector to the curve at knot i
    vgl_vector_3d<double> dpdt = axis_.tangent(t);
    dpdt /= dpdt.length();
    vgl_vector_3d<double> unit_x(1.0, 0.0, 0.0), unit_y(0.0, 1.0, 0.0), unit_z(0.0, 0.0, 1.0), motion;

    // determine which orientation the curve normal should have and then compute the motion normal to the curve
    // to achieve the required dy
    double dp_x = dot_product(dpdt, unit_x), dp_z = dot_product(dpdt, unit_z);
    if(vcl_fabs(dp_x)>vcl_fabs(dp_z)){
    vgl_vector_3d<double> curve_normal = cross_product(dpdt,unit_z);  
    curve_normal/=curve_normal.length();
    motion= curve_normal*dy/vcl_fabs(dot_product(curve_normal, unit_y));
    }else{
      if(is_right)
        unit_x *= -1.0;
      vgl_vector_3d<double> curve_normal = cross_product(dpdt,unit_x);  
      curve_normal/=curve_normal.length();
      motion= curve_normal*dy/vcl_fabs(dot_product(curve_normal, unit_y));
    }
    ret.push_back(motion);
  }
  boxm2_vecf_spline_field field(axis_, ret);
  return field;
}

boxm2_vecf_mandible boxm2_vecf_mandible::apply_vector_field(boxm2_vecf_spline_field const& field) const{
  // transform self's spline
  vgl_cubic_spline_3d<double> axis_v = field.apply_field();
  vcl_vector<bvgl_cross_section> csects_v;
  for(vcl_vector<bvgl_cross_section>::const_iterator cit =  cross_sections_.begin();
      cit != cross_sections_.end(); ++cit){
    double t = cit->t();
    vgl_vector_3d<double> v = field(t);
    bvgl_cross_section csv = cit->apply_vector(v);
    csects_v.push_back(csv);
  }
  return boxm2_vecf_mandible(boundary_knots_, axis_v, csects_v);
}

void boxm2_vecf_mandible::display_axis_spline(vcl_ofstream& ostr) const{
  bvrml_write::write_vrml_header(ostr);
  // display the knots
  vcl_vector<vgl_point_3d<double> > knots = axis_.knots();
  unsigned n = static_cast<unsigned>(knots.size());
  double nd = static_cast<double>(n-1);
  float r = 1.0f;
  for(unsigned i =0; i<n; ++i){
    vgl_point_3d<double> p = knots[i];
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r);
    bool found = false;
    for(vcl_map<vcl_string, unsigned>::const_iterator bit =  boundary_knots_.begin();
        bit != boundary_knots_.end()&&!found;++bit)
      found = bit->second == i;
    if(found)
      bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.0f, 1.0f);      
    else
      bvrml_write::write_vrml_sphere(ostr, sp, 0.0f, 1.0f, 0.0f);      
  }
  // display the spline points
  for(double t = 0; t<=nd; t+=0.05){
    vgl_point_3d<double> p = axis_(t);
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, 0.25f);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 1.0f, 0.0f);
  }
  ostr.close();
}
