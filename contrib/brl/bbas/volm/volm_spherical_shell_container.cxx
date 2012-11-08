#include "volm_spherical_shell_container.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_line_segment_3d.h>
#include <bvrml/bvrml_write.h>

// constructor
volm_spherical_shell_container::volm_spherical_shell_container(double radius, float cap_angle, float point_angle, float top_angle, float bottom_angle)
  : radius_(radius),
    point_angle_(point_angle*vnl_math::pi_over_180),
    cap_angle_(cap_angle*vnl_math::pi_over_180), top_angle_(top_angle*vnl_math::pi_over_180), bottom_angle_(bottom_angle*vnl_math::pi_over_180)
{
  coord_sys_ = new vsph_spherical_coord(vgl_point_3d<double>(0.0,0.0,0.0), radius_);
  this->add_uniform_views();
  this->remove_top_and_bottom();
}

void volm_spherical_shell_container::add_uniform_views()
{
  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  double radius = radius_;
  vgl_point_3d<double> center = coord_sys_->origin();
  vcl_vector<vgl_point_3d<double> > verts;
  vgl_point_3d<double> v1(center.x(),center.y(),center.z()+radius); verts.push_back(v1);
  vgl_point_3d<double> v2(center.x(),center.y(),center.z()-radius); verts.push_back(v2);
  vgl_point_3d<double> v3(center.x()+radius,center.y(),center.z()); verts.push_back(v3);
  vgl_point_3d<double> v4(center.x()-radius,center.y(),center.z()); verts.push_back(v4);
  vgl_point_3d<double> v5(center.x(),center.y()+radius,center.z()); verts.push_back(v5);
  vgl_point_3d<double> v6(center.x(),center.y()-radius,center.z()); verts.push_back(v6);

  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  vcl_vector<vcl_vector<int> > triangles;

  vcl_vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);

  vcl_vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);

  vcl_vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);

  vcl_vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);

  vcl_vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);

  vcl_vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);

  vcl_vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);

  vcl_vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);
  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spherical coordinate system
  vgl_vector_3d<double> vector1=verts[triangles[0][0]]-center;
  vgl_vector_3d<double> vector2=verts[triangles[0][1]]-center;

  bool done=false;
  while (!done) {
    vcl_vector<vcl_vector<int> >  new_triangles;
    int ntri=(int)triangles.size();
    for (int i=0; i<ntri; i++) {
      vcl_vector<int> points;
      for (int j=0; j<3; j++) {
        // find the mid points of edges
        int next=j+1; if (next == 3) next=0;
        vgl_line_segment_3d<double> edge1(verts[triangles[i][j]],verts[triangles[i][next]]);
        vgl_point_3d<double> mid=edge1.point_t(0.5);

        // move the point onto the surface of the sphere
        vsph_sph_point_3d sv;
        coord_sys_->spherical_coord(mid, sv);
        coord_sys_->move_point(sv);
        mid = coord_sys_->cart_coord(sv);

        // add a new vertex for mid points of the edges of the triangle
        int idx = (int)verts.size();
        verts.push_back(mid);

        points.push_back(triangles[i][j]);  // existing vertex of the bigger triangle
        points.push_back(idx);              // new mid-point vertex
      }

      // add new samller 4 triangles instead of the old big one
      /******************************
                   /\
                  /  \
                 /    \
                /      \
               /--------\
              / \      / \
             /   \    /   \
            /     \  /     \
           /       \/       \
           -------------------
      *******************************/
      done=true;
      vcl_vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      // check for point_angles
      vcl_vector<vgl_point_3d<double> > triangle;
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle_)) done=false;

      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle_)) done=false;

      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle_)) done=false;

      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, point_angle_)) done=false;
    }
#if 0
    // check the angle again to see if the threshold is met
    vgl_vector_3d<double> vector1=verts[new_triangles[0][0]]-center;
    vgl_vector_3d<double> vector2=verts[new_triangles[0][1]]-center;
#endif
    triangles.clear();
    triangles=new_triangles;
  }  // done for the refine process

  // refine the vertices to points, eliminate duplicate ones and
  // also eliminate the ones below given elevation
  int ntri=(int)triangles.size();
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      vsph_sph_point_3d sv;
      coord_sys_->spherical_coord(verts[triangles[i][j]], sv);
      if (sv.theta_ < cap_angle_) {
        double dist;
        if ( find_closest(verts[triangles[i][j]],dist) ) {
          if (dist > 0.0001) { // make sure the two points are far enough
            cart_points_.push_back(verts[triangles[i][j]]);
            sph_points_.push_back(sv);
          }
        }
        else {
          cart_points_.push_back(verts[triangles[i][j]]);
          sph_points_.push_back(sv);
        }
      }
    }
  }
}

