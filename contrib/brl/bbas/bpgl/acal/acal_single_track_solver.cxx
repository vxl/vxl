#include <limits>
#include <math.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/algo/vgl_intersection.h>
#include "acal_single_track_solver.h"
#include <vnl/algo/vnl_svd.h>
#define verbose_plus false

bool acal_single_track_solver::solve()
{
  std::cout << "\n=====> Solve for cam translation(s)<=====" << std::endl;

  std::vector<vgl_ray_3d<double> >  track_rays;
  for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track_.begin();
       cit != track_.end(); ++cit)
    {
      size_t cam_idx = cit->first;
      const vpgl_affine_camera<double>& cam = track_acams_[cam_idx];
      vgl_homg_point_2d<double> img_pt(cit->second);
      vgl_ray_3d<double> ray = cam.backproject_ray(img_pt);
      track_rays.push_back(ray);
    }
  if(track_rays.size()<2){
    std::cerr << "Insufficient number of rays - fail" << std::endl;
    return false;
  }
  if(!use_covariance_){
    if (!vgl_intersection(track_rays, track_3d_point_)){
      std::cerr << "Intersection failed - while not using covariance" << std::endl;
      return false;
    }
  }else{
    // condition large track solutions
    if(track_rays.size() > large_track_size_){
      std::cout << "Processing the large number of rays case ( " << track_rays.size() << " )"<< std::endl;
      vnl_svd<double> svd(covar_plane_cs_);
      size_t nr = covar_plane_cs_.rows();
      if(svd.rank() != nr){
        std::cout << "satellite pose covariance matrix is singular" <<std::endl;
        return false;
      }
      vnl_diag_matrix<double> W = svd.W();
      vnl_matrix<double> cond_covar = covar_plane_cs_;
      double max_singular_value = W[0];
      double min_singular_value = W[nr-1];
      // ill-conditioned problem metric
      double ill_cond = max_singular_value/min_singular_value;
      if(ill_cond > 100){
        std::cout << "Large ray satellite covar is ill-conditioned ( " << ill_cond << " )"<< std::endl;
        double cond_add = max_singular_value*max_sing_val_fraction_;
        for(size_t r = 0; r<nr; ++r)
          cond_covar[r][r] += cond_add;
        std::cout << "adding scaled identity matrix " << cond_add << " x I " << std::endl;
        vnl_svd<double> svd_add(cond_covar);
        std::cout << "After add condition is  " << 1.0/svd_add.well_condition() << std::endl;
      }
      if (!vgl_intersection(track_rays, cond_covar, track_3d_point_)){
        std::cerr << "Intersection failed - while using covariance on a large number of rays" << std::endl;
        return false;
      }
      else{
        std::cout << "Large number of rays intersection point (lvcs) " << track_3d_point_ << std::endl;
      }// end large number of rays
    }else if (!vgl_intersection(track_rays, covar_plane_cs_, track_3d_point_)){
      std::cerr << "Intersection failed - while using covariance" << std::endl;
      return false;
    }else if(verbose_plus){
      size_t n = track_rays.size();
      std::cout << " Rays " << std::endl;
      for (size_t i = 0; i < n; ++i) {
        std::cout << track_rays[i] << std::endl;
      }
      std::cout << "plane covariance\n"<< covar_plane_cs_ << std::endl;
    }// end verbose +
  }// end using satellite covariance
  size_t nr = track_rays.size();
  std::vector<double> sanity_thresholds(nr, 20.0);
  if(use_covariance_){
    for(size_t i = 0; i<nr; ++i){
      size_t r = 2*i, c = 2*i;
      double var = covar_plane_cs_[r][c];
      if(var>100.0)
        sanity_thresholds[i] = 50.0;
    }
  }
  vnl_vector<double> translations(2 * track_acams_.size());
  translations.fill(0.0);
  size_t it = 0;
  for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track_.begin();
       cit != track_.end(); ++cit, ++it)
    {
      size_t cam_idx = cit->first;
      const vpgl_affine_camera<double>& cam = track_acams_[cam_idx];
      vgl_point_2d<double> img_pt(cit->second);
      vgl_point_2d<double> proj_3d_pt = cam.project(track_3d_point_);
      vgl_vector_2d<double> t = img_pt - proj_3d_pt;
      translations[2*it] = t.x();
      translations[2*it+1] = t.y();
      vnl_matrix_fixed<double, 3, 4> m = cam.get_matrix();
      m[0][3] += translations[2*it];
      m[1][3] += translations[2*it+1];
      adjusted_acams_[cam_idx].set_matrix(m);
    }
  it = 0;
  std::vector<size_t> removed_cams;
  if(verbose_)std::cout << "final translations:(cam idx tu  tv)" << std::endl;
  for (std::map<size_t, vpgl_affine_camera<double> >::iterator ait = track_acams_.begin();
       ait != track_acams_.end(); ++ait, ++it) {
    size_t cam_idx = ait->first;
    double tu = translations(2 * it);
    double tv = translations(2 * it + 1);
    double sanity_thresh = sanity_thresholds[it];
    if (fabs(tu) > sanity_thresh || fabs(tv) > sanity_thresh) {
      std::cout << "solution for cam id " << cam_idx << " has too large a translation( "<< tu << ' ' << tv 
                << ") removing from solved cameras" << std::endl;
      removed_cams.push_back(cam_idx);
    }else{
    vgl_vector_2d<double> trans(tu, tv);
    translations_[cam_idx] = trans;
    if(verbose_) std::cout << cam_idx << ' ' << tu << ' ' << tv << std::endl;
    }
  }
  // errors should be zero but check to detect problems
  for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track_.begin();
       cit != track_.end(); ++cit, ++it){
    size_t cidx = cit->first;
    std::vector<size_t>::iterator rit = std::find(removed_cams.begin(), removed_cams.end(), cidx);
    if(rit != removed_cams.end())
      continue;
    double min_eps_u =0.0, min_eps_v = 0.0;
    double max_eps_u = 0.0, max_eps_v = 0.0;
    double sq_eps_u = 0, sq_eps_v = 0;
    const vpgl_affine_camera<double>& cam = adjusted_acams_[cidx];
    vgl_point_2d<double> img_pt(cit->second);
    vgl_point_2d<double> proj_3d_pt = cam.project(track_3d_point_);
    vgl_vector_2d<double>  er = (img_pt - proj_3d_pt);
    min_eps_u = er.x();
    max_eps_u = er.x();
    sq_eps_u = min_eps_u*min_eps_u;
    min_eps_v = er.y();
    max_eps_v = er.y();
    sq_eps_v = min_eps_v*min_eps_v;
    sol_errors_[cidx] = acal_solution_error(min_eps_u, min_eps_v, max_eps_u, max_eps_v, sqrt(sq_eps_u), sqrt(sq_eps_v));
  }
  return true;
}


