//:
// \file
// \author J.L. Mundy
// \date 6/23/15


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_orbit_params.h"
#include "../boxm2_vecf_fit_orbit.h"
#include "../boxm2_vecf_fit_margins.h"
#include <vgl/vgl_sphere_3d.h>
//#define BUILD_FIT_MARGIN
#define CRISTINA 1
#define P069 0
#define P113 0
#define bs00 0
static bool show_model = true;
void test_fit_margin()
{
bool good = false;
#ifdef BUILD_FIT_MARGIN
#if CRISTINA
  std::string cr_base_path = "c:/Users/mundy/VisionSystems/Janus/experiments/Helena/cristina/";
  boxm2_vecf_fit_orbit cr_fo;
  std::string cr_right_anchor_path = cr_base_path + "cristina_right_anchors.txt";
  good = cr_fo.read_anchor_file(cr_right_anchor_path);
  if(!good)
    return;
   std::string cr_right_sclera_path = cr_base_path + "cristina_right_sclera.txt";
  //std::string right_sclera_path = base_path + "cristina_right_lower_lid_cover.txt";
  good = cr_fo.load_orbit_data("right_eye_sclera", cr_right_sclera_path);
  if(!good)
    return;
  good = cr_fo.fit_right();
  if(!good)
    return;

  std::string cr_left_anchor_path = cr_base_path + "cristina_left_anchors.txt";
  good = cr_fo.read_anchor_file(cr_left_anchor_path);
  if(!good)
    return;
  std::string cr_left_sclera_path = cr_base_path + "cristina_left_sclera.txt";
  good = cr_fo.load_orbit_data("left_eye_sclera", cr_left_sclera_path);
  if(!good)
    return;
  good = cr_fo.fit_left();
  if(!good)
    return;


  std::string cr_right_inferior_margin_path = cr_base_path + "cristina_right_inferior_margin.txt";
  cr_fo.load_orbit_data("right_eye_inferior_margin", cr_right_inferior_margin_path);

  std::string cr_right_superior_margin_path = cr_base_path + "cristina_right_superior_margin.txt";
  cr_fo.load_orbit_data("right_eye_superior_margin", cr_right_superior_margin_path);

  std::string cr_right_superior_crease_path = cr_base_path + "cristina_right_superior_crease.txt";
  cr_fo.load_orbit_data("right_eye_superior_crease", cr_right_superior_crease_path);

  // non-linear fit
  vgl_point_3d<double> rlat, rmed;
  good = cr_fo.lab_point("right_eye_lateral_canthus", rlat);
  if(!good)
    return;
  good = cr_fo.lab_point("right_eye_medial_canthus", rmed);
  if(!good)
    return;
  boxm2_vecf_fit_margins cr_right_fmargs(cr_fo.orbit_data("right_eye_inferior_margin"), cr_fo.orbit_data("right_eye_superior_margin"),
                                         cr_fo.orbit_data("right_eye_superior_crease"), rlat, rmed,true);
  cr_right_fmargs.set_initial_guess( cr_fo.right_params());
  cr_right_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params cr_rprm = cr_right_fmargs.orbit_parameters();
   cr_fo.set_right_params(cr_rprm);
  std::string cr_right_marg_path = cr_base_path + "cristina_right_margin_fit.txt";
  std::ofstream cr_rmstr(cr_right_marg_path.c_str());
  good = cr_right_fmargs.plot_orbit(cr_rmstr);
  cr_rmstr.close();
  ///
  std::string cr_right_vrml_path = cr_base_path + "cristina_right_orbit_plot.wrl";
  std::ofstream cr_rostr(cr_right_vrml_path.c_str());
  good = cr_fo.display_orbit_vrml(cr_rostr, true, show_model);
  cr_rostr.close();

  std::string cr_right_param_path = cr_base_path + "cristina_right_orbit_params.txt";
  std::ofstream cr_rpstr(cr_right_param_path.c_str());
  cr_rpstr << cr_fo.right_params() << '\n';
  cr_rpstr.close();

  std::string cr_left_inferior_margin_path = cr_base_path + "cristina_left_inferior_margin.txt";
  cr_fo.load_orbit_data("left_eye_inferior_margin", cr_left_inferior_margin_path);

  std::string cr_left_superior_margin_path = cr_base_path + "cristina_left_superior_margin.txt";
  cr_fo.load_orbit_data("left_eye_superior_margin", cr_left_superior_margin_path);

  std::string cr_left_superior_crease_path = cr_base_path + "cristina_left_superior_crease.txt";
  cr_fo.load_orbit_data("left_eye_superior_crease", cr_left_superior_crease_path);

  // non-linear fit
  vgl_point_3d<double> llat, lmed;
  good = cr_fo.lab_point("left_eye_lateral_canthus", llat);
  if(!good)
    return;
  good = cr_fo.lab_point("left_eye_medial_canthus", lmed);
  if(!good)
    return;

  boxm2_vecf_fit_margins cr_left_fmargs(cr_fo.orbit_data("left_eye_inferior_margin"), cr_fo.orbit_data("left_eye_superior_margin"),
                                        cr_fo.orbit_data("left_eye_superior_crease"), llat, lmed);
  cr_left_fmargs.set_initial_guess( cr_fo.left_params());
  cr_left_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params cr_lprm = cr_left_fmargs.orbit_parameters();
  cr_fo.set_left_params(cr_lprm);
  std::string cr_left_marg_path = cr_base_path + "cristina_left_margin_fit.txt";
  std::ofstream cr_lmstr(cr_left_marg_path.c_str());
  good = cr_left_fmargs.plot_orbit(cr_lmstr);
  cr_lmstr.close();
  ///
  std::string cr_left_vrml_path = cr_base_path + "cristina_left_orbit_plot.wrl";
  std::ofstream cr_lostr(cr_left_vrml_path.c_str());
  good = cr_fo.display_orbit_vrml(cr_lostr, false,show_model);
  cr_lostr.close();
  std::string cr_left_param_path = cr_base_path + "cristina_left_orbit_params.txt";
  std::ofstream cr_lpstr(cr_left_param_path.c_str());
  cr_lpstr << cr_fo.left_params() << '\n';
  cr_lpstr.close();

#endif //CRISTINA
#if P069
  std::string p69_base_path = "c:/Users/mundy/VisionSystems/Janus/experiments/Helena/patient_069/";
  boxm2_vecf_fit_orbit p69_fo;
  std::string p69_right_anchor_path = p69_base_path + "patient_069_right_anchors.txt";
  good = p69_fo.read_anchor_file(p69_right_anchor_path);
  if(!good)
    return;
   std::string p69_right_sclera_path = p69_base_path + "patient_069_right_sclera.txt";
  //std::string right_sclera_path = base_path + "patient_069_right_lower_lid_cover.txt";
  good = p69_fo.load_orbit_data("right_eye_sclera", p69_right_sclera_path);
  if(!good)
    return;
  good = p69_fo.fit_right();
  if(!good)
    return;

  std::string p69_left_anchor_path = p69_base_path + "patient_069_left_anchors.txt";
  good = p69_fo.read_anchor_file(p69_left_anchor_path);
  if(!good)
    return;
  std::string p69_left_sclera_path = p69_base_path + "patient_069_left_sclera.txt";
  good = p69_fo.load_orbit_data("left_eye_sclera", p69_left_sclera_path);
  if(!good)
    return;
  good = p69_fo.fit_left();
  if(!good)
    return;


  std::string p69_right_inferior_margin_path = p69_base_path + "patient_069_right_inferior_margin.txt";
  p69_fo.load_orbit_data("right_eye_inferior_margin", p69_right_inferior_margin_path);

  std::string p69_right_superior_margin_path = p69_base_path + "patient_069_right_superior_margin.txt";
  p69_fo.load_orbit_data("right_eye_superior_margin", p69_right_superior_margin_path);

  std::string p69_right_superior_crease_path = p69_base_path + "patient_069_right_superior_crease.txt";
  p69_fo.load_orbit_data("right_eye_superior_crease", p69_right_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins p69_right_fmargs(p69_fo.orbit_data("right_eye_inferior_margin"), p69_fo.orbit_data("right_eye_superior_margin"),
                                          p69_fo.orbit_data("right_eye_superior_crease"), true);
  p69_right_fmargs.set_initial_guess( p69_fo.right_params());
  p69_right_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params p69_rprm = p69_right_fmargs.orbit_parameters();
  p69_fo.set_right_params(p69_rprm);
  std::string p69_right_marg_path = p69_base_path + "patient_069_right_margin_fit.txt";
  std::ofstream p69_rmstr(p69_right_marg_path.c_str());
  good = p69_right_fmargs.plot_orbit(p69_rmstr);
  p69_rmstr.close();
  ///
  std::string p69_right_vrml_path = p69_base_path + "patient_069_right_orbit_plot.wrl";
  std::ofstream p69_rostr(p69_right_vrml_path.c_str());
  good = p69_fo.display_orbit_vrml(p69_rostr, true, show_model);
  p69_rostr.close();

  std::string p69_right_param_path = p69_base_path + "patient_069_right_orbit_params.txt";
  std::ofstream p69_rpstr(p69_right_param_path.c_str());
  p69_rpstr << p69_fo.right_params() << '\n';
  p69_rpstr.close();

  std::string p69_left_inferior_margin_path = p69_base_path + "patient_069_left_inferior_margin.txt";
  p69_fo.load_orbit_data("left_eye_inferior_margin", p69_left_inferior_margin_path);

  std::string p69_left_superior_margin_path = p69_base_path + "patient_069_left_superior_margin.txt";
  p69_fo.load_orbit_data("left_eye_superior_margin", p69_left_superior_margin_path);

  std::string p69_left_superior_crease_path = p69_base_path + "patient_069_left_superior_crease.txt";
  p69_fo.load_orbit_data("left_eye_superior_crease", p69_left_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins p69_left_fmargs(p69_fo.orbit_data("left_eye_inferior_margin"), p69_fo.orbit_data("left_eye_superior_margin"),
                                         p69_fo.orbit_data("left_eye_superior_crease") );
  p69_left_fmargs.set_initial_guess( p69_fo.left_params());
  p69_left_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params p69_lprm = p69_left_fmargs.orbit_parameters();
  p69_fo.set_left_params(p69_lprm);
  std::string p69_left_marg_path = p69_base_path + "patient_069_left_margin_fit.txt";
  std::ofstream p69_lmstr(p69_left_marg_path.c_str());
  good = p69_left_fmargs.plot_orbit(p69_lmstr);
  p69_lmstr.close();
  ///
  std::string p69_left_vrml_path = p69_base_path + "patient_069_left_orbit_plot.wrl";
  std::ofstream p69_lostr(p69_left_vrml_path.c_str());
  good = p69_fo.display_orbit_vrml(p69_lostr, false, show_model);
  p69_lostr.close();

  std::string p69_left_param_path = p69_base_path + "patient_069_left_orbit_params.txt";
  std::ofstream p69_lpstr(p69_left_param_path.c_str());
  p69_lpstr << p69_fo.left_params() << '\n';
  p69_lpstr.close();
#endif //P069
#if P113
  std::string p113_base_path = "c:/Users/mundy/VisionSystems/Janus/experiments/Helena/patient_113/";
  boxm2_vecf_fit_orbit p113_fo;
  std::string p113_right_anchor_path = p113_base_path + "patient_113_right_anchors.txt";
 good = p113_fo.read_anchor_file(p113_right_anchor_path);
  if(!good)
    return;
   std::string p113_right_sclera_path = p113_base_path + "patient_113_right_sclera.txt";
  //std::string right_sclera_path = p113_base_path + "patient_113_right_lower_lid_cover.txt";
  good = p113_fo.load_orbit_data("right_eye_sclera", p113_right_sclera_path);
  if(!good)
    return;
  good = p113_fo.fit_right();
  if(!good)
    return;

  std::string p113_left_anchor_path = p113_base_path + "patient_113_left_anchors.txt";
  good = p113_fo.read_anchor_file(p113_left_anchor_path);
  if(!good)
    return;
  std::string p113_left_sclera_path = p113_base_path + "patient_113_left_sclera.txt";
  good = p113_fo.load_orbit_data("left_eye_sclera", p113_left_sclera_path);
  if(!good)
    return;
  good = p113_fo.fit_left();
  if(!good)
    return;


  std::string p113_right_inferior_margin_path = p113_base_path + "patient_113_right_inferior_margin.txt";
  p113_fo.load_orbit_data("right_eye_inferior_margin", p113_right_inferior_margin_path);

  std::string p113_right_superior_margin_path = p113_base_path + "patient_113_right_superior_margin.txt";
  p113_fo.load_orbit_data("right_eye_superior_margin", p113_right_superior_margin_path);

  std::string p113_right_superior_crease_path = p113_base_path + "patient_113_right_superior_crease.txt";
  p113_fo.load_orbit_data("right_eye_superior_crease", p113_right_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins p113_right_fmargs(p113_fo.orbit_data("right_eye_inferior_margin"), p113_fo.orbit_data("right_eye_superior_margin"), true);
  p113_right_fmargs.set_initial_guess( p113_fo.right_params());
  p113_right_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params p113_rprm;
  double p113_right_dphi_rad = 0.0;
  p113_right_fmargs.orbit_parameters(p113_rprm, p113_right_dphi_rad);
  p113_fo.set_right_params(p113_rprm); p113_fo.set_right_dphi_rad(p113_right_dphi_rad);
  std::string p113_right_marg_path = p113_base_path + "patient_113_right_margin_fit.txt";
  std::ofstream p113_rmstr(p113_right_marg_path.c_str());
  good = p113_right_fmargs.plot_orbit(p113_rmstr);
  p113_rmstr.close();
  ///
  std::string p113_right_vrml_path = p113_base_path + "patient_113_right_orbit_plot.wrl";
  std::ofstream p113_rostr(p113_right_vrml_path.c_str());
  good = p113_fo.display_orbit_vrml(p113_rostr, true, show_model);
  p113_rostr.close();
  std::string p113_right_param_path = p113_base_path + "patient_113_right_orbit_params.txt";
  std::ofstream p113_rpstr(p113_right_param_path.c_str());
  p113_rpstr << p113_fo.right_params() << '\n';
  p113_rpstr.close();


  std::string p113_left_inferior_margin_path = p113_base_path + "patient_113_left_inferior_margin.txt";
  p113_fo.load_orbit_data("left_eye_inferior_margin", p113_left_inferior_margin_path);

  std::string p113_left_superior_margin_path = p113_base_path + "patient_113_left_superior_margin.txt";
  p113_fo.load_orbit_data("left_eye_superior_margin", p113_left_superior_margin_path);

  std::string p113_left_superior_crease_path = p113_base_path + "patient_113_left_superior_crease.txt";
  p113_fo.load_orbit_data("left_eye_superior_crease", p113_left_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins p113_left_fmargs(p113_fo.orbit_data("left_eye_inferior_margin"), p113_fo.orbit_data("left_eye_superior_margin"));
  p113_left_fmargs.set_initial_guess( p113_fo.left_params());
  p113_left_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params p113_lprm;
  double p113_left_dphi_rad = 0.0;
  p113_left_fmargs.orbit_parameters(p113_lprm, p113_left_dphi_rad);
  p113_fo.set_left_params(p113_lprm); p113_fo.set_left_dphi_rad(p113_left_dphi_rad);
  std::string p113_left_marg_path = p113_base_path + "patient_113_left_margin_fit.txt";
  std::ofstream p113_lmstr(p113_left_marg_path.c_str());
  good = p113_left_fmargs.plot_orbit(p113_lmstr);
  p113_lmstr.close();
  ///
  std::string p113_left_vrml_path = p113_base_path + "patient_113_left_orbit_plot.wrl";
  std::ofstream p113_lostr(p113_left_vrml_path.c_str());
  good = p113_fo.display_orbit_vrml(p113_lostr, false, show_model);
  p113_lostr.close();

  std::string p113_left_param_path = p113_base_path + "patient_113_left_orbit_params.txt";
  std::ofstream p113_lpstr(p113_left_param_path.c_str());
  p113_lpstr << p113_fo.left_params() << '\n';
  p113_lpstr.close();
#endif //P113
#if bs00
  std::string bs00_base_path = "c:/Users/mundy/VisionSystems/Janus/experiments/Bosphorous/bs00_CAU_0/";
  boxm2_vecf_fit_orbit bs00_fo;
  std::string bs00_right_anchor_path = bs00_base_path + "bs00_right_anchors.txt";
  good = bs00_fo.read_anchor_file(bs00_right_anchor_path);
  if(!good)
    return;
   std::string bs00_right_sclera_path = bs00_base_path + "bs00_right_sclera.txt";
  //std::string right_sclera_path = base_path + "bs00_right_lower_lid_cover.txt";
  good = bs00_fo.load_orbit_data("right_eye_sclera", bs00_right_sclera_path);
  if(!good)
    return;
  good = bs00_fo.fit_right();
  if(!good)
    return;

  std::string bs00_left_anchor_path = bs00_base_path + "bs00_left_anchors.txt";
  good = bs00_fo.read_anchor_file(bs00_left_anchor_path);
  if(!good)
    return;
  std::string bs00_left_sclera_path = bs00_base_path + "bs00_left_sclera.txt";
  good = bs00_fo.load_orbit_data("left_eye_sclera", bs00_left_sclera_path);
  if(!good)
    return;
  good = bs00_fo.fit_left();
  if(!good)
    return;


  std::string bs00_right_inferior_margin_path = bs00_base_path + "bs00_right_inferior_margin.txt";
  bs00_fo.load_orbit_data("right_eye_inferior_margin", bs00_right_inferior_margin_path);

  std::string bs00_right_superior_margin_path = bs00_base_path + "bs00_right_superior_margin.txt";
  bs00_fo.load_orbit_data("right_eye_superior_margin", bs00_right_superior_margin_path);

  std::string bs00_right_superior_crease_path = bs00_base_path + "bs00_right_superior_crease.txt";
  bs00_fo.load_orbit_data("right_eye_superior_crease", bs00_right_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins bs00_right_fmargs(bs00_fo.orbit_data("right_eye_inferior_margin"), bs00_fo.orbit_data("right_eye_superior_margin"), true);
  bs00_right_fmargs.set_initial_guess( bs00_fo.right_params());
  bs00_right_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params bs00_rprm;
  double bs00_right_dphi_rad = 0.0;
  bs00_right_fmargs.orbit_parameters(bs00_rprm, bs00_right_dphi_rad);
  bs00_fo.set_right_params(bs00_rprm); bs00_fo.set_right_dphi_rad(bs00_right_dphi_rad);
  std::string bs00_right_marg_path = bs00_base_path + "bs00_right_margin_fit.txt";
  std::ofstream bs00_rmstr(bs00_right_marg_path.c_str());
  good = bs00_right_fmargs.plot_orbit(bs00_rmstr);
  bs00_rmstr.close();
  /////

  std::string bs00_right_vrml_path = bs00_base_path + "bs00_right_orbit_plot.wrl";
  std::ofstream bs00_rostr(bs00_right_vrml_path.c_str());
  good = bs00_fo.display_orbit_vrml(bs00_rostr, true, show_model);
  bs00_rostr.close();
  std::string bs00_right_param_path = bs00_base_path + "bs00_right_orbit_params.txt";
  std::ofstream bs00_rpstr(bs00_right_param_path.c_str());
  bs00_rpstr << bs00_fo.right_params() << '\n';
  bs00_rpstr.close();

  std::string bs00_left_inferior_margin_path = bs00_base_path + "bs00_left_inferior_margin.txt";
  bs00_fo.load_orbit_data("left_eye_inferior_margin", bs00_left_inferior_margin_path);

  std::string bs00_left_superior_margin_path = bs00_base_path + "bs00_left_superior_margin.txt";
  bs00_fo.load_orbit_data("left_eye_superior_margin", bs00_left_superior_margin_path);

  std::string bs00_left_superior_crease_path = bs00_base_path + "bs00_left_superior_crease.txt";
  bs00_fo.load_orbit_data("left_eye_superior_crease", bs00_left_superior_crease_path);

  // non-linear fit
  boxm2_vecf_fit_margins bs00_left_fmargs(bs00_fo.orbit_data("left_eye_inferior_margin"), bs00_fo.orbit_data("left_eye_superior_margin"));
  bs00_left_fmargs.set_initial_guess( bs00_fo.left_params());
  bs00_left_fmargs.fit(&std::cout, true);
  boxm2_vecf_orbit_params bs00_lprm;
  double bs00_left_dphi_rad = 0.0;
  bs00_left_fmargs.orbit_parameters(bs00_lprm, bs00_left_dphi_rad);
  bs00_fo.set_left_params(bs00_lprm); bs00_fo.set_left_dphi_rad(bs00_left_dphi_rad);
  std::string bs00_left_marg_path = bs00_base_path + "bs00_left_margin_fit.txt";
  std::ofstream bs00_lmstr(bs00_left_marg_path.c_str());
  good = bs00_left_fmargs.plot_orbit(bs00_lmstr);
  bs00_lmstr.close();
  ///

  std::string bs00_left_vrml_path = bs00_base_path + "bs00_left_orbit_plot.wrl";
  std::ofstream bs00_lostr(bs00_left_vrml_path.c_str());
  good = bs00_fo.display_orbit_vrml(bs00_lostr, false,show_model);
  bs00_lostr.close();
  std::string bs00_left_param_path = bs00_base_path + "bs00_left_orbit_params.txt";
  std::ofstream bs00_lpstr(bs00_left_param_path.c_str());
  bs00_lpstr << bs00_fo.left_params() << '\n';
  bs00_lpstr.close();

#endif //BS00
#endif //BUILD_FIT_MARGIN
}
TESTMAIN( test_fit_margin );

