// This is//external/acal/acal_intertile_graph_solver.h
#ifndef acal_intertile_graph_solver_h
#define acal_intertile_graph_solver_h

//:
// \file
// \brief A non-linear optimizer for solving for camera translations
// \author J.L. Mundy
// \date May 6, 2019
//
// \verbatim
//  Modifications
//   moved to acal October 14, 2019
// \endverbatim

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <bpgl/acal/acal_f_utils.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_least_squares_function.h>
#include <bpgl/acal/acal_match_tree_solver.h>
#include <bpgl/acal/acal_solution_error.h>
//
// the least squares cost function for the Levenberg-Marquardt solver. the function f(x, residuals)  evaluates a vector of residuals for the
// current estimate of camera translations, x. There are two types of residuals: 1) the difference between a projected 3-d point coordinate
// and the sift feature coordinate. 2) the translation assigned to the [0][3] or [1][3] element of the camera matrix. Residuals of type 2
// only defined for non-seed cameras, since seed camera translations are fixed and do not appear in the parameter vector, x.
//
class acal_intertile_solver_lsqr : public vnl_least_squares_function
{
 public:
  //: Default constructor, use to define variable for assignment
 acal_intertile_solver_lsqr() : vnl_least_squares_function(0,0){}
  //: Constructor
  //                              tile id         cam_id         camera
 acal_intertile_solver_lsqr(std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > const& tile_acams,
                            //       tile id       track            cam_id     image corr
                            std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > const& tracks,
                            //       tile id      track idx           cam_id   residual indx
                            std::map<size_t, std::map<size_t, std::map<size_t, size_t> > > tile_indices_to_residual_index,
                            // unknown translations  total residuals    num projection error vectors
                            size_t n_unknowns, size_t n_residuals, size_t n_proj_residuals,
                            //   trans indx  cam id
                            std::map<size_t, size_t>& trans_indx_to_cam_id,
                            //    cam id    trans indx                   weight for camera translation residuals
                            std::map<size_t, size_t>& cam_id_to_trans_indx, double cam_trans_penalty)
   : vnl_least_squares_function(n_unknowns, n_residuals, vnl_least_squares_function::no_gradient),
     tile_acams_(tile_acams), translated_tile_acams_(tile_acams), tracks_(tracks), tile_indices_to_residual_index_(tile_indices_to_residual_index),
     verbose_(false), track_intersect_failure_(false),n_proj_residuals_(n_proj_residuals),
     trans_indx_to_cam_id_(trans_indx_to_cam_id), cam_id_to_trans_indx_(cam_id_to_trans_indx),
     cam_trans_penalty_(cam_trans_penalty){}

  //: The main function.
  //  Given the parameter vector translations, compute the vector of residuals, projection errors
  virtual void f(vnl_vector<double> const& translations,  // size is 2*n_nonseed cameras
                 vnl_vector<double>& residuals);  // size is 2*(all_cams.size()*tracks.size()) + 2*n_nonseed_cameras

  void compute_residuals(vnl_vector<double> const& x, vnl_vector<double>& residuals);

  void set_verbose(bool verbose) {verbose_ = verbose;}

  bool track_intersect_failed() const {return track_intersect_failure_;}

  // 3-d points resulting from triangulation of track correspondences - computed on each iteration
  //      tile id         track indx  3-d intersection point
  std::map<size_t, std::map<size_t, vgl_point_3d<double> > > track_3d_points(){return track_3d_points_;}
  std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > translated_tile_acams(){
    return translated_tile_acams_;
  }
 protected:
  bool verbose_;
  double cam_trans_penalty_;
  bool track_intersect_failure_;
  size_t start_indx_cam_trans_residuals_;
  size_t n_proj_residuals_;
  size_t n_residuals_;

  //       tile             cam id           camera(cam_id)
  std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > tile_acams_;
  std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > translated_tile_acams_;

  //       tile          tracks          cam id          (u, v)
  std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > tracks_;

  //        tile           track idx      triangulated pt
  std::map<size_t, std::map<size_t, vgl_point_3d<double> > > track_3d_points_;

  //      tile_idx       track_idx          cam_id   residual index
  std::map<size_t, std::map<size_t, std::map<size_t, size_t> > > tile_indices_to_residual_index_;

  //residual indx   (tile_idx track_idx, cam_idx)
  std::vector<std::vector<size_t> > residual_index_to_tile_indices_;

  //residual indx   cam_id
  std::map<size_t, size_t> trans_indx_to_cam_id_;

  //     cam_id              trans_indx
  std::map<size_t, size_t> cam_id_to_trans_indx_;

