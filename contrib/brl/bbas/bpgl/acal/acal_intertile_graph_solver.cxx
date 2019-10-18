#include "acal_intertile_graph_solver.h"
#include <limits>
#include <math.h>
//for sorting tile residuals to obtain best tiles for the solver
static bool residual_less(std::pair<double, std::pair<size_t, std::set<size_t> > > const& a,
                          std::pair<double, std::pair<size_t, std::set<size_t> > > const& b){
  return a.first < b.first;
}
// the least squares cost function
void acal_intertile_solver_lsqr::f(vnl_vector<double> const& translations, vnl_vector<double>& residuals){
  for(std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > >::iterator tlit =  tile_acams_.begin();
      tlit != tile_acams_.end(); ++tlit){
    size_t tindx = tlit->first;
    std::map<size_t, vpgl_affine_camera<double> >& cams = tlit->second;
    for(std::map<size_t, vpgl_affine_camera<double> >::iterator cit = cams.begin();
        cit != cams.end(); ++cit){
      size_t cam_id = cit->first;
      if(cam_id_to_trans_indx_.count(cam_id)>0){
        size_t trans_base_idx = cam_id_to_trans_indx_[cam_id];
        vnl_matrix_fixed<double, 3, 4> m = cit->second.get_matrix();
        m[0][3] += translations[2*trans_base_idx];
        m[1][3] += translations[2*trans_base_idx+1];
        if (fabs(translations[2 * trans_base_idx]) > 1.0e6 || fabs(translations[2 * trans_base_idx + 1]) > 1.0e6)
          int k = 0;
        translated_tile_acams_[tindx][cam_id].set_matrix(m);
      }
    }
  }
  // use adjusted cameras to compute residuals
  compute_residuals(translations,residuals);
}
// residuals consist of camera projection errors for non-seed cameras and a cost for large translations
void acal_intertile_solver_lsqr::compute_residuals(vnl_vector<double> const& x, vnl_vector<double>& residuals){
  residuals.fill(0.0);
  for(std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >::iterator tlitr = tracks_.begin();
      tlitr != tracks_.end(); ++tlitr){
    size_t tindx = tlitr->first;
    //     track idx   3d intersection point
    std::map<size_t, vgl_point_3d<double> > inter_pts;
    std::map<size_t, std::map<size_t, vgl_point_2d<double> > > proj_tracks;
    std::vector< std::map<size_t, vgl_point_2d<double> > >& trks = tlitr->second;
    // find the 3-d intersection point of for the tracks and return the resulting projected points for each camera
    if(! acal_f_utils::intersect_tracks_with_3d(translated_tile_acams_[tindx], trks, inter_pts, proj_tracks)){
      std::cout << "forcing huge residuals" << std::endl;
      residuals.fill(std::numeric_limits<double>::max());
      track_intersect_failure_ = true;
      return;
    }
    track_3d_points_[tindx] = inter_pts;
    for(std::map<size_t, std::map<size_t, vgl_point_2d<double> > >::iterator pit = proj_tracks.begin();
        pit != proj_tracks.end(); ++pit){
      size_t tridx = pit->first; //track index
      std::map<size_t, vgl_point_2d<double> > temp = pit->second;
      for(std::map<size_t, vgl_point_2d<double> >::iterator cit = temp.begin();
          cit != temp.end(); ++cit){
        size_t cam_id = cit->first;
        vgl_point_2d<double>& pt      = trks[tridx][cam_id];
        vgl_point_2d<double>& proj_pt = cit->second;
        size_t base_ridx = 2 * (tile_indices_to_residual_index_[tindx][tridx][cam_id]);
        // residuals based on the difference vector between the correspondence point and the projected 3-d point
        residuals[base_ridx] = pt.x()-proj_pt.x();
        residuals[base_ridx + 1] = pt.y()-proj_pt.y();
      }
    }
  }
  // also penalize camera translations
  size_t base_idx = 2 * n_proj_residuals_;
  for(std::map<size_t,  size_t>::iterator cit =cam_id_to_trans_indx_.begin();
      cit != cam_id_to_trans_indx_.end(); ++cit){
	  size_t c = cit->second;
    size_t cidx = 2*c;
    size_t ridx = base_idx + cidx;
    residuals[ridx] = cam_trans_penalty_*x[cidx];
    residuals[ridx+1] = cam_trans_penalty_*x[cidx+1];
  }
}
  // compute number of residuals and set map with respect to residual index
