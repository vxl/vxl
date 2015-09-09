#include "boxm2_vecf_mandible.h"
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vcl_cmath.h>
//: cross section class member implementations

// motion vector should be coplanar
cross_section cross_section::apply_vector( vgl_vector_3d<double> const& v) const{
  vgl_point_3d<double> pt_v = p_ + v;
  unsigned n = ptset_.npts();
  vcl_vector<vgl_point_3d<double> > pts;
  vcl_vector<vgl_vector_3d<double> > normals;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i) + v;
    pts.push_back(p);
    normals.push_back(ptset_.n(i));
  }
  vgl_pointset_3d<double> ptset_v(pts, normals);
  vgl_plane_3d<double> plane(plane_.normal(), pt_v);
  return cross_section(t_, pt_v, plane, ptset_v);
}


void cross_section::display_cross_section_pts(vcl_ofstream& ostr) const{
  unsigned n = ptset_.npts();
  float r = 3.0f;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i);
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.5f, 1.0f);      
  }
}
void cross_section::display_cross_section_plane( vcl_ofstream& ostr) const{
  vgl_vector_3d<double> norm = plane_.normal();
  float r = 10.0f, h = 0.1f;
  bvrml_write::write_vrml_cylinder(ostr, p_, norm, r, h, 0.0f, 1.0f, 1.0f, 1);
}

void cross_section::display_cross_section_normal_disks(vcl_ofstream& ostr) const{
  unsigned n = ptset_.npts();
  float r = 3.0f, h = 0.1f;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i);
    vgl_vector_3d<double> n = ptset_.n(i);
    bvrml_write::write_vrml_cylinder(ostr, p, n, r, h, 0.0f, 1.0f, 0.3f, 1);
  }
}
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
void boxm2_vecf_mandible::load_cross_section_pointsets(vcl_ifstream& istr){
  cross_sections_.clear();
  // load the entire mandible pointset
  vgl_pointset_3d<double> ptset;
  istr >> ptset;
  // scan through the axis knots
  for(double t = 0.0; t<=axis_.max_t(); t += 0.5){
    vgl_point_3d<double> p = axis_(t);
    vgl_plane_3d<double> norm_plane = axis_.normal_plane(t);
    vgl_pointset_3d<double> cpts = vgl_intersection(norm_plane, ptset,0.5), final_cpts;
    vgl_box_3d<double> total_box = vgl_bounding_box(ptset);
    vgl_box_3d<double> box_minus, box_plus;
    vgl_point_3d<double> pmin_minus = total_box.min_point();
    vgl_point_3d<double> pmax_minus = total_box.max_point();
    pmax_minus.set(0.0, pmax_minus.y(), pmax_minus.z());
    box_minus.add(pmin_minus);    box_minus.add(pmax_minus);
    vgl_point_3d<double> pmin_plus = total_box.min_point();
    pmin_plus.set(0.0, pmin_plus.y(), pmin_plus.z());
    vgl_point_3d<double> pmax_plus = total_box.max_point();
    box_plus.add(pmin_plus);    box_plus.add(pmax_plus);
    if(p.x()<0.0)
      final_cpts = vgl_intersection(box_minus, cpts);
    else
      final_cpts = vgl_intersection(box_plus, cpts);
    cross_section cs(t, p, norm_plane, final_cpts);
    cross_sections_.push_back(cs);
  }
}
boxm2_vecf_mandible boxm2_vecf_mandible::apply_vector_field(boxm2_vecf_spline_field const& field) const{
  // transform self's spline
  vgl_cubic_spline_3d<double> axis_v = field.apply_field();
  vcl_vector<cross_section> csects_v;
  for(vcl_vector<cross_section>::const_iterator cit =  cross_sections_.begin();
      cit != cross_sections_.end(); ++cit){
    double t = cit->t();
    vgl_vector_3d<double> v = field(t);
    cross_section csv = cit->apply_vector(v);
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
        if(i == 18)
                vcl_cout << ' ';
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
void boxm2_vecf_mandible::display_cross_section_planes(vcl_ofstream& ostr) const{
  bvrml_write::write_vrml_header(ostr);
  unsigned n = static_cast<unsigned>(cross_sections_.size());
  for(unsigned i = 0; i<n; i++)
    cross_sections_[i].display_cross_section_plane(ostr);
  ostr.close();
}

void boxm2_vecf_mandible::display_cross_section_pointsets(vcl_ofstream& ostr) const{
  bvrml_write::write_vrml_header(ostr);
  unsigned n = static_cast<unsigned>(cross_sections_.size());
  for(unsigned i = 0; i<n; i++)
    cross_sections_[i].display_cross_section_pts(ostr);
  ostr.close();
}

void boxm2_vecf_mandible::display_surface_disks(vcl_ofstream& ostr) const{
  bvrml_write::write_vrml_header(ostr);
  unsigned n = static_cast<unsigned>(cross_sections_.size());
  for(unsigned i = 0; i<n; i++)
    cross_sections_[i].display_cross_section_normal_disks(ostr);
  ostr.close();
}
