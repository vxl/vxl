#include <limits>
#include <math.h>

#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_svd.h>

#include "acal_match_tree_solver.h"


void
acal_match_tree_lsqr_covar::f(vnl_vector<double> const& translations,
                        vnl_vector<double>& residuals)
{
  size_t n = tree_acams_.size();
  if(translations.size() != 2*n){
    for(size_t r = 0; r=residuals.size(); ++r)
      residuals[r] = std::numeric_limits<double>::max();
    track_intersect_failure_ = true;
    return ; // will eventually cause solution to fail
  }
  // translate the cameras according to the current translation solution
  size_t i =0;
  for(std::map<size_t, vpgl_affine_camera<double> >::iterator cit =  tree_acams_.begin();
      cit != tree_acams_.end(); ++cit, ++i){
    size_t cam_idx = cit->first;
    vnl_matrix_fixed<double, 3, 4> m = cit->second.get_matrix();
    m[0][3] += translations[2*i];
    m[1][3] += translations[2*i+1];
    trans_acams_[cam_idx].set_matrix(m);
  }
  // use adjusted cameras to compute residuals
  compute_residuals(translations,residuals);
}


void
acal_match_tree_lsqr_covar::gradf(vnl_vector<double> const& x,
                            vnl_matrix<double>& jacobian)
{
  // n columns of Jacobian is number of camera translation components = 2*n_cams
  // n rows of Jacobian is number of residuals
  // J[r][c] = d residual r / d trans component c
  // a residual is (u - pu) or (v - pv), where u is a x track coord and v is the y track coord
  // pu and pv are projected points from 3-d track intersection points.
  // so for example if a03 = a03 + tu , d(u-pu) = -1

  // initialize Jacobian to zero
  jacobian.fill(0.0);
  size_t tidx = 0;// translation parameter index
  size_t ncams = tree_acams_.size();
  for (std::vector< std::map<size_t, vgl_point_2d<double> > > ::iterator fit = tracks_.begin();
       fit != tracks_.end(); ++fit, tidx += (2 * ncams)) {
    for (size_t c = 0; c < ncams; ++c) {

      size_t idx_u = tidx + 2 * c;
      size_t idx_v = idx_u + 1;
      jacobian[idx_u][2 * c] = -1.0;
      jacobian[idx_v][2 * c + 1] = -1.0;
    }
  }
  for (size_t c = 0; c < ncams; ++c){
      size_t cidx_u = tidx + 2*c;
      size_t cidx_v = cidx_u + 1;
      jacobian[cidx_u][2*c] = 1.0;
      jacobian[cidx_v][2*c+1] = 1.0;
  }
}