size_t acal_intertile_graph_solver::initialize_lsqr(std::vector<size_t> const& tile_indices, std::set<size_t>& unique_cams){
  size_t n_cams_tracks = 0;
  unique_cams.clear();
  tile_indices_to_residual_index_.clear();
  residual_index_to_tile_indices_.clear();
  trans_indx_to_cam_id_.clear();
  cam_id_to_trans_indx_.clear();
  n_proj_residuals_ = 0;
  for(std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >::iterator tile_it = tracks_.begin();
     tile_it != tracks_.end(); ++tile_it){
    std::vector<size_t> indx(3);
    size_t tile_idx = tile_it->first;
    indx[0] = tile_idx;
    if (tile_indices.size() > 0) {
      std::vector<size_t>::const_iterator trit;
      trit = std::find(tile_indices.begin(), tile_indices.end(), tile_idx);
      if(trit == tile_indices.end())//is tile_idx not found?
        continue; //yes, not a valid tile index
    }
    std::vector< std::map<size_t, vgl_point_2d<double> > >& trks = tile_it->second;
    size_t trindx = 0;
    for(std::vector< std::map<size_t, vgl_point_2d<double> > >::iterator vit = trks.begin();
        vit != trks.end(); ++vit, ++trindx){
      indx[1]=trindx;
      std::map<size_t, vgl_point_2d<double> >& track = (*vit);
      for(std::map<size_t, vgl_point_2d<double>>::iterator trk_it = track.begin();
          trk_it != track.end(); ++trk_it){
        size_t cam_id = trk_it->first;
        indx[2]=cam_id;
        unique_cams.insert(cam_id);
        tile_indices_to_residual_index_[tile_idx][trindx][cam_id] = n_cams_tracks;
        residual_index_to_tile_indices_.push_back(indx);
        n_cams_tracks++;//each track generates a proj image pt for each camera
      }
    }
    
  }
  n_proj_residuals_ = n_cams_tracks;
  // establish maps between indices of the translation parameter vector and the camera id
  size_t i = 0; 
  for(std::set<size_t>::iterator uit = unique_cams.begin();
      uit != unique_cams.end(); ++uit){
    size_t cid = *uit;
    std::vector<size_t>::iterator sit;
    sit = std::find(seed_cam_ids_.begin(), seed_cam_ids_.end(), cid);
    if(sit != seed_cam_ids_.end())//is cid is a seed cam?
      continue;//yes
    trans_indx_to_cam_id_[i]=cid;
    cam_id_to_trans_indx_[cid]=i++;
  }
  size_t n_unknowns = 2 * trans_indx_to_cam_id_.size();
  n_residuals_ = 2 * (n_cams_tracks + trans_indx_to_cam_id_.size());
  return n_unknowns;
}
// construct the levenberg_marquardt solver using the least squares cost function
// hard-coded solution parameters for now. Most values are the same as defaults of the vnl algorithm
void acal_intertile_graph_solver::construct_levmarq(acal_intertile_solver_lsqr& gc_lsq){
  levmarq_ = vnl_levenberg_marquardt(gc_lsq);
  levmarq_.set_verbose(true);
  // Set the x-tolerance.  Minimization terminates when the length of the steps taken in X (variables) are less than input x-tolerance
  levmarq_.set_x_tolerance(1e-13);
  // Set the epsilon-function.  This is the step length for FD Jacobian
  levmarq_.set_epsilon_function(0.01);
  // Set the f-tolerance.  Minimization terminates when the successive RSM errors are less then this
  levmarq_.set_f_tolerance(1e-15);
  // Set the maximum number of iterations
  levmarq_.set_max_function_evals(500);
}
// extract the rms residual for each tile,  residuals are for non-seed cameras
//                  rms error          tile         cam_ids
std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > > acal_intertile_graph_solver::cam_residuals(vnl_vector<double> const& residuals){
  std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > > ret;
  for(std::map<size_t, std::map<size_t, std::map<size_t, size_t> > >::iterator tlit = tile_indices_to_residual_index_.begin();
      tlit != tile_indices_to_residual_index_.end(); ++tlit){
    size_t tindx = tlit->first;
    std::map<size_t, std::map<size_t, size_t> >& trks = tlit->second;
    double sum_sq = 0;
    double nres = 0.0;
    std::set<size_t> low_residual_cam_ids;    
    for(std::map<size_t, std::map<size_t, size_t> >::iterator trit = trks.begin();
        trit != trks.end(); ++trit)
      for(std::map<size_t, size_t>::iterator cit = trit->second.begin();
          cit != trit->second.end(); ++cit){
        size_t cam_id = cit->first;
        std::vector<size_t>::iterator sit;
        sit = std::find(seed_cam_ids_.begin(), seed_cam_ids_.end(), cam_id);
        if(sit == seed_cam_ids_.end())//is cid is a seed cam?
          low_residual_cam_ids.insert(cam_id);//no
        size_t ridx = cit->second;
        sum_sq += residuals[2*ridx]*residuals[2*ridx];
        sum_sq += residuals[2*ridx+1]*residuals[2*ridx+1];
        nres += 2.0;
      }
    double rms = sqrt(sum_sq/nres);
    size_t n_low_res_cams = low_residual_cam_ids.size();
    std::pair<size_t, std::set<size_t> > pr(tindx, low_residual_cam_ids);
    ret.emplace_back(rms, pr);
  }
  return ret;
}
// the set of cam ids that have residuals below a threshold
// search stops when all non-seed cameras have been included
std::set<size_t> acal_intertile_graph_solver::
find_minimum_residual_set(std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > >& residual_cams, std::vector<size_t>& tile_indices){
  std::set<size_t> ret;
  size_t nr = residual_cams.size();
  size_t n_non_seed = trans_indx_to_cam_id_.size();
  tile_indices.clear();
  bool all_non_seed_reached = false;
  for(size_t i = 0; i<nr&&!all_non_seed_reached; ++i){
    std::set<size_t>& cam_ids = residual_cams[i].second.second;
    if (residual_cams[i].first <= max_tile_residual_) {
        tile_indices.push_back(residual_cams[i].second.first);
      for (std::set<size_t>::iterator cit = cam_ids.begin();
          cit != cam_ids.end(); ++cit)
          ret.insert(*cit);
    }
    all_non_seed_reached = ret.size() >= n_non_seed;
  }
  return ret;
}
// print the set of tiles and non-seed cameras ordered by rms projection error
void acal_intertile_graph_solver::print_cam_residuals(std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > >& residual_cams){
  std::cout << "<= UNIQUE CAMS vs. residual RMS =>"<< std::endl;
  std::set<size_t> unique_residual_cams;
  size_t nr = residual_cams.size();
  size_t n_non_seed = trans_indx_to_cam_id_.size();
  bool all_non_seed_reached = false;
  for (size_t i = 0; i < nr && !all_non_seed_reached; ++i) {
    std::set<size_t>& cam_ids = residual_cams[i].second.second;
    for(std::set<size_t>::iterator cit = cam_ids.begin();
        cit != cam_ids.end(); ++cit)
      unique_residual_cams.insert(*cit);
    all_non_seed_reached = unique_residual_cams.size() >= n_non_seed;
    std::cout << residual_cams[i].first << ' ' << residual_cams[i].second.first << ' ' << unique_residual_cams.size() << std::endl;
  }
  std::cout << "<= End UNIQUE CAMS vs. residual RMS =>"<< std::endl;
}
// the main process function. Two passes are executed. Later may apply more passes
bool acal_intertile_graph_solver::solve_least_squares_problem() {
  
  std::cout << "\n=====> Solve for cam translation(s)<=====" << std::endl;
  std::set<size_t> unique_cams;
  std::vector<size_t> tile_indices;
  size_t n_unknowns = initialize_lsqr(tile_indices, unique_cams);

  acal_intertile_solver_lsqr gc_lsq(tile_acams_, tracks_, tile_indices_to_residual_index_,
                                   n_unknowns, n_residuals_, n_proj_residuals_,
                                 trans_indx_to_cam_id_, cam_id_to_trans_indx_, cam_trans_penalty_);
  gc_lsq.set_verbose(verbose_);
  vnl_vector<double> translations(n_unknowns, 0.0);
  vnl_vector<double> residuals(n_residuals_, 0.0);

  //check if cameras are too bad to use
  gc_lsq.f(translations, residuals);
  if (gc_lsq.track_intersect_failed())
    return false;

  construct_levmarq(gc_lsq);
  // Minimize the error and get the best intersection point
  levmarq_.minimize(translations);
  levmarq_.diagnose_outcome();
  gc_lsq.f(translations, residuals);// final set of residuals and translations

  size_t n_non_seed = trans_indx_to_cam_id_.size();
  std::cout << "Total number of cameras " << unique_cams.size() << " n_seed "<< seed_cam_ids_.size() << " n_non_seed " << n_non_seed << std::endl;
  std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > >residual_cams;
  residual_cams = cam_residuals(residuals);
  std::sort(residual_cams.begin(), residual_cams.end(), residual_less);
  std::set<size_t> unique_res_cams = find_minimum_residual_set(residual_cams, tile_indices);
  if(verbose_) print_cam_residuals(residual_cams);
  std::cout << "FIRST PASS TRANSLATIONS" << std::endl;
  double sanity_thresh = 30.0;
  bool first_pass_fail = false;
  for(std::map<size_t, size_t>::iterator trit = trans_indx_to_cam_id_.begin();
      trit != trans_indx_to_cam_id_.end(); ++trit){
    size_t tidx = trit->first;
    size_t cam_idx = trit->second;
    double tu = translations(2 * tidx);
    double tv = translations(2 * tidx + 1);
    if ((fabs(tu) > sanity_thresh || fabs(tv) > sanity_thresh)) {
      std::cout << "solution failed on first pass - large translation (" << tu << ' ' << tv << ")" << std::endl;
     first_pass_fail = true;
    }
    vgl_vector_2d<double> trans(tu, tv);
    translations_[cam_idx] = trans;
    if(true) std::cout << cam_idx << ' ' << tu << ' ' << tv << std::endl;
  }
  //
  // 2nd pass after weeding out tiles with large residuals
  //
  residuals.clear();
  translations.clear();
  translations_.clear();
  n_unknowns = initialize_lsqr(tile_indices, unique_cams);
  acal_intertile_solver_lsqr gc_lsq2(tile_acams_, tracks_, tile_indices_to_residual_index_,
                                  n_unknowns, n_residuals_, n_proj_residuals_,
                                  trans_indx_to_cam_id_, cam_id_to_trans_indx_, cam_trans_penalty_);
  gc_lsq2.set_verbose(verbose_);
  translations.set_size(n_unknowns);  translations.fill(0.0);
  residuals.set_size(n_residuals_);   residuals.fill(0.0);
  // check if there are large projection errors
  gc_lsq2.f(translations, residuals);
  if (gc_lsq2.track_intersect_failed())
    return false;
  // Minimize the projection errors
  construct_levmarq(gc_lsq2);
  levmarq_.minimize(translations);
  levmarq_.diagnose_outcome();
  gc_lsq2.f(translations, residuals);// final set of residuals and translations  
  residual_cams = cam_residuals(residuals);
  unique_res_cams = find_minimum_residual_set(residual_cams, tile_indices);
  if(verbose_) print_cam_residuals(residual_cams);
  // transfer converged set of 3d points from least squares function
  track_3d_points_ = gc_lsq2.track_3d_points();
  bool second_pass_fail = false;

  if(verbose_)std::cout << "final translations:(cam idx tu  tv)" << std::endl;
  for(std::map<size_t, size_t>::iterator trit = trans_indx_to_cam_id_.begin();
      trit != trans_indx_to_cam_id_.end(); ++trit){
    size_t tidx = trit->first;
    size_t cam_idx = trit->second;
    double tu = translations(2 * tidx);
    double tv = translations(2 * tidx + 1);
    if ((fabs(tu) > sanity_thresh || fabs(tv) > sanity_thresh)) {
      std::cout << "solution failed on second pass - large translation (" << tu << ' ' << tv << ")" << std::endl;
      second_pass_fail = true;
    }
    vgl_vector_2d<double> trans(tu, tv);
    translations_[cam_idx] = trans;
    //if(verbose_) std::cout << cam_idx << ' ' << tu << ' ' << tv << std::endl;
  }
  if(second_pass_fail)
    return false;
  
  size_t n_cams = unique_res_cams.size();;
  if(verbose_){
    std::cout << n_cams << " solved cameras out of a total of " << trans_indx_to_cam_id_.size() << std::endl;
    std::cout << trans_indx_to_cam_id_.size()-n_cams << " cam solutions had projection errors exceeding " << max_tile_residual_ <<
              " pixels" << std::endl;
    }
  for(std::map<size_t, vgl_vector_2d<double> >::iterator trit = translations_.begin();
      trit != translations_.end(); ++trit){
    size_t cam_id = trit->first;
    double max_eps_u = 0.0, max_eps_v = 0.0;
    double sq_eps_u = 0, sq_eps_v = 0;
    double min_eps_u = std::numeric_limits<double>::max(), min_eps_v = min_eps_u;
    double n_res = 0.0;
    
    for(std::map<size_t, std::map<size_t, std::map<size_t, size_t> > >::iterator titr =tile_indices_to_residual_index_.begin();
        titr != tile_indices_to_residual_index_.end(); ++titr){
      size_t tile_indx = titr->first;
    std::map<size_t, std::map<size_t, size_t> >&  trks = tile_indices_to_residual_index_[tile_indx];
    for(std::map<size_t, std::map<size_t, size_t> >::iterator trit = trks.begin();
        trit != trks.end(); ++trit){
      std::map<size_t, size_t>& trk = trit->second;
      if (cam_id == 37) {
          std::cout << "37 track " << trk.size() << std::endl;
      }
      if(trk.count(cam_id)>0){
        n_res += 1.0;
        size_t res_indx = trk[cam_id];
        double eps_u = residuals[2*res_indx];
        double eps_v = residuals[2*res_indx+1];
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
    }
    }
    sq_eps_u /= n_res;
    sq_eps_v /= n_res;
    acal_solution_error asol(min_eps_u, min_eps_v, max_eps_u, max_eps_v, sqrt(sq_eps_u), sqrt(sq_eps_v));
    sol_errors_[cam_id] = asol;
  }
  if(verbose_) print_solution();
  return true;
}
std::map<size_t, std::string> acal_intertile_graph_solver::solved_inames(){
  std::map<size_t, std::string> ret;
  for(std::map<size_t, vgl_vector_2d<double> >::const_iterator mit =  translations_.begin();
      mit != translations_.end(); ++mit){
    size_t cam_idx = mit->first;
    ret[cam_idx]=cam_inames_[cam_idx];
  }
  return ret;
}
void acal_intertile_graph_solver::print_solution(){
  std::cout << "+++ [" << translations_.size() << "] Camera Translations +++" << std::endl;
  std::cout << "           image name                     cam_id  tx      ty             min     max      rms" << std::endl;
  std::cout.precision(4);
  for(std::map<size_t, vgl_vector_2d<double> >::const_iterator mit =  translations_.begin();
      mit != translations_.end(); ++mit){
    size_t cam_idx = mit->first;
    std::string name = cam_inames_[mit->first];
    std::cout << std::fixed << name << ' ' << cam_idx << " (" << mit->second.x() << ' ' << mit->second.y() << ")     \t"
              << sol_errors_[cam_idx].min_err() << '\t' << sol_errors_[cam_idx].max_err()
              << '\t' << sol_errors_[cam_idx].total_rms() << std::endl;
  }
}
