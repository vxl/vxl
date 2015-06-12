//:
// \file
// \author J.L. Mundy
// \date 5/28/15


#include <testlib/testlib_test.h>
#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_orbit_params.h"
#include "../boxm2_vecf_fit_orbit.h"
#include <vgl/vgl_sphere_3d.h>
//#define BUILD_FIT_ORBIT
void test_fit_orbit()
{
#ifdef BUILD_FIT_ORBIT
  vcl_string base_path = "c:/Users/mundy/VisionSystems/Janus/experiments/Helena/";
  boxm2_vecf_fit_orbit fo;
#if 0
  //labeled_point lp0(11.162500381470, 68.934997558594,50.533798217773, "right_eye_inner_cusp");
  labeled_point lp0(12.48, 66.9,50.533798217773, "right_eye_inner_cusp");
  labeled_point lp1(-18.690200805664, 71.667800903320,36.611301422119, "right_eye_outer_cusp");
  labeled_point lp2(-4.839529991150, 64.590599060059,51.780799865723, "right_eye_mid_lower_lid");
 // labeled_point lp3(-4.691959857941,78.860801696777,55.672100067139, "right_eye_mid_upper_lid");
   labeled_point lp3(-3.46,78.2, 55.672100067139,"right_eye_mid_upper_lid");
   labeled_point lp4(-0.68,82.78, 52.04,"right_eye_mid_crease");
  fo.add_labeled_point(lp0);   fo.add_labeled_point(lp1); fo.add_labeled_point(lp2); fo.add_labeled_point(lp3);
  fo.add_labeled_point(lp4);
#endif
  vcl_string right_anchor_path = base_path + "cristina_right_anchors.txt";
  bool good = fo.read_anchor_file(right_anchor_path);
  if(!good)
    return;
   vcl_string right_sclera_path = base_path + "cristina_right_sclera.txt";
  //vcl_string right_sclera_path = base_path + "cristina_right_lower_lid_cover.txt";
  good = fo.load_orbit_data("right_eye_sclera", right_sclera_path);
  if(!good)
    return;
  good = fo.fit_right();
  if(!good)
    return;

  vcl_string left_anchor_path = base_path + "cristina_left_anchors.txt";
  good = fo.read_anchor_file(left_anchor_path);
  if(!good)
    return;
  vcl_string left_sclera_path = base_path + "cristina_left_sclera.txt";
  good = fo.load_orbit_data("left_eye_sclera", left_sclera_path);
  if(!good)
    return;
  good = fo.fit_left();
  if(!good)
    return;
#define RGT 1
#if RGT
  vcl_string right_lower_lid_path = base_path + "cristina_right_lower_lid.txt";
  fo.load_orbit_data("right_eye_mid_lower_lid", right_lower_lid_path);
  fo.plot_orbit_data("right_eye_mid_lower_lid");

  vcl_string right_upper_lid_path = base_path + "cristina_right_upper_lid.txt";
  fo.load_orbit_data("right_eye_mid_upper_lid", right_upper_lid_path);
  fo.plot_orbit_data("right_eye_mid_upper_lid");

  vcl_string right_crease_path = base_path + "cristina_right_crease.txt";
  fo.load_orbit_data("right_eye_mid_crease", right_crease_path);
  fo.plot_orbit_data("right_eye_mid_crease");
#else
  vcl_string left_lower_lid_path = base_path + "cristina_left_lower_lid.txt";
  fo.load_orbit_data("left_eye_mid_lower_lid", left_lower_lid_path);
  fo.plot_orbit_data("left_eye_mid_lower_lid");
 
  vcl_string left_upper_lid_path = base_path + "cristina_left_upper_lid.txt";
  fo.load_orbit_data("left_eye_mid_upper_lid", left_upper_lid_path);
  fo.plot_orbit_data("left_eye_mid_upper_lid");

   vcl_string left_crease_path = base_path + "cristina_left_crease.txt";
  fo.load_orbit_data("left_eye_mid_crease", left_crease_path);
  fo.plot_orbit_data("left_eye_mid_crease");
#endif
#endif //BUILD_FIT_ORBIT
}
TESTMAIN( test_fit_orbit );
 
