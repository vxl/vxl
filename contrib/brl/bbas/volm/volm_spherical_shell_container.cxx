#include "volm_spherical_shell_container.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <bvrml/bvrml_write.h>
#include <volm/volm_io.h>
#include <volm/volm_category_io.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_vector_io.h>

// constructor
volm_spherical_shell_container::volm_spherical_shell_container(double  /*radius*/, float  /*cap_angle*/, float point_angle, float top_angle, float bottom_angle)
{
  usph_ = new vsph_unit_sphere(point_angle, top_angle, 180.0-bottom_angle);
}

//: Minimal constructor (to internally construct vsph_unit_sphere)
volm_spherical_shell_container::
volm_spherical_shell_container(double point_angle, double min_theta,
                               double max_theta) {
  usph_ = new vsph_unit_sphere(point_angle, min_theta, max_theta);
}

std::vector<vgl_point_3d<double> > volm_spherical_shell_container::cart_points() const
{
  const std::vector<vgl_vector_3d<double> >& cart_vects = usph_->cart_vectors_ref();
  unsigned n  = cart_vects.size();
  std::vector<vgl_point_3d<double> > temp(n);
  for (unsigned i = 0; i<n; ++i) {
    const vgl_vector_3d<double>& v = cart_vects[i];
    temp[i].set(v.x(), v.y(), v.z());
  }
  return temp;
}

// the angle units are as maintained in usph_
std::vector<vsph_sph_point_3d> volm_spherical_shell_container::sph_points() const
{
  const std::vector<vsph_sph_point_2d>& sph_pts = usph_->sph_points_ref();
  unsigned n  = sph_pts.size();
  std::vector<vsph_sph_point_3d> temp(n);
  for (unsigned i = 0; i<n; ++i)
    temp[i].set(1.0, sph_pts[i].theta_, sph_pts[i].phi_);
  return temp;
}

void volm_spherical_shell_container::draw_template(const std::string& vrml_file_name)
{
  std::ofstream ofs(vrml_file_name.c_str());
  if (!ofs.is_open()) {
    std::cout << " in ::draw_template vrml path does not exist - "
             << vrml_file_name << '\n';
    return;
  }
  std::vector<vgl_point_3d<double> > cart_pts = this->cart_points();
  // write the header
  bvrml_write::write_vrml_header(ofs);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(ofs, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_x, (float)rad*20, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_y, (float)rad*20, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_z, (float)rad*20, 1.0f, 1.0f, 0.0f);
  // write the voxel structure

  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (const auto & cart_pt : cart_pts) {
    vgl_vector_3d<double> ray = cart_pt-orig;
    //bvrml_write::write_vrml_line(ofs, orig, ray, 10.0f, 0.0f, 0.0f, 1.0f);
    bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, 0.5f, 0.0f, 0.0f, 1.0f);
  }
  ofs.close();
}

//: draw each disk with a color with respect to the values, the size and order of the values should be the size and order of the cart_points
void volm_spherical_shell_container::draw_template(const std::string& vrml_file_name, std::vector<unsigned char>& values, unsigned char special)
{
  assert(values.size() == usph_->size());

  std::ofstream ofs(vrml_file_name.c_str());
  if (!ofs.is_open()) {
    std::cout << " in ::draw_template vrml path does not exist - "
             << vrml_file_name << '\n';
    return;
  }
  std::vector<vgl_point_3d<double> > cart_pts = this->cart_points();

  // write the header
  bvrml_write::write_vrml_header(ofs);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(ofs, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_x, (float)rad*20, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_y, (float)rad*20, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(ofs, cent_ray, axis_z, (float)rad*20, 0.0f, 1.0f, 1.0f);
  vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y(), (float)cent.z()+20, (float)rad);
  bvrml_write::write_vrml_sphere(ofs, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

  // write the voxel structure
  float disc_radius = 0.09f;
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_pts.size(); i++) {
    vgl_vector_3d<double> ray = cart_pts[i]-orig;
    if (values[i] == special)
      bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, disc_radius, 1.0f, 1.0f, 0.0f);
    else if (values[i] == 253) // invalid
      bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, disc_radius, 1.0f, 0.0f, 0.0f);
    else
      bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, disc_radius, 0.0f, 0.0f, values[i]/255.0f);
  }
  ofs.close();
}

