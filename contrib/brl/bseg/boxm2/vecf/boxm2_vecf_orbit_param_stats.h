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
#include <vcl_map.h>
#include "boxm2_vecf_orbit_params.h"

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
 private:
  void average_params();
  vcl_map<vcl_string, vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params > > param_map_;
  vcl_map<vcl_string, boxm2_vecf_orbit_params> param_avg_;
  //      patient id          parameter   deviation
  vcl_map<vcl_string, vcl_map<vcl_string, double > > devs_;
};
#endif// boxm2_vecf_orbit_param_stats