void volm_spherical_shell_container::remove_top_and_bottom()
{
  vcl_vector<vgl_point_3d<double> > cart_points_new;
  vcl_vector<vsph_sph_point_3d> sph_points_new;
  for (unsigned i = 0; i < sph_points_.size(); i++) {
    if (sph_points_[i].theta_ > top_angle_ && sph_points_[i].theta_ < vnl_math::pi - bottom_angle_) {
      sph_points_new.push_back(sph_points_[i]);
      cart_points_new.push_back(cart_points_[i]);
    }
  }
  
  sph_points_.clear();
  sph_points_ = sph_points_new;
  cart_points_.clear();
  cart_points_ = cart_points_new;
}


bool volm_spherical_shell_container::find_closest(vgl_point_3d<double> p, double& dist)
{
  double min_dist = 1E20;
  int uid = -1;
  for (unsigned i = 1; i < (unsigned)cart_points_.size(); i++) {
    vgl_point_3d<double> cp = cart_points_[i];
    double d = vgl_distance(cp,p);
    if (d < min_dist) {
      min_dist = d;
      uid = i;
    }
  }
  dist = min_dist;
  if (uid > -1)
    return true;
  else
    return false;
}


bool volm_spherical_shell_container::min_angle(vcl_vector<vgl_point_3d<double> > list, double point_angle)
{
  if (list.size() < 2)
    return false;

  vgl_point_3d<double> center = coord_sys_->origin();
  for (unsigned i=0; (unsigned)i<list.size(); i++) {
    unsigned next = i+1;
    if (next == list.size()) next = 0;
    vgl_vector_3d<double> vector1=list[i]-center;
    vgl_vector_3d<double> vector2=list[next]-center;
    if (angle(vector1, vector2) > point_angle)
      return false;
  }
  return true;
}

void volm_spherical_shell_container::draw_template(vcl_string vrml_file_name)
{
  vcl_ofstream ofs(vrml_file_name.c_str());
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
  for (unsigned i = 0; i < cart_points_.size(); i++) {
    vgl_vector_3d<double> ray = cart_points_[i]-orig;
    //bvrml_write::write_vrml_line(ofs, orig, ray, 10.0f, 0.0f, 0.0f, 1.0f);
    bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, 0.5f, 0.0f, 0.0f, 1.0f);
  }
  ofs.close();
}

//: draw each disk with a color with respect to the values, the size and order of the values should be the size and order of the cart_points
void volm_spherical_shell_container::draw_template(vcl_string vrml_file_name, vcl_vector<unsigned char>& values, unsigned char special)
{
  assert(values.size() == cart_points_.size());
  
  vcl_ofstream ofs(vrml_file_name.c_str());
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
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_points_.size(); i++) {
    vgl_vector_3d<double> ray = cart_points_[i]-orig;
    if (values[i] == special)
      bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, 0.6f, 1.0f, 1.0f, 0.0f);
    else 
      bvrml_write::write_vrml_disk(ofs, orig+10*ray, ray, 0.6f, 0.0f, 0.0f, values[i]/255.0f);
  }
  ofs.close();
}

//: generate panaroma image
//  create an image with width 360 and height 180 to pour all the ray values such that left most column is east direction, and the viewsphere is painted clockwise
void volm_spherical_shell_container::panaroma_img(vil_image_view<vxl_byte>& img, vcl_vector<unsigned char>& values)
{
  assert(values.size() == sph_points_.size());
  img.set_size(360, 180);
  for (unsigned i = 0; i < sph_points_.size(); i++) {
    vsph_sph_point_3d pt = sph_points_[i];
    unsigned ii = (unsigned)vcl_floor(vnl_math::angle_0_to_2pi(pt.phi_)*vnl_math::one_over_pi*180);
    unsigned jj = (unsigned)vcl_floor(vnl_math::angle_0_to_2pi(pt.theta_)*vnl_math::one_over_pi*180);
    img(ii,jj) = values[i];
  }
}