//: generate panaroma image
//  create an image with width 360 and height 180 to pour all the ray values such that left most column is east direction, and the viewsphere is painted clockwise
void volm_spherical_shell_container::panaroma_img(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values)
{
  assert(values.size() == usph_->size());
  std::vector<vsph_sph_point_3d> sph_pts = this->sph_points();
  img.set_size(360, 180);
  img.fill(127);
  for (unsigned i = 0; i < sph_pts.size(); i++) {
    vsph_sph_point_3d pt = sph_pts[i];
    auto ii = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.phi_)*vnl_math::deg_per_rad+0.5);
    auto jj = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.theta_)*vnl_math::deg_per_rad+0.5);
    if (ii >= img.ni() || jj >= img.nj()) // cannot be negative since unsigned ...
      continue;
    if (values[i] == 253) { // invalid
      img(ii,jj).r = 255;
      img(ii,jj).g = 0;
      img(ii,jj).b = 0;
    }
    else if (values[i] == 254) { // sky
      img(ii,jj).r = 0;
      img(ii,jj).g = 0;
      img(ii,jj).b = 255;
    }
    else {
      img(ii,jj).r = values[i];
      img(ii,jj).g = values[i];
      img(ii,jj).b = values[i];
    }
  }
}

void volm_spherical_shell_container::panaroma_img_class_labels(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values)
{
  assert(values.size() == usph_->size());
  std::vector<vsph_sph_point_3d> sph_pts = this->sph_points();
  img.set_size(360, 180);
  img.fill(127);
  for (unsigned i = 0; i < sph_pts.size(); i++) {
    vsph_sph_point_3d pt = sph_pts[i];
    auto ii = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.phi_)*vnl_math::deg_per_rad+0.5);
    auto jj = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.theta_)*vnl_math::deg_per_rad+0.5);
    if (ii >= img.ni() || jj >= img.nj()) // cannot be negative since unsigned ...
      continue;
    if (values[i] == 253) { // invalid
      std::cout << " pixel " << ii << ',' << jj << " is not valid!\n";
      img(ii,jj).r = 255;
      img(ii,jj).g = 0;
      img(ii,jj).b = 0;
    }
    else if (values[i] == 254) { // sky
      img(ii,jj).r = 0;
      img(ii,jj).g = 0;
      img(ii,jj).b = 255;
    }
    else if (values[i] == 29) { // pier
      img(ii,jj).r = 255;
      img(ii,jj).g = 0;
      img(ii,jj).b = 0;
    }
#if 0
    else if (volm_label_table::land_id.find((int)values[i]) == volm_label_table::land_id.end()) {
      std::cerr << "cannot find this value: " << (int)values[i] << " in the color table!\n";
      img(ii,jj) = vil_rgb<vxl_byte>(255, 0, 0);
    }
    else
      img(ii,jj) = volm_label_table::land_id[(int)values[i]].color_;
#endif // 0
    else
      img(ii,jj) = volm_osm_category_io::volm_land_table[values[i]].color_;
      //img(ii,jj) = volm_label_table::get_color(values[i]); // returns invalid color if it cannot find this id
  }
}


