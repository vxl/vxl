#include "icam_cylinder_map.h"
#include <vil/vil_convert.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_closest_point.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
bool icam_cylinder_map::closest_camera(vgl_ray_3d<double> const& cyl_ray,
                                       vgl_point_3d<double> const& p,
                                       unsigned& cam_index,
                                       double& u, double& v) const{
  cam_index = 0;
//UNUSED  vgl_point_3d<double> co = cyl_ray.origin();
  vgl_vector_3d<double> dir = cyl_ray.direction();
  int imin = -1;
  unsigned i = 0;
  double dmin = vnl_numeric_traits<double>::maxval;
  double umin, vmin;
  unsigned end = images_.size();
  //unsigned end = 1;
  for(;i<end; ++i){
    if(!cams_[i]->is_class("vpgl_perspective_camera")&&
       !cams_[i]->is_class("vpgl_proj_camera"))
      return false;
    auto* cam = dynamic_cast<vpgl_proj_camera<double>*>(cams_[i].ptr());
    if(!cam) return false;
    // be sure the camera projects inside the corresponding image
    double ni = images_[i].ni(), nj = images_[i].nj();
    double ud=0.0, vd=0.0;
    cam->project(p.x(), p.y(), p.z(), ud, vd);
    if(ud<0.0||vd<0.0||ud>=ni||vd>=nj)
      continue;
    vgl_homg_point_3d<double> hc = cam->camera_center();
    vgl_point_3d<double> c(hc);
    //make sure the camera ray is opposite in sense to the outward cylinder ray
    vgl_vector_3d<double> cam_ray = p - c;
    cam_ray=normalize(cam_ray);
    double dp = dot_product(dir, cam_ray);
    if(dp>0)
      continue;
#if 0
    vgl_point_3d<double> close_pt = vgl_closest_point(c, cyl_ray);
    double d = (close_pt - c).length();
#endif
    double dr = cam_ray.length();
    double d = (1.0+dp)*dr;
    if(d<dmin){
      imin = i;
      dmin = d;
      umin = ud; vmin=vd;
    }
  }
  if(imin<0) return false;
  cam_index = imin;
  u = umin; v = vmin;
  return true;
}
bool icam_cylinder_map::map_cylinder(){
  cyl_map_.set_size(n_theta_, nz_, 3);
  cyl_map_.fill(0.0f);
  double height = upper_height_ + lower_height_;
  double ox=origin_.x(), oy=origin_.y(), oz = origin_.z();
  double dz = height/nz_;
  double dth = 2.0*vnl_math::pi/n_theta_;
  for(unsigned iz = 0; iz<nz_; ++iz){
    // determine ray origin
    double z = -lower_height_ + dz*iz + oz;
    vgl_point_3d<double> orig(ox, oy, z);
    for(unsigned ith = 0; ith<n_theta_; ++ith){
      //determine point on cylinder
      double th = ith*dth;
      double x = radius_*std::cos(th), y = radius_*std::sin(th);
      x+=ox; y+=oy;
      vgl_point_3d<double> pc(x, y, z);
      vgl_vector_3d<double> dir = pc-orig;
      dir = normalize(dir);
      vgl_ray_3d<double> ray(orig, dir);
      unsigned cam_index = 0;
      double u=0.0, v=0.0;
      if(!closest_camera(ray, pc, cam_index, u, v))
        continue;
      int ui = static_cast<int>(u), vi = static_cast<int>(v);
      for(int p = 0; p<3; ++p)
        cyl_map_(ith, iz, p) = images_[cam_index](ui, vi, p);
    }
      std::cout << '.';
  }
  std::cout << '\n';
  return true;
}

bool icam_cylinder_map::
render_map(vil_image_view<vxl_byte>const& backgnd,
           vpgl_camera_double_sptr const& cam,
           double theta, vil_image_view<vxl_byte>& img,
           double scale,
           float back_r, float back_g, float back_b)
{
  double twopi = 2.0*vnl_math::pi;
  //requires perspective camera with calibration matrix
  if(!cam->is_class("vpgl_perspective_camera"))
    return false;
  auto* pcam = dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr());
  if(!pcam) return false;
  //get the principal ray of the camera
  vgl_vector_3d<double> pvec = pcam->principal_axis();
  pvec = normalize(pvec);
  // get image size from camera principal point
  vpgl_calibration_matrix<double> K = pcam->get_calibration();
  vgl_point_2d<double> pp = K.principal_point();
  double pu = pp.x(), pv =pp.y();
  auto ni = static_cast<unsigned>(2.0*pu*scale),
    nj = static_cast<unsigned>(2.0*pv*scale);
  img.set_size(ni, nj, 3);
  bool use_backgnd = backgnd.ni()>0;
  for (unsigned int j = 0; j<nj; ++j)
    for (unsigned int i = 0; i<ni; ++i)
      if (use_backgnd)
        for (unsigned int p = 0; p<3; ++p)
          img(i, j, p)= backgnd(i,j,p);
      else
        {
        img(i,j,0) = back_r;
        img(i,j,1) = back_g;
        img(i,j,2) = back_b;
        }
  //scan the cylinder
  double height = upper_height_ + lower_height_;
  double ox=origin_.x(), oy=origin_.y(), oz = origin_.z();
  double dz = height/nz_;
  double dth = twopi/n_theta_;
  for (unsigned int iz = 0; iz<nz_; ++iz)
    {
    // determine ray origin
    double z = -lower_height_ + dz*iz + oz;
    vgl_point_3d<double> orig(ox, oy, z);
    for (unsigned ith = 0; ith<n_theta_; ++ith)
      {
      //determine point on cylinder
      double th = ith*dth;
      double tth = th +theta;
      if(tth>=twopi)
        tth -= twopi;
      if(tth<0.0)
        tth += twopi;
      int kth = static_cast<int>(tth/dth);
      double x = radius_*std::cos(th), y = radius_*std::sin(th);
      x+=ox; y+=oy;
      vgl_point_3d<double> pc(x, y, z);
      vgl_vector_3d<double> dir = pc-orig;
      dir = normalize(dir);
      double dp = dot_product(dir, pvec);
      if(dp>0.0)
        continue;
      double ud, vd;
      cam->project(x, y, z, ud, vd);
      ud -= pu; vd -= pv;
      ud *= scale;       vd *= scale;
      ud += pu*scale; vd += pv*scale;
      if (ud<0.0||vd<0.0||ud>=ni||vd>=nj)
        continue;
      auto u = static_cast<unsigned>(ud+0.5), v = static_cast<unsigned>(vd+0.5);
      // interpolate in a 2x2 neighborhood
      for(unsigned int p = 0; p<3; ++p)
        {
        double sum = 0.0;
        sum += cyl_map_(kth, iz, p);
        double w = 1.0;
        if((kth+1)<n_theta_)
          {
          sum += 0.25*cyl_map_(kth+1, iz,p);
          w+=0.25;
          }
        if((kth-1)>=0)
          {
          sum += 0.25*cyl_map_(kth-1, iz,p);
          w+=0.25;
          }
        if(iz < (nz_-1))
          {
          sum += 0.25*cyl_map_(kth, iz+1,p);
          w+=0.25;
          }
        if(iz != 0)
          {
          sum += 0.25*cyl_map_(kth, iz-1,p);
          w+=0.25;
          }
        img(u, v, p) = static_cast<vxl_byte>(sum/w);
        }
      }
    }
 return true;
}
