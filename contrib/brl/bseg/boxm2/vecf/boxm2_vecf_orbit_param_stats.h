#ifndef boxm2_vecf_orbit_param_stats_h_
#define boxm2_vecf_orbit_param_stats_h_
//:
// \file
// \brief  Assemble orbit data and compute statistics
//
// \author J.L. Mundy
// \date   5 July 2015
//
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_orbit_params.h"
#include <vnl/vnl_matrix.h>
class boxm2_vecf_orbit_param_stats{
 public:
  boxm2_vecf_orbit_param_stats()= default;
 boxm2_vecf_orbit_param_stats(std::map<std::string, std::pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > >  param_map):
  param_map_(std::move(param_map)){}

  void add_param_pair(std::string patient_id, std::pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params> const& param_pair){
    param_map_[patient_id] = param_pair;
  }
  void generate_stats();
  void print_stats();
  void print_xy_fitting_error();
  void print_xyz_fitting_error();
  bool merge_margins_and_crease();
  bool plot_merged_margins(std::ofstream& os, unsigned sample_skip = 3);
  void compute_feature_vectors();
  void compute_covariance_matrix();
  void separation_stats();
 private:
  void average_params();
  std::map<std::string, std::pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > > param_map_;
  std::map<std::string, boxm2_vecf_orbit_params> param_avg_;
  //      patient id          parameter   deviation
  std::map<std::string, std::map<std::string, double > > devs_;
  std::map<std::string, std::vector<vgl_point_3d<double> > > merged_inf_margin_;
  std::map<std::string, std::vector<vgl_point_3d<double> > > merged_sup_margin_;
  std::map<std::string, std::vector<vgl_point_3d<double> > > merged_crease_;
  std::map<std::string, vnl_matrix<double> > feature_vectors_;
  vnl_matrix<double> mean_;
  vnl_matrix<double> cov_;
};
#endif// boxm2_vecf_orbit_param_stats