void
acal_match_tree_lsqr_covar::compute_residuals(vnl_vector<double> const& x,
                                        vnl_vector<double>& residuals)
{
  double tol = 15.0;//increased from 10 7/3/2020 JLM - numerical gradient has larger residuals on first iteration
  // if a track doesn't project make the residual 0 so no penalty
  residuals.fill(0.0);
  //     track idx   3d intersection point
  std::map<size_t, vgl_point_3d<double> > inter_pts;
  //    track idx           cam_id     proj pt
  std::map<size_t, std::map<size_t, vgl_point_2d<double> > > proj_tracks;
  if(! acal_f_utils::intersect_tracks_with_3d(trans_acams_, tracks_, inter_pts, proj_tracks, ray_covariance_plane_cs_)){
    std::cout << "forcing huge residuals" << std::endl;
    residuals.fill(std::numeric_limits<double>::max());
    track_intersect_failure_ = true;
    return;
  }
  size_t n_cams = trans_acams_.size();
  track_3d_points_ = inter_pts;
  size_t ridx = 0;
  vnl_vector<double> temp_res(2*n_cams);
  for(std::map<size_t, std::map<size_t, vgl_point_2d<double> > >::iterator pit = proj_tracks.begin();
      pit != proj_tracks.end(); ++pit, ridx+=2*n_cams){
    size_t tidx = pit->first; //track index
    if (track_intersect_failure_&&verbose_) std::cout << "residual track id " << tidx << std::endl;
    std::map<size_t, vgl_point_2d<double> > temp = pit->second;
    size_t offset = 0;
    for(std::map<size_t, vgl_point_2d<double> >::iterator cit = temp.begin();
        cit != temp.end(); ++cit, offset+= 2){
      size_t cam_id = cit->first;
      vgl_point_2d<double>& pt      = tracks_[tidx][cam_id];
      vgl_point_2d<double>& proj_pt = cit->second;
      size_t idx = ridx+offset;
      temp_res[idx] = pt.x()-proj_pt.x();
      temp_res[idx + 1] = pt.y()-proj_pt.y();
      bool big_residuals = fabs(temp_res[idx]) > tol || fabs(temp_res[idx + 1]) > tol;
      if(verbose_ && big_residuals) {
      std::cout << "Warning residual exceeds tolerance " << tol << std::endl;
      }
      if(big_residuals&&verbose_) std::cout << temp_res[idx] << ' ' << temp_res[idx+1] << std::endl;
    }
  }
  if (track_intersect_failure_&&verbose_) {
    std::cout << "3D intersection points" << std::endl;
    for (std::map<size_t, vgl_point_3d<double> >::iterator pit = inter_pts.begin();
         pit != inter_pts.end(); ++pit)
      std::cout << pit->first << ' ' << pit->second.x() << ' ' << pit->second.y() << ' ' << pit->second.z() << std::endl;
  }
  // if there is a covariance matrix defined then apply the  Cholesky upper triangular matrix
  if(use_covariance_){
    vnl_vector<double> res_with_covar = sensor_inv_covar_cholesky_upper_tri_*temp_res;
    for(size_t i = 0; i<2*n_cams; ++i)
      residuals[i] = res_with_covar[i];
  }
  else 
    for (size_t i = 0; i < 2 * n_cams; ++i)
      residuals[i] = temp_res[i];
  
  size_t cam_offset = 2*tracks_.size()*tree_acams_.size();
  size_t cidx = 0;
  for(std::map<size_t, vpgl_affine_camera<double> >::iterator cit =  tree_acams_.begin();
      cit != tree_acams_.end(); ++cit, cam_offset +=2,cidx++){
    residuals[cam_offset] = cam_trans_penalty_*x[2*cidx];
    residuals[cam_offset+1] = cam_trans_penalty_*x[2*cidx+1];
    if(false&&verbose_) std::cout << residuals[cam_offset] << ' ' << residuals[cam_offset+1] << std::endl;
  }
}


bool
acal_match_tree_solver::init(size_t conn_comp_index)
{
  conn_comp_index_ = conn_comp_index;
  match_graph_.find_connected_components();
  match_graph_.compute_match_trees();
  match_graph_.validate_match_trees_and_set_metric();
  match_tree_ = match_graph_.largest_tree(conn_comp_index);
  if(!match_tree_)
    return false;
  tracks_ = match_tree_->tracks();
  std::map<size_t, vpgl_affine_camera<double> >& acams = match_graph_.all_acams_;
  std::vector<size_t> tree_cam_ids = match_tree_->cam_ids();
  for(std::vector<size_t>::iterator cit = tree_cam_ids.begin();
      cit != tree_cam_ids.end(); ++cit){
    if(acams.count(*cit) == 0){
      std::cout << "affine camera " << *cit << " not in match graph - fatal" << std::endl;
      return false;
    }
    tree_acams_[*cit] = acams[*cit];
  }
  n_residuals_ = 2*tree_acams_.size()*(tracks_.size()+1);
  return true;
}


bool
acal_match_tree_solver::input_data(std::string const& fmatches_path,
                                   std::string const& affine_cam_path)
{
  if(!match_graph_.load_from_fmatches(fmatches_path))
    return false;
  return match_graph_.load_affine_cams(affine_cam_path);
}


