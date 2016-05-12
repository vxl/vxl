#include "bvgl_cross_section.h"
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_box_3d.h>

// motion vector is projected onto the plane in case it isn't already coplanar
bvgl_cross_section bvgl_cross_section::apply_vector( vgl_vector_3d<double> const& v) const{
  vgl_vector_3d<double> norm = plane_.normal();
  normalize(norm);//just in case
  double ndotv = dot_product(norm, v);
  vgl_vector_3d<double> temp = ndotv*norm;
  vgl_vector_3d<double> nv = v-temp;
  vgl_point_3d<double> pt_v = p_ + nv;
  unsigned n = ptset_.npts();
  std::vector<vgl_point_3d<double> > pts;
  std::vector<vgl_vector_3d<double> > normals;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i) + nv;
    pts.push_back(p);
    normals.push_back(ptset_.n(i));
  }
  vgl_pointset_3d<double> ptset_v(pts, normals);
  vgl_plane_3d<double> plane(plane_.normal(), pt_v);
  return bvgl_cross_section(t_, pt_v, plane, ptset_v);
}

bool bvgl_cross_section::contains(vgl_point_3d<double> const&p) const{
  return bbox_.contains(p);
}

vgl_point_3d<double> bvgl_cross_section::closest_point(vgl_point_3d<double> const&p, double dist_thresh) const{
  return vgl_closest_point(ptset_, p, dist_thresh);
}

void bvgl_cross_section::display_cross_section_pts(std::ofstream& ostr) const{
  unsigned n = ptset_.npts();
  float r = 3.0f;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i);
    vgl_point_3d<float> pf(static_cast<float>(p.x()), static_cast<float>(p.y()), static_cast<float>(p.z()));
    vgl_sphere_3d<float> sp(pf, r);
    bvrml_write::write_vrml_sphere(ostr, sp, 1.0f, 0.5f, 1.0f);
  }
}
void bvgl_cross_section::display_cross_section_plane( std::ofstream& ostr) const{
  vgl_vector_3d<double> norm = plane_.normal();
  float r = 10.0f, h = 0.1f;
  bvrml_write::write_vrml_cylinder(ostr, p_, norm, r, h, 0.0f, 1.0f, 1.0f, 1);
}

void bvgl_cross_section::display_cross_section_normal_disks(std::ofstream& ostr) const{
  unsigned n = ptset_.npts();
  float r = 3.0f, h = 0.1f;
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<double> p = ptset_.p(i);
    vgl_vector_3d<double> n = ptset_.n(i);
    bvrml_write::write_vrml_cylinder(ostr, p, n, r, h, 0.0f, 1.0f, 0.3f, 1);
  }
}