void
acal_single_track_solver::print_solution()
{
  
  std::cout << "+++ [" << translations_.size() << "] Camera Translations +++" << std::endl;
  std::cout << "           image name                                    cam_id     tx      ty      min       max      rms" << std::endl;
  for(std::map<size_t, vgl_vector_2d<double> >::const_iterator mit =  translations_.begin();
      mit != translations_.end(); ++mit){
    size_t cam_idx = mit->first;
    std::string name = inames_[mit->first];
    std::cout << name << ' ' << cam_idx << " (" << mit->second.x() << ' ' << mit->second.y() << ") "
              << sol_errors_[cam_idx].min_err() << ' ' << sol_errors_[cam_idx].max_err()
              << ' ' << sol_errors_[cam_idx].total_rms() << std::endl;
  }
}
std::map<size_t, vgl_vector_2d<double> >
acal_single_track_solver::translations_with_specified_3d_pt(vgl_point_3d<double> const& pt_3d){
  std::map<size_t, vgl_vector_2d<double> > ret;
  for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track_.begin();
       cit != track_.end(); ++cit)
    {
      size_t cam_idx = cit->first;
      const vpgl_affine_camera<double>& cam = track_acams_[cam_idx];
      vgl_point_2d<double> img_pt(cit->second);
      vgl_point_2d<double> proj_3d_pt = cam.project(pt_3d);
      vgl_vector_2d<double> t = img_pt - proj_3d_pt;
      ret[cam_idx] = t;
    }
  return ret;
}