bool
acal_match_tree_solver::solve_least_squares_problem()
{
  std::cout << "\n=====> Solve for cam translation(s)<=====" << std::endl;

  acal_match_tree_lsqr_covar mt_lsq;
  // check if covariance is used for weighted least squares

  // initialize residual index and retrieve affine cameras
    // define track network for solver
  // solve full connected component
  // to make clear index corresponds to a connected component
  mt_lsq = acal_match_tree_lsqr_covar(tree_acams_, tracks_, n_residuals_, cam_trans_penalty_);
  if(use_covariance_)
    mt_lsq.set_covariance_info(sensor_inv_covar_cholesky_upper_tri_,
                               ray_covariance_plane_cs_);
  mt_lsq.set_verbose(verbose_);
  vnl_levenberg_marquardt levmarq(mt_lsq);
  levmarq.set_verbose(true);
  // Set the x-tolerance.  Minimization terminates when the length of the steps taken in X (variables) are less than input x-tolerance
  levmarq.set_x_tolerance(1e-13);
  // Set the epsilon-function.  This is the step length for FD Jacobian
  levmarq.set_epsilon_function(0.01);
  // Set the f-tolerance.  Minimization terminates when the successive RSM errors are less then this
  levmarq.set_f_tolerance(1e-15);
  // Set the maximum number of iterations
  levmarq.set_max_function_evals(500);
  vnl_vector<double> translations;
  translations.set_size(2 * (unsigned)tree_acams_.size());
  translations.fill(0.0);
  vnl_vector<double> residuals(n_residuals_, 0.0);
  mt_lsq.f(translations, residuals);
  if (mt_lsq.track_intersect_failed())
    return false;

  // Minimize the error and get the best intersection point
  levmarq.minimize(translations);
  levmarq.diagnose_outcome();
  mt_lsq.f(translations, residuals);// final set of residuals and translations
  // transfer converged set of 3d points from least squares function
  track_3d_points_ = mt_lsq.track_3d_points();
  adjusted_acams_ = mt_lsq.trans_acams();
  size_t it = 0;
  double sanity_thresh = 20.0;
  bool fail = false;
  if(verbose_)std::cout << "final translations:(cam idx tu  tv)" << std::endl;
  for (std::map<size_t, vpgl_affine_camera<double> >::iterator ait = tree_acams_.begin();
       ait != tree_acams_.end(); ++ait, ++it) {
    size_t cam_idx = ait->first;
    double tu = translations(2 * it);
    double tv = translations(2 * it + 1);
    if (fabs(tu) > sanity_thresh || fabs(tv) > sanity_thresh) {
      std::cout << "solution failed " << std::endl;
      fail = true;
    }
    vgl_vector_2d<double> trans(tu, tv);
    translations_[cam_idx] = trans;
    if(!fail&&verbose_) std::cout << cam_idx << ' ' << tu << ' ' << tv << std::endl;
  }
  if(!fail&&verbose_) std::cout << "mean sq track proj errors:(cam idx du  dv)" << std::endl;
  size_t n_cams = tree_acams_.size();
  size_t cc = 0;
  for (std::map<size_t, vpgl_affine_camera<double> >::iterator ait = tree_acams_.begin();
       ait != tree_acams_.end(); ++ait, ++cc) {
    size_t cidx = ait->first;
    double max_eps_u = 0.0, max_eps_v = 0.0;
    size_t n_tracks = tracks_.size();
    double sq_eps_u = 0, sq_eps_v = 0;
    double min_eps_u = std::numeric_limits<double>::max(), min_eps_v = min_eps_u;
    for (size_t t = 0; t < n_tracks; t++) {
      size_t offset = 2 * t*n_cams;
      double eps_u = residuals[offset + 2 * cc];
      double eps_v = residuals[offset + 2 * cc + 1];
      double ab_eps_u = fabs(eps_u);
      double ab_eps_v = fabs(eps_v);
      sq_eps_u += eps_u * eps_u;
      sq_eps_v += eps_v * eps_v;
      if (ab_eps_u > max_eps_u)
        max_eps_u = ab_eps_u;
      if (ab_eps_v > max_eps_v)
        max_eps_v = ab_eps_v;
      if (ab_eps_u < min_eps_u)
        min_eps_u = ab_eps_u;
      if (ab_eps_v < min_eps_v)
        min_eps_v = ab_eps_v;
    }
    sq_eps_u /= n_tracks;
    sq_eps_v /= n_tracks;

    sol_errors_[cidx] = acal_solution_error(min_eps_u, min_eps_v, max_eps_u, max_eps_v, sqrt(sq_eps_u), sqrt(sq_eps_v));
  }

  return !fail;
}


void
acal_match_tree_solver::print_solution()
{
  std::map<size_t, std::string> inames = match_graph_.image_names();
  std::cout << "+++ [" << translations_.size() << "] Camera Translations +++" << std::endl;
  std::cout << "           image name                                    cam_id     tx      ty      min       max      rms" << std::endl;
  for(std::map<size_t, vgl_vector_2d<double> >::const_iterator mit =  translations_.begin();
      mit != translations_.end(); ++mit){
    size_t cam_idx = mit->first;
    std::string name = inames[mit->first];
    std::cout << name << ' ' << cam_idx << " (" << mit->second.x() << ' ' << mit->second.y() << ") "
              << sol_errors_[cam_idx].min_err() << ' ' << sol_errors_[cam_idx].max_err()
              << ' ' << sol_errors_[cam_idx].total_rms() << std::endl;
  }
}


bool
acal_match_tree_solver::save_tree_in_dot_format(std::string path)
{
  return match_tree_->save_tree_dot_format(path);
}
