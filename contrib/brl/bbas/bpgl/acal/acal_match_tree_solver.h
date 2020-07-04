// This is//external/acal/acal_match_tree_solver.h
#ifndef acal_match_tree_solver_h
#define acal_match_tree_solver_h

//:
// \file
// \brief A non-linear optimizer for solving for camera translations
// \author J.L. Mundy
// \date July 3, 2020
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include "acal_f_utils.h"
#include <vpgl/vpgl_affine_camera.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_least_squares_function.h>
#include "acal_match_tree.h"
#include "acal_match_graph.h"
#include "acal_solution_error.h"
//
// July 3, 2020 - the original implementation used the gradf function, i.e. vnl_least_squares_function::use_gradient
// However an experiment where the numerical gradient is used yielded much smaller projection errors. The average rms
// projection error decreased from 0.35 to 0.02 pixels.So the current implementation numerically computes the Jacobian
// at each step. The original analytical Jacobian did not account for the variation in ray intersection point. There was
// no observed change in compute time. The resulting translations differ from the original implementations by ~0.5pix on average.
// 
class acal_match_tree_lsqr_covar : public vnl_least_squares_function
{
 public:
  //: Default constructor, use to define variable for assignment
   acal_match_tree_lsqr_covar() : vnl_least_squares_function(0, 0) { use_covariance_ = false; }

  //: Constructor
  acal_match_tree_lsqr_covar(std::map<size_t, vpgl_affine_camera<double> >& tree_acams,
                        std::vector< std::map<size_t, vgl_point_2d<double> > > tracks,
                        size_t n_residuals,  double cam_trans_penalty):
  vnl_least_squares_function(2*tree_acams.size(), n_residuals, vnl_least_squares_function::no_gradient),
    tree_acams_(tree_acams), trans_acams_(tree_acams), tracks_(tracks),
    verbose_(false), track_intersect_failure_(false),
      cam_trans_penalty_(cam_trans_penalty), use_covariance_(false){}

  //: provide covariance data derived from satellite pose errors, i.e. error in 
  // satellite position and orientation. The Levenberg-Marquardt algorithm minimizes
  // the sum of squares of a residual vector, f, i.e., finds the camera translations,
  //  x, that minimize f(x)^T f(x). Cholesky decomposition is applied to the inverse
  //  of the pose covariance matrix S to form S^-1 = L L^T. The residual vector is
  //  transformed as f_s(x) = L^T f(x) so that f_s(x)^T f_s(x) = f(x)^T S^-1 f(x), 
  //  i.e. weighted least squares.
  //
  // The covariance matrix in the coordinate systems of the planes orthogonal to each ray,
  // ray_covariance_plane_cs, is used by the ray intersection algorithm to compute a 
  // weighted least squares estimate of the triangulated point.
  // note that covariance is only applied if there is a single track.
  void set_covariance_info(vnl_matrix<double> sensor_inv_covar_cholesky_upper_tri,
                             vnl_matrix<double> const& ray_covariance_plane_cs){
    if(tracks_.size() != 1){
      std::cerr << "Warning attempt to apply weighted least squares to more than one track, reverting to unweighted solution"
                << std::endl;
      return;
    }
    sensor_inv_covar_cholesky_upper_tri_ = sensor_inv_covar_cholesky_upper_tri;
      ray_covariance_plane_cs_ = ray_covariance_plane_cs;
      use_covariance_ = true;
      use_gradient_ = vnl_least_squares_function::no_gradient;//symbolic Jacobian not used
  }

  //: The main function.
  //  Given the parameter vector translations, compute the vector of residuals, projection errors
  virtual void f(vnl_vector<double> const& translations,   // size is 2*cams.size() or 2 for single cam
                 vnl_vector<double>& residuals);  // size is 2*cams.size()*tracks.size() or 2*tracks.size() for single cam

  virtual void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

  void compute_residuals(vnl_vector<double> const& x, vnl_vector<double>& residuals);

  void set_verbose(bool verbose) {verbose_ = verbose;}

  bool track_intersect_failed() const {return track_intersect_failure_;}

  // 3-d points resulting from triangulation of track correspondences - computed on each iteration
  std::map<size_t, vgl_point_3d<double> >  track_3d_points(){return track_3d_points_;}