void volm_spherical_shell_container::panaroma_img_orientations(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values)
{
  assert(values.size() == usph_->size());
  std::vector<vsph_sph_point_3d> sph_pts = this->sph_points();
  img.set_size(360, 180);
  img.fill(127);
  for (unsigned i = 0; i < sph_pts.size(); i++) {
    vsph_sph_point_3d pt = sph_pts[i];
    auto ii = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.phi_)*vnl_math::deg_per_rad+0.5);
    auto jj = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.theta_)*vnl_math::deg_per_rad+0.5);
    if (ii >= img.ni() || jj >= img.nj()) // cannot be negative since unsigned ...
      continue;
    if (values[i] == 253) { // invalid
      img(ii,jj).r = 255;
      img(ii,jj).g = 0;
      img(ii,jj).b = 0;
    }
    else if (values[i] == 254) { // sky
      img(ii,jj).r = 0;
      img(ii,jj).g = 0;
      img(ii,jj).b = 255;
    }
    else if (volm_orient_table::ori_index_colors.find((int)values[i]) == volm_orient_table::ori_index_colors.end()) {
      std::cerr << "cannot find this value: " << (int)values[i] << " in the color table!\n";
      img(ii,jj) = vil_rgb<vxl_byte>(255, 0, 0);
    }
    else
      img(ii,jj) = volm_orient_table::ori_index_colors[(int)values[i]];
  }
}

void volm_spherical_shell_container::panaroma_images_from_combined(vil_image_view<vil_rgb<vxl_byte> >& img_orientation, vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values)
{
  assert(values.size() == usph_->size());
  std::vector<vsph_sph_point_3d> sph_pts = this->sph_points();
  img.set_size(360, 180);
  img_orientation.set_size(360, 180);
  img.fill(127);
  img_orientation.fill(127);
  for (unsigned i = 0; i < sph_pts.size(); i++) {
    vsph_sph_point_3d pt = sph_pts[i];
    auto ii = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.phi_)*vnl_math::deg_per_rad+0.5);
    auto jj = (unsigned)std::floor(vnl_math::angle_0_to_2pi(pt.theta_)*vnl_math::deg_per_rad+0.5);
    if (ii >= img.ni() || jj >= img.nj()) // cannot be negative since unsigned ...
      continue;
    std::cout << '(' << (int)values[i] << ", ";
    if (values[i] == 253) { // invalid
      img(ii,jj).r = 255;
      img(ii,jj).g = 0;
      img(ii,jj).b = 0;
      img_orientation(ii,jj).r = 255;
      img_orientation(ii,jj).g = 0;
      img_orientation(ii,jj).b = 0;
    }
    else if (values[i] == 254) { // sky
      img(ii,jj).r = 0;
      img(ii,jj).g = 0;
      img(ii,jj).b = 255;
      img_orientation(ii,jj).r = 0;
      img_orientation(ii,jj).g = 0;
      img_orientation(ii,jj).b = 255;
    }
    else {
      unsigned char orientation_value, label_value;
      volm_io_extract_values(values[i], orientation_value, label_value);
      std::cout << (int)orientation_value << ", " << (int)label_value << ") ";

      if (volm_orient_table::ori_index_colors.find((int)orientation_value) == volm_orient_table::ori_index_colors.end()) {
        std::cerr << "cannot find this value: " << (int)orientation_value << " in the color table!\n";
        img_orientation(ii,jj) = vil_rgb<vxl_byte>(255, 0, 0);
      }
      else
        img_orientation(ii,jj) = volm_orient_table::ori_index_colors[(int)orientation_value];

      //img(ii,jj) = volm_label_table::get_color(label_value); // returns invalid color if it cannot find this id
      img(ii,jj) = volm_osm_category_io::volm_land_table[label_value].color_;
    }
  }
}

//: binary IO write
void volm_spherical_shell_container::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, usph_);
}

//: binary IO read
void volm_spherical_shell_container::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver ==1) {
    vsl_b_read(is, usph_);
  }
  else {
    std::cerr << "volm_spherical_shell_container - unknown binary io version " << ver <<'\n';
    return;
  }
}

bool volm_spherical_shell_container::operator== (const volm_spherical_shell_container &other) const
{
  return *usph_ == *(other.unit_sphere());
}
