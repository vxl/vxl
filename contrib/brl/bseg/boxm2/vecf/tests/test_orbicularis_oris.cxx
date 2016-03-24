// This is contrib/brl/bseg/boxm2/vecf/test/test_orbicularis_oris.cxx
//:
// \file
// \author Yi Dong
// \date January, 2016

#include <string>
#include <fstream>
#include <iostream>
#include <testlib/testlib_test.h>
#include <boxm2/vecf/boxm2_vecf_orbicularis_oris_params.h>
#include <boxm2/vecf/boxm2_vecf_orbicularis_oris.h>


bool test_orbicularis_oris_params()
{
  boxm2_vecf_orbicularis_oris_params pr;
  pr.principal_eigenvector_1_.set(1.0, 1.0, 1.0);
  pr.principal_eigenvector_2_.set(2.0, 0.0, 1.0);
  pr.lambda_ = 0.8;
  pr.gamma_ = 0.5;
  pr.planar_surface_dist_thresh_ = 2.0;
  pr.tilt_angle_in_deg_ = 1.0;
  pr.scale_factor_ = 1.0;
  std::string param_file = "./boxm2_vecf_orbicularis_oris_params.txt";
  std::ofstream ostr(param_file.c_str());
  ostr << pr;
  ostr.close();
  // read in
  boxm2_vecf_orbicularis_oris_params pr_in;
  std::ifstream istr(param_file.c_str());
  istr >> pr_in;
  istr.close();
  bool good = pr_in.principal_eigenvector_1_ == pr.principal_eigenvector_1_;
  good     &= pr_in.principal_eigenvector_2_ == pr.principal_eigenvector_2_;
  good     &= std::fabs(pr_in.lambda_ - pr.lambda_) < 1E-5;
  good     &= std::fabs(pr_in.gamma_ - pr.gamma_) < 1E-5;
  return good;
}