  // cameras translated by vector of unknowns x
  std::map<size_t, vpgl_affine_camera<double> > trans_acams() {return trans_acams_;}

 protected:
bool verbose_;
  bool use_covariance_;
  vnl_matrix<double> sensor_inv_covar_cholesky_upper_tri_;
  vnl_matrix<double> ray_covariance_plane_cs_;
  double cam_trans_penalty_;
  bool track_intersect_failure_;
  std::map<size_t, vpgl_affine_camera<double> > tree_acams_; //original affine cameras
  std::map<size_t, vpgl_affine_camera<double> > trans_acams_; //translated affine cameras
  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks_;
    //      track idx     triangulated pt
  std::map<size_t, vgl_point_3d<double> > track_3d_points_;
};


class acal_match_tree_solver
{
public:
 acal_match_tree_solver(): verbose_(false), cam_trans_penalty_(0.05),conn_comp_index_(-1),use_covariance_(false){}

  acal_match_tree_solver(acal_match_graph const& match_graph, size_t conn_comp_index, double cam_trans_penalty = 0.05):
  match_graph_(match_graph), verbose_(false),
    cam_trans_penalty_(cam_trans_penalty), conn_comp_index_(conn_comp_index), use_covariance_(false)
  {
    match_graph_.compute_match_trees();
    match_graph_.validate_match_trees_and_set_metric();
    match_tree_ = match_graph_.largest_tree(conn_comp_index);
    tracks_ = match_tree_->tracks();
    std::map<size_t, vpgl_affine_camera<double> >& acams = match_graph_.all_acams_;
    std::vector<size_t> tree_cam_ids = match_tree_->cam_ids();
    for(std::vector<size_t>::iterator cit = tree_cam_ids.begin();
        cit != tree_cam_ids.end(); ++cit){
      if(acams.count(*cit) == 0){
        std::cout << "affine camera " << *cit << " not in match graph - fatal" << std::endl;
        return;
      }
      tree_acams_[*cit] = acams[*cit];
    }
    n_residuals_ = 2*tree_acams_.size()*(tracks_.size()+1);
  }

  void set_verbose(bool verbose) { verbose_ = verbose; }
  void set_covariance_info(vnl_matrix<double> const& sensor_inv_covar_cholesky_upper_tri,
                               vnl_matrix<double> const& ray_covariance_plane_cs){
    sensor_inv_covar_cholesky_upper_tri_ = sensor_inv_covar_cholesky_upper_tri;
    ray_covariance_plane_cs_ = ray_covariance_plane_cs;
    use_covariance_ = true;
  }
  bool solve_least_squares_problem();

  //      cam_id         translation
  std::map<size_t, vgl_vector_2d<double> > translations(){ return translations_;}

  // transfered from least squares function - valid if f is called after convergence
  //      track idx     triangulated pt
  std::map<size_t, vgl_point_3d<double> >  track_3d_points(){return track_3d_points_;}

  std::map<size_t, acal_solution_error>  solution_errors(){return sol_errors_;}

  std::shared_ptr<acal_match_tree> match_tree(){return match_tree_;}

  std::map<size_t, vpgl_affine_camera<double> > adjusted_acams() {return adjusted_acams_;}

  // ===== for debug purposes =====
  bool input_data(std::string const& fmatches_path, std::string const& affine_cam_path);
  bool init(size_t conn_comp_index);
  void print_solution();
  bool save_tree_in_dot_format(std::string path);

 private:
  bool verbose_;
  bool use_covariance_;
  vnl_matrix<double> ray_covariance_plane_cs_;
  vnl_matrix<double> sensor_inv_covar_cholesky_upper_tri_;
  size_t conn_comp_index_;
  double cam_trans_penalty_;
  acal_match_graph match_graph_;
  std::shared_ptr<acal_match_tree> match_tree_;
  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks_;
  std::map<size_t, vpgl_affine_camera<double> > tree_acams_;
  std::map<size_t, vpgl_affine_camera<double> > adjusted_acams_;
  std::map<size_t, vgl_vector_2d<double> > translations_;
  size_t n_residuals_;
  //    track idx    triangulated pt
  std::map<size_t, vgl_point_3d<double> >  track_3d_points_;
  std::map<size_t, acal_solution_error> sol_errors_;
};

#endif
