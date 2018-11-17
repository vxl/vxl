#include <iostream>
#include <fstream>
#include "boxm2_vecf_point_cloud_orbit_viewer.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_plot_orbit.h"
std::vector<vgl_point_3d<double> >  boxm2_vecf_point_cloud_orbit_viewer::draw_sphere(vgl_sphere_3d<double> const& sph){
  std::vector<vgl_point_3d<double> > pts;
  // construct bounding box
  vgl_point_3d<double> c = sph.centre();
  double x0 = c.x(), y0 = c.y(), z0=c.z();
  double r = sph.radius();
  vgl_box_3d<double> bb;
  bb.add(vgl_point_3d<double>(x0-r, y0, z0));
  bb.add(vgl_point_3d<double>(x0+r, y0, z0));
  bb.add(vgl_point_3d<double>(x0, y0-r, z0));
  bb.add(vgl_point_3d<double>(x0, y0+r, z0));
  bb.add(vgl_point_3d<double>(x0, y0, z0-r));
  bb.add(vgl_point_3d<double>(x0, y0, z0+r));
  double inc = 0.2*r;
  vgl_point_3d<double> minp =  bb.min_point();
  vgl_point_3d<double> maxp =  bb.max_point();
  for(double x = minp.x(); x<=maxp.x(); x += inc)
    for(double y = minp.y(); y<=maxp.y(); y += inc)
      for(double z = minp.z(); z<=maxp.z(); z += inc){
        vgl_point_3d<double> p(x, y, z);
        if(sph.contains(p))
          pts.push_back(p);
      }
  return pts;
}
bool boxm2_vecf_point_cloud_orbit_viewer::set_point_cloud(std::string const& pc_path){
  std::ifstream istr(pc_path.c_str());
  if(!istr){
    std::cout << "invalid point cloud path " << pc_path << '\n';
    return false;
  }
  pc_.clear();
  double x, y, z;
  int r, g, b;
  while(istr >> x >> y >> z >> r >> g >> b){
    cpoint p(vgl_point_3d<double>(x, y, z), r, g, b);
    pc_.push_back(p);
  }
  istr.close();
  return true;
}
void boxm2_vecf_point_cloud_orbit_viewer::add_sphere(vgl_sphere_3d<double> const& sph, int r, int g, int b){
  std::vector<vgl_point_3d<double> > pts = this->draw_sphere(sph);
  for(auto & pt : pts){
    cpoint p(pt, r, g, b);
    pc_.push_back(p);
  }
}

bool boxm2_vecf_point_cloud_orbit_viewer::display_orbit(const boxm2_vecf_orbit_params& opr, bool is_right){
  double xm_min = opr.x_min()-10.0;
  double xm_max = opr.x_max()+10.0;
  std::vector<vgl_point_3d<double> > inf_pts, sup_pts, crease_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(opr, is_right, xm_min, xm_max, inf_pts);
  boxm2_vecf_plot_orbit::plot_superior_margin(opr, is_right, xm_min, xm_max, sup_pts);
  boxm2_vecf_plot_orbit::plot_crease(opr, is_right, xm_min, xm_max, crease_pts);
  int imin, imax;
  bool good = boxm2_vecf_plot_orbit::plot_limits(inf_pts, sup_pts, imin, imax);
  if(!good){
    std::cout << "determine plot limits failed \n";
    return false;
  }
  double vx = opr.x_trans(), vy = opr.y_trans(), vz = opr.z_trans();
  vgl_vector_3d<double> v(vx, vy, vz);
  double r = 1.0;
  // inf points are yellow, sup points red, crease points cyan
  for(int i = imin; i<=imax; ++i){
    // inf
    vgl_point_3d<double> pinf = inf_pts[i] + v;
    vgl_sphere_3d<double> sinf(pinf, r);
    this->add_sphere(sinf, 255, 255,0);
    // sup
    vgl_point_3d<double> psup = sup_pts[i] + v;
    vgl_sphere_3d<double> ssup(psup, r);
    this->add_sphere(ssup, 255, 0, 0);
    // crease
    vgl_point_3d<double> pcrease = crease_pts[i] + v;
    vgl_sphere_3d<double> screase(pcrease, r);
    this->add_sphere(screase, 0, 255, 255);
  }
  return true;
}
bool boxm2_vecf_point_cloud_orbit_viewer::save_point_cloud(std::string const& pc_path) const{
  std::ofstream ostr(pc_path.c_str());
  if(!ostr){
    std::cout << "invalid point clould path " << pc_path << '\n';
    return false;
  }
  for(const auto & p : pc_){
    ostr << p.pt_.x() <<' '<< p.pt_.y()<<' '<< p.pt_.z()<<' '<< p.r_ <<' '<< p.g_<<' '<< p.b_<< '\n';
  }
  ostr.close();
  return true;
}
