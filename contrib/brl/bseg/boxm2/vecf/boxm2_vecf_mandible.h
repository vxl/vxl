#ifndef boxm2_vecf_mandible_h_
#define boxm2_vecf_mandible_h_
//:
// \file
// \brief  The mandible represented as a generalized cylinder
//
// \author J.L. Mundy
// \date   28 Aug 2015
//
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include "boxm2_vecf_spline_field.h"
class cross_section{
 public:
  cross_section(){}
 cross_section(double t, vgl_point_3d<double> p, vgl_plane_3d<double> plane, vgl_pointset_3d<double> ptset):
  t_(t),p_(p), plane_(plane), ptset_(ptset){}
  //: accessors
  double t() const {return t_;}
  vgl_point_3d<double> p(){return p_;}
  vgl_plane_3d<double> pl() const{return plane_;}
  vgl_pointset_3d<double> pts() const{return ptset_;}

  cross_section apply_vector(vgl_vector_3d<double> const& v) const;
  void display_cross_section_plane(vcl_ofstream& ostr) const;
  void display_cross_section_pts(vcl_ofstream& ostr) const;
  void display_cross_section_normal_disks(vcl_ofstream& ostr) const; 
 private:
  double t_;
  vgl_point_3d<double> p_;
  vgl_plane_3d<double> plane_;
  vgl_pointset_3d<double> ptset_;
};
class boxm2_vecf_mandible{
 public:

 boxm2_vecf_mandible(){
   fill_boundary_map();
  }

 boxm2_vecf_mandible(vcl_map<vcl_string, unsigned> const& boundary_knots, vgl_cubic_spline_3d<double> const& axis,
                     vcl_vector<cross_section> const& csects):
 boundary_knots_(boundary_knots), axis_(axis), cross_sections_(csects){fill_boundary_map();}

 void read_axis_spline(vcl_ifstream& istr){
   istr >> axis_;
 }
 const vgl_cubic_spline_3d<double>& axis() const {return axis_;}

 boxm2_vecf_spline_field translate(vgl_vector_3d<double> const& tr);

 //: offset axis curve in the curve normal direction
 boxm2_vecf_spline_field offset_axis(double offset);
 
 //: adjust slope of ramus relative to nominal
 boxm2_vecf_spline_field tilt_ramus(double delta_z_at_condyle);

 //: adjust slope of body relative to nominal
 boxm2_vecf_spline_field  tilt_body(double delta_y_at_chin);

 //: construct cross sections (planes defined at each knot perpendicular to the axis curve)
 // input is a pointset for the entire mandible
 void load_cross_section_pointsets(vcl_ifstream& istr);

 //: create a new mandible by applying a vector field
 boxm2_vecf_mandible apply_vector_field(boxm2_vecf_spline_field const& field) const;
 
 void display_axis_spline(vcl_ofstream& ostr) const;
 void display_cross_section_planes(vcl_ofstream& ostr) const;
 void display_cross_section_pointsets(vcl_ofstream& ostr) const;
 void display_surface_disks(vcl_ofstream& ostr) const;

 private:
 void fill_boundary_map();
 vcl_map<vcl_string, unsigned> boundary_knots_;
 vgl_cubic_spline_3d<double> axis_;
 vcl_vector<cross_section> cross_sections_;
};
vcl_ostream&  operator << (vcl_ostream& s, boxm2_vecf_mandible const& pr);
vcl_istream&  operator >> (vcl_istream& s, boxm2_vecf_mandible& pr);

#endif// boxm2_vecf_mandible
