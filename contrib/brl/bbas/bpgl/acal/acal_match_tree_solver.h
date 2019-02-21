// This is//external/acal/acal_match_tree_solver.h
#ifndef acal_match_tree_solver_h
#define acal_match_tree_solver_h

//:
// \file
// \brief A non-linear optimizer for solving for camera translations
// \author J.L. Mundy
// \date Nov 28, 2018
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
#include <vnl/vnl_least_squares_function.h>
#include "acal_match_tree.h"
#include "acal_match_graph.h"


class solution_error
{
 public:
  solution_error():min_eps_u_(0.0), min_eps_v_(0.0), max_eps_u_(0.0), max_eps_v_(0.0), rms_err_u_(0.0),rms_err_v_(0.0){}
  solution_error(double min_eps_u, double min_eps_v, double max_eps_u, double max_eps_v,  double rms_err_u,  double rms_err_v):
  min_eps_u_( min_eps_u), min_eps_v_(min_eps_v), max_eps_u_(max_eps_u), max_eps_v_(max_eps_v), rms_err_u_(rms_err_u),rms_err_v_(rms_err_v){}
  double min_eps_u_;
  double min_eps_v_;
  double max_eps_u_;
  double max_eps_v_;
  double rms_err_u_;
  double rms_err_v_;
  void print(){
    std::cout << "min (" << min_eps_u_ << ' ' << min_eps_v_ << ")" << std::endl;
    std::cout << "max (" << max_eps_u_ << ' ' << max_eps_v_ << ")" << std::endl;
    std::cout << "rms (" << rms_err_u_ << ' ' << rms_err_v_ << ")" << std::endl;
  }
  double max_err() {return max_eps_u_ > max_eps_v_ ? max_eps_u_ : max_eps_v_;}
  double min_err() {return min_eps_u_ < min_eps_v_ ? min_eps_u_ : min_eps_v_;}
  double total_rms() {return rms_err_u_ + rms_err_v_;}
};


class acal_match_tree_lsqr : public vnl_least_squares_function
{
 public:
  //: Default constructor, use to define variable for assignment
  acal_match_tree_lsqr() : vnl_least_squares_function(0,0){}

  //: Constructor
  acal_match_tree_lsqr(std::map<size_t, vpgl_affine_camera<double> >& tree_acams,
                        std::vector< std::map<size_t, vgl_point_2d<double> > > tracks,
                        size_t n_residuals,  double cam_trans_penalty):
    vnl_least_squares_function(2*tree_acams.size(), n_residuals, vnl_least_squares_function::use_gradient),
    tree_acams_(tree_acams), trans_acams_(tree_acams), tracks_(tracks),
    verbose_(false), track_intersect_failure_(false),
    cam_trans_penalty_(cam_trans_penalty){}

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
  acal_match_tree_solver(): verbose_(false), cam_trans_penalty_(0.05),conn_comp_index_(-1){}

  acal_match_tree_solver(acal_match_graph const& match_graph, size_t conn_comp_index, double cam_trans_penalty = 0.05):
    match_graph_(match_graph), verbose_(false),
    cam_trans_penalty_(cam_trans_penalty), conn_comp_index_(conn_comp_index)
  {
    match_graph_.compute_match_trees();
    match_graph_.validate_match_trees_and_set_metric();
    match_tree_ = match_graph_.largest_tree(conn_comp_index);
    tracks_ = match_tree_->tracks();
    std::map<size_t, vpgl_affine_camera<double> >& acams = match_graph_.all_acams();
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
  bool solve_least_squares_problem();

  //      cam_id         translation
  std::map<size_t, vgl_vector_2d<double> > translations(){ return translations_;}

  // transfered from least squares function - valid if f is called after convergence
  //      track idx     triangulated pt
  std::map<size_t, vgl_point_3d<double> >  track_3d_points(){return track_3d_points_;}

  std::map<size_t, solution_error>  solution_errors(){return sol_errors_;}

  std::shared_ptr<acal_match_tree> match_tree(){return match_tree_;}

  std::map<size_t, vpgl_affine_camera<double> > adjusted_acams() {return adjusted_acams_;}

  // ===== for debug purposes =====
  bool input_data(std::string const& fmatches_path, std::string const& affine_cam_path);
  bool init(size_t conn_comp_index);
  void print_solution();
  bool save_tree_in_dot_format(std::string path);

 private:
  bool verbose_;
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
  std::map<size_t, solution_error> sol_errors_;
};

#endif