  // tile_idx              cam_id   trans base
  std::map<size_t, std::map<size_t, size_t> > tile_indices_to_trans_indx_;
};

//
// == the process class for solving non-seed camera translations==
// this class sets up the Levenberg Marquardt algorithm and collects the solved translations
// as well as the residual errors. there are two solution passes where tiles with large projection
// residuals are detected and eliminated during the second pass. This strategy produces a significant
// reduction in the residual projection errors for the remaining tiles, albiet with potentially fewer
// corrected cameras
//
class acal_intertile_graph_solver {
public:
 acal_intertile_graph_solver(): verbose_(false), cam_trans_penalty_(0.05) {}

  //                                tile_id          cam_id       camera
 acal_intertile_graph_solver(std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > >tile_acams,
                             //       tile_id       track            cam_id        image corr
                             std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > tracks,
                             //       tile_id      non-seed cam_ids
                             std::map<size_t, std::vector<size_t> >  nonseed_cam_ids,
                             //  ids for seed cameras
                             std::vector<size_t> seed_cam_ids, std::map<size_t, std::string> cam_inames,
                             // residual penalty factor for cam offsets
                             double cam_trans_penalty = 0.05,
                             // max rms projection error for a tile
                             double max_tile_residual = 0.5)
   : tile_acams_(tile_acams), tracks_(tracks),seed_cam_ids_(seed_cam_ids), nonseed_cam_ids_(nonseed_cam_ids), cam_inames_(cam_inames),
     cam_trans_penalty_(cam_trans_penalty), verbose_(false), max_tile_residual_(max_tile_residual){}

  void set_verbose(bool verbose) { verbose_ = verbose; }
  bool solve_least_squares_problem();
  void print_cam_residuals(std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > >& residual_cams);
  void print_solution();

  // ===== output data =====
  //      cam_id         translation
  std::map<size_t, vgl_vector_2d<double> > translations() { return translations_;}
  //      cam_id         min, max and rms projection error
  std::map<size_t, acal_solution_error> solution_error() {return sol_errors_;}
  //      cam_id       solved_iname
  std::map<size_t, std::string> solved_inames();
  // ==== end output datainterface ====
 private:
  // internal functions
  size_t initialize_lsqr(std::vector<size_t> const& tile_indices, std::set<size_t>& unique_cams);
  vnl_levenberg_marquardt construct_levmarq(acal_intertile_solver_lsqr& gc_lsq);
  std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > > cam_residuals(vnl_vector<double> const& residuals);
  std::set<size_t> find_minimum_residual_set(std::vector<std::pair<double, std::pair<size_t, std::set<size_t> > > >& residual_cams,
                                              std::vector<size_t>& tile_indices);
  //members
  bool verbose_;
  double cam_trans_penalty_;
  double max_tile_residual_;
  //vnl_levenberg_marquardt levmarq_;

  size_t start_indx_cam_trans_residuals_;

  //      cam_id       iname
  std::map<size_t, std::string> cam_inames_;

  //       tile           cam_ids
  std::map<size_t, std::vector<size_t> >  nonseed_cam_ids_;

  std::vector<size_t> seed_cam_ids_;

  //       tile          tracks          cam id          (u, v)
  std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > tracks_;

  //       tile             cam id           camera(cam_id)
  std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > tile_acams_;
  std::map<size_t, std::map<size_t, vpgl_affine_camera<double> > > translated_tile_acams_;

  //      cam id         tu, tv
  std::map<size_t, vgl_vector_2d<double> > translations_;

  size_t n_proj_residuals_;  // number of residuals due to projection error (/2)
  size_t n_residuals_;  // total number of residuals (/2)

  // trans indx is the position of a camera translation in the vector of unknowns (/2)
  //     trans_indx           cam_id
  std::map<size_t, size_t> trans_indx_to_cam_id_;
  //     cam_id              trans_indx
  std::map<size_t, size_t> cam_id_to_trans_indx_;

  //        tile           track idx      triangulated pt
  std::map<size_t, std::map<size_t, vgl_point_3d<double> > > track_3d_points_;

  // residual index is the position in the vector of residuals for a given camera
  // in a given track in a given tile
  // residual index  (tile_idx,track_idx,cam_id)
  std::vector<std::vector<size_t> > residual_index_to_tile_indices_;

  //      tile_idx       track_idx          cam_id   residual index
  std::map<size_t, std::map<size_t, std::map<size_t, size_t> > > tile_indices_to_residual_index_;

  //       cam_id   error(cam_id)
  std::map<size_t, acal_solution_error> sol_errors_;
};
#endif