bool test_orbicularis_oris_doformation(std::string const& geometry_file, std::string const& param_file)
{
  std::string base_dir = vul_file::dirname(geometry_file);
  boxm2_vecf_orbicularis_oris oom(geometry_file);
  std::cout << "number of knots along axis: " << oom.axis().knots().size() << std::endl;
  std::cout << "number of cross sections: " << oom.n_cross_sections() << std::endl;

  // set the parameters
  boxm2_vecf_orbicularis_oris_params pr;
  std::ifstream istr(param_file.c_str());
  istr >> pr;
  istr.close();

  std::string init_axis_wrl  = base_dir + "/initial_axis.wrl";
  std::string init_pts_wrl   = base_dir + "/initial_pts.wrl";
  std::string init_cross_wrl = base_dir + "/initial_cross.wrl";
  std::string init_axis_file = base_dir + "/initial_axis.txt";
  std::string init_pts_file  = base_dir + "/initial_pts.txt";

  std::ofstream aostr_init(init_axis_file.c_str());
  oom.display_axis_spline_ascii(aostr_init);
  std::ofstream postr_init(init_pts_file.c_str());
  oom.display_cross_section_pointsets_ascii(postr_init);
  std::ofstream axis_ofs(init_axis_wrl.c_str());
  oom.display_axis_spline(axis_ofs);
  std::ofstream pts_ofs(init_pts_wrl.c_str());
  oom.display_cross_section_pointsets(pts_ofs);
  std::ofstream cross_ofs(init_cross_wrl.c_str());
  oom.display_cross_section_planes(cross_ofs);


  // test scale operation
  // enlarge
  boxm2_vecf_orbicularis_oris_params pr_scale = pr;
  pr_scale.scale_factor_ = 1.5;
  oom.set_params(pr_scale);
  boxm2_vecf_orbicularis_oris oom_large = oom.scale();

  std::string large_axis_wrl  = base_dir + "/enlarge_axis.wrl";
  std::string large_cross_wrl = base_dir + "/enlarge_cross.wrl";
  std::string large_pts_wrl   = base_dir + "/enlarge_pts.wrl";
  std::string large_axis_file = base_dir + "/enlarge_axis.txt";
  std::string large_pts_file  = base_dir + "/enlarge_pts.txt";
  std::ofstream large_axis_ofs(large_axis_file.c_str());
  oom_large.display_axis_spline_ascii(large_axis_ofs, 0, 0, 255);
  std::ofstream large_pts_ofs(large_pts_file.c_str());
  oom_large.display_cross_section_pointsets_ascii(large_pts_ofs, 0, 0, 255);
  std::ofstream large_axis_wrl_ofs(large_axis_wrl.c_str());
  oom_large.display_axis_spline(large_axis_wrl_ofs);
  std::ofstream large_cross_ofs(large_cross_wrl.c_str());
  oom_large.display_cross_section_planes(large_cross_ofs);
  std::ofstream large_pts_wrl_ofs(large_pts_wrl.c_str());
  oom_large.display_cross_section_pointsets(large_pts_wrl_ofs);

  // shrink
  pr_scale.scale_factor_ = 0.5;
  oom.set_params(pr_scale);
  boxm2_vecf_orbicularis_oris oom_shrink = oom.scale();

  std::string shrink_axis_wrl  = base_dir + "/shrink_axis.wrl";
  std::string shrink_cross_wrl = base_dir + "/shrink_cross.wrl";
  std::string shrink_pts_wrl   = base_dir + "/shrink_pts.wrl";
  std::string shrink_axis_file = base_dir + "/shrink_axis.txt";
  std::string shrink_pts_file  = base_dir + "/shrink_pts.txt";
  std::ofstream shrink_axis_ofs(shrink_axis_file.c_str());
  oom_shrink.display_axis_spline_ascii(shrink_axis_ofs, 255, 0, 0);
  std::ofstream shrink_pts_ofs(shrink_pts_file.c_str());
  oom_shrink.display_cross_section_pointsets_ascii(shrink_pts_ofs, 255, 0, 0);
  std::ofstream shrink_axis_wrl_ofs(shrink_axis_wrl.c_str());
  oom_shrink.display_axis_spline(shrink_axis_wrl_ofs);
  std::ofstream shrink_cross_wrl_ofs(shrink_cross_wrl.c_str());
  oom_shrink.display_cross_section_planes(shrink_cross_wrl_ofs);
  std::ofstream shrink_pts_wrl_ofs(shrink_pts_wrl.c_str());
  oom_shrink.display_cross_section_pointsets(shrink_pts_wrl_ofs);

  // test tilt operation
  // open
  boxm2_vecf_orbicularis_oris_params pr_tilt = pr;
  pr_tilt.tilt_angle_in_deg_ = 25.0;
  oom.set_params(pr_tilt);
  boxm2_vecf_orbicularis_oris oom_open = oom.tilt();

  std::string open_axis_wrl  = base_dir + "/mouth_open_25_deg_axis.wrl";
  std::string open_cross_wrl = base_dir + "/mouth_open_25_deg_cross.wrl";
  std::string open_pts_wrl   = base_dir + "/mouth_open_25_deg_pts.wrl";
  std::string open_axis_file = base_dir + "/mouth_open_25_deg_axis.txt";
  std::string open_pts_file  = base_dir + "/mouth_open_25_deg_pts.txt";
  std::ofstream open_axis_ofs(open_axis_file.c_str());
  oom_open.display_axis_spline_ascii(open_axis_ofs, 0, 255, 255);
  std::ofstream open_pts_ofs(open_pts_file.c_str());
  oom_open.display_cross_section_pointsets_ascii(open_pts_ofs, 0, 255, 255);
  std::ofstream open_axis_wrl_ofs(open_axis_wrl.c_str());
  oom_open.display_axis_spline(open_axis_wrl_ofs);
  std::ofstream open_cross_wrl_ofs(open_cross_wrl.c_str());
  oom_open.display_cross_section_planes(open_cross_wrl_ofs);
  std::ofstream open_pts_wrl_ofs(open_pts_wrl.c_str());
  oom_open.display_cross_section_pointsets(open_pts_wrl_ofs);

  // close
  pr_tilt.tilt_angle_in_deg_ = -20.0;
  oom.set_params(pr_tilt);
  boxm2_vecf_orbicularis_oris oom_close = oom.tilt();

  std::string close_axis_wrl  = base_dir + "/mouth_close_20_deg_axis.wrl";
  std::string close_cross_wrl = base_dir + "/mouth_close_20_deg_cross.wrl";
  std::string close_pts_wrl   = base_dir + "/mouth_close_20_deg_pts.wrl";
  std::string close_axis_file = base_dir + "/mouth_close_20_deg_axis.txt";
  std::string close_pts_file  = base_dir + "/mouth_close_20_deg_pts.txt";
  std::ofstream close_axis_ofs(close_axis_file.c_str());
  oom_close.display_axis_spline_ascii(close_axis_ofs, 138, 43, 226);
  std::ofstream close_pts_ofs(close_pts_file.c_str());
  oom_close.display_cross_section_pointsets_ascii(close_pts_ofs, 138, 43, 226);
  std::ofstream close_axis_wrl_ofs(close_axis_wrl.c_str());
  oom_close.display_axis_spline(close_axis_wrl_ofs);
  std::ofstream close_cross_wrl_ofs(close_cross_wrl.c_str());
  oom_close.display_cross_section_planes(close_cross_wrl_ofs);
  std::ofstream close_pts_wrl_ofs(close_pts_wrl.c_str());
  oom_close.display_cross_section_pointsets(close_pts_wrl_ofs);

  // test pucker deformation
  oom.set_params(pr);
  boxm2_vecf_orbicularis_oris oom_deformed = oom.circular_deform();

  std::string out_axis_vrml  = base_dir + "/pucker_axis.wrl";
  std::string out_pts_vrml   = base_dir + "/pucker_pts.wrl";
  std::string out_cross_vrml = base_dir + "/pucker_cross.wrl";
  std::string def_axis_file  = base_dir + "/pucker_axis.txt";
  std::string def_pts_file   = base_dir + "/pucker_pts.txt";

  std::ofstream aostr_vrml(out_axis_vrml.c_str());
  oom_deformed.display_axis_spline(aostr_vrml);
  std::ofstream postr_vrml(out_pts_vrml.c_str());
  oom_deformed.display_cross_section_pointsets(postr_vrml);
  std::ofstream costr_vrml(out_cross_vrml.c_str());
  oom_deformed.display_cross_section_planes(costr_vrml);
  std::ofstream aostr_def(def_axis_file.c_str());
  oom_deformed.display_axis_spline_ascii(aostr_def, 255, 255, 0);
  std::ofstream postr_def(def_pts_file.c_str());
  oom_deformed.display_cross_section_pointsets_ascii(postr_def, 255, 255, 0);


  return true;
}

void test_orbicularis_oris()
{
#if 0
  bool success = test_orbicularis_oris_params();
  TEST("orbicular oris parameter" , success, true);

  std::string param_file = "./boxm2_vecf_orbicularis_oris_params.txt";
  std::string geometry_file = "D:/work/janus/Orbicularis_Oris/expt/geometry_file.txt";
  if ( vul_file::exists(param_file) && vul_file::exists(geometry_file)) {
    success = test_orbicularis_oris_doformation(geometry_file, param_file);
    TEST("orbicular oris deformation", success, true);
  }
  else {
    std::cout << "Missing input geometry file: " << geometry_file << ", test boxm2_vecf_orbicularis_oris ignored." << std::endl;
  }
  return;
#endif
}

TESTMAIN( test_orbicularis_oris );
