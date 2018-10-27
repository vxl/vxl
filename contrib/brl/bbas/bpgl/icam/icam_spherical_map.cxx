#include "icam_spherical_map.h"
#include <vil/vil_convert.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_closest_point.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
bool icam_spherical_map::closest_camera(vgl_ray_3d<double> const& sph_ray,
                                       vgl_point_3d<double> const& p,
                                        unsigned& cam_index, double& dot_prod,
                                        double& u, double& v
                                        ) const{
  cam_index = 0;
//UNUSED  vgl_point_3d<double> co = sph_ray.origin();
  vgl_vector_3d<double> dir = sph_ray.direction();//dir is unit vector
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
    // be sure the camera projects inside the image to be evaluated
    double ni = images_[i].ni(), nj = images_[i].nj();
    double ud=0.0, vd=0.0;
    cam->project(p.x(), p.y(), p.z(), ud, vd);
    if(ud<0.0||vd<0.0||ud>=ni||vd>=nj)
      continue;
    vgl_homg_point_3d<double> hc = cam->camera_center();
    vgl_point_3d<double> c(hc);
    //make sure the camera ray is opposite in sense to the outward sphere ray
    vgl_vector_3d<double> cam_ray = p - c;
    vgl_vector_3d<double> norm_cam_ray = normalize(cam_ray);
    double dp = dot_product(dir, norm_cam_ray);
    if(dp>0)
      continue;
    double dr = cam_ray.length();
    double d = (1.0+dp)*dr;// dr was always 1 before adding norm_cam_ray
    if(d<dmin){
      imin = i;
      dmin = d;
      umin = ud; vmin=vd;
      dot_prod = dp;
    }
  }
  if(imin<0) return false;
  cam_index = imin;
  u = umin; v = vmin;
  return true;
}
bool icam_spherical_map::map_sphere(double angle_exponent,
                                    bool use_image,
                                    vxl_byte fg_r, vxl_byte fg_g,
                                    vxl_byte fg_b, vxl_byte bk_r,
                                    vxl_byte bk_g, vxl_byte bk_b){
  sph_map_.set_size(n_theta_, n_phi_, 3);
  sph_map_.fill(0);
  double ox=origin_.x(), oy=origin_.y(), oz = origin_.z();
  double dtheta = (vnl_math::pi-2.0*theta_cap_)/n_theta_;
  double dphi = 2.0*vnl_math::pi/n_phi_;
  for(unsigned ith = 0; ith<n_theta_; ++ith){
    // determine ray origin
    double theta = theta_cap_ + ith*dtheta;
    std::cout << '.';
    vgl_point_3d<double> orig(ox, oy, oz);
    double tr_theta = theta+t_theta_;
    for(unsigned iph= 0; iph<n_phi_; ++iph){
      //determine point on sphere
      double phi = iph*dphi;
      double tr_phi = phi + t_phi_;
      double si = std::sin(tr_theta);
      double x = radius_*si*std::cos(tr_phi), y = radius_*si*std::sin(tr_phi);
      double z = radius_*std::cos(tr_theta);
      x+=ox; y+=oy; z+=oz;
      vgl_point_3d<double> pc(x, y, z);
      vgl_vector_3d<double> dir = pc-orig;
      dir = normalize(dir);
      vgl_ray_3d<double> ray(orig, dir);
      unsigned cam_index = 0;
      double u=0.0, v=0.0, dot_prod = 1.0;
      if(!closest_camera(ray, pc, cam_index, dot_prod, u, v))
        continue;
      double fade = 1.0;
      if(angle_exponent !=0.0)
        fade = std::exp(-angle_exponent*(1.0-std::fabs(dot_prod)));
      int ui = static_cast<int>(u), vi = static_cast<int>(v);
      if(use_image){
        for(int p = 0; p<3; ++p)
          sph_map_(ith, iph, p) =
            static_cast<vxl_byte>(fade*images_[cam_index](ui, vi, p));
      }else{
        double vr = fade*fg_r + bk_r, vg = fade*fg_g + bk_g;
        double vb = fade*fg_b + bk_b;
        if(vr>255) vr = 255.0;
        if(vg>255) vg = 255.0;
        if(vb>255) vb = 255.0;
        sph_map_(ith, iph, 0) = static_cast<vxl_byte>(vr);
        sph_map_(ith, iph, 1) = static_cast<vxl_byte>(vg);
        sph_map_(ith, iph, 2) = static_cast<vxl_byte>(vb);
      }
    }
  }
  return true;
}

bool icam_spherical_map::
render_map(vil_image_view<vxl_byte>const& backgnd,
           vpgl_camera_double_sptr const& cam,
           double theta_off, double phi_off, vil_image_view<vxl_byte>& img,
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
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
      if(use_backgnd)
        for(unsigned p = 0; p<3; ++p)
        img(i, j, p)= backgnd(i,j,p);
      else{
        img(i,j,0) = back_r;
        img(i,j,1) = back_g;
        img(i,j,2) = back_b;
      }
  //scan the sphere
  double ox=origin_.x(), oy=origin_.y(), oz = origin_.z();
  double dtheta = (vnl_math::pi-2.0*theta_cap_)/n_theta_;
  double dphi = twopi/n_phi_;
  for(int ith = 0; ith<n_theta_; ++ith){
    double theta = theta_cap_ + ith*dtheta;
    double theta_map = theta_off + theta;
    if(theta_map<theta_cap_)
      continue;
    if(theta_map>(vnl_math::pi-theta_cap_))
      continue;
    int kth = static_cast<int>(theta_map/dtheta);
        if(kth>=n_theta_) kth = n_theta_-1;
    for(unsigned iph = 0; iph<n_phi_; ++iph){
      //determine point on sphere
      double phi = iph*dphi;
      double phi_map = phi_off + phi;
      if(phi_map>=twopi)
        phi_map -= twopi;
      int kphi = static_cast<int>(phi_map/dphi);
      double x = radius_*std::sin(theta)*std::cos(phi);
      double y = radius_*std::sin(theta)*std::sin(phi);
      double z = radius_*std::cos(theta);
      x+=ox; y+=oy; z += oz;
      vgl_point_3d<double> pc(x, y, z);
      vgl_vector_3d<double> dir = pc-origin_;
      dir = normalize(dir);
      double dp = dot_product(dir, pvec);
      if(dp>0.0)
        continue;
      double ud, vd;
      cam->project(x, y, z, ud, vd);
      ud -= pu; vd -= pv;
      ud *= scale;       vd *= scale;
      ud += pu*scale; vd += pv*scale;
      if(ud<0.0||vd<0.0||ud>=ni||vd>=nj)
        continue;
      auto u = static_cast<unsigned>(ud+0.5), v = static_cast<unsigned>(vd+0.5);
      for(unsigned p = 0; p<3; ++p){
        double sum = 0.0;
        sum += sph_map_(kth, kphi, p);
        double w = 1.0;
        if((kphi+1)<n_phi_){
          sum += 0.25*sph_map_(kth, kphi+1,p);
          w+=0.25;
        }
        if((kphi-1)>=0){
          sum += 0.25*sph_map_(kth, kphi-1,p);
          w+=0.25;
        }
        if((kth+1)<n_theta_){
          sum += 0.25*sph_map_(kth+1, kphi, p);
          w+=0.25;
        }
        if((kth-1)>=0){
          sum += 0.25*sph_map_(kth-1, kphi, p);
          w+=0.25;
        }
        img(u, v, p) = static_cast<vxl_byte>(sum/w);
      }
    }
  }
 return true;
}
