#ifndef boxm2_vecf_orbit_param_stats_h_
#define boxm2_vecf_orbit_param_stats_h_
//:
// \file
// \brief  Assemble orbit data and compute statistics
//
// \author J.L. Mundy
// \date   5 July 2015
//
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vcl_map.h>
#include "boxm2_vecf_orbit_params.h"
#include <vnl/vnl_matrix.h>
class boxm2_vecf_orbit_param_stats{
 public:
  boxm2_vecf_orbit_param_stats(){}
 boxm2_vecf_orbit_param_stats(vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > > const& param_map):
  param_map_(param_map){}

  void add_param_pair(vcl_string patient_id, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params> const& param_pair){
    param_map_[patient_id] = param_pair;
  }
  void generate_stats();
  void print_stats();
  void print_xy_fitting_error();
  void print_xyz_fitting_error();
  bool merge_margins_and_crease();
  bool plot_merged_margins(vcl_ofstream& os, unsigned sample_skip = 3);
  void compute_feature_vectors();
  void compute_covariance_matrix();
  void separation_stats();
 private:
  void average_params();
  vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > > param_map_;
  vcl_map<vcl_string, boxm2_vecf_orbit_params> param_avg_;
  //      patient id          parameter   deviation
  vcl_map<vcl_string, vcl_map<vcl_string, double > > devs_;
  vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > > merged_inf_margin_;
  vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > > merged_sup_margin_;
  vcl_map<vcl_string, vcl_vector<vgl_point_3d<double> > > merged_crease_;
  vcl_map<vcl_string, vnl_matrix<double> > feature_vectors_;
  vnl_matrix<double> mean_;
  vnl_matrix<double> cov_;
};
#endif// boxm2_vecf_orbit_param_stats
