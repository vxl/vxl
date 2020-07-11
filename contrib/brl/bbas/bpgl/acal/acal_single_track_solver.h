// This is//external/acal/acal_single_track_solver.h
#ifndef acal_single_track_solver_h
#define acal_single_track_solver_h

//:
// \file
// \brief Covariance-enabled solver for a single track
// \author J.L. Mundy
// \date July 10, 2020
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
#include "acal_match_tree.h"
#include "acal_match_graph.h"
#include "acal_solution_error.h"
class acal_single_track_solver
{
public:
  acal_single_track_solver(): verbose_(false){}

 acal_single_track_solver(std::map<size_t, std::string> inames,std::map<size_t, vgl_point_2d<double> > track,std::map<size_t, vpgl_affine_camera<double> > const& acams):
  inames_(inames), track_(track), track_acams_(acams),verbose_(false), use_covariance_(false){}
  void set_verbose(bool verbose) { verbose_ = verbose; }
  void set_covar_plane_cs(vnl_matrix<double> const& covar){
    covar_plane_cs_ = covar;
    use_covariance_ = true;
  }
  // the main solution method
  bool solve();

  //      cam_id         translation
  std::map<size_t, vgl_vector_2d<double> > translations(){ return translations_;}

  // track intersection point
   vgl_point_3d < double> track_3d_point(){return track_3d_point_;}

   std::map < size_t, acal_solution_error>  solution_errors(){return sol_errors_;}

   std::map<size_t, vpgl_affine_camera<double> > adjusted_acams() {return adjusted_acams_;}

  // ===== for debug purposes =====
   std::map<size_t, vgl_vector_2d<double> > translations_with_specified_3d_pt(vgl_point_3d<double> const& pt_3d);
   void print_solution();
 

 private:
  bool verbose_;
  bool use_covariance_;
  vnl_matrix<double> covar_plane_cs_;
  std::map<size_t, std::string> inames_;
  std::map<size_t, vgl_point_2d<double> > track_;
  std::map<size_t, vpgl_affine_camera<double> > track_acams_;
  std::map<size_t, vpgl_affine_camera<double> > adjusted_acams_;
  std::map<size_t, vgl_vector_2d<double> > translations_;
  vgl_point_3d<double> track_3d_point_;
  std::map<size_t, acal_solution_error> sol_errors_;
};

#endif
