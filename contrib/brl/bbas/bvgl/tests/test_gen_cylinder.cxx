//:
// \file
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_cross_section.h>
#include <bvgl/bvgl_gen_cylinder.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#define TEST_GEN_CYLINDER 0
//: Test changes
static void test_gen_cylinder()
{
#if TEST_GEN_CYLINDER
  std::string base_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  std::string axis_path3 = base_dir  + "mandible_spline_2x_zero_samp_1.0_r35_norm.txt";
  std::string axis_display_path = base_dir  + "gen_cyl_axis_vrml.wrl";
  std::string cross_path = base_dir + "mandible-2x-zero-samp-1.0-r35-norm.txt";
  std::string aggregate_cross_path = base_dir + "aggregate_mandible-2x-zero-samp-1.0-r35-norm.txt";
  std::string cross_display_path = base_dir + "gen_cyl_display_cross.wrl";
  std::string cross_normal_display_path = base_dir + "gen_cyl_display_cross_normal.wrl";
  std::string cross_disks_display_path = base_dir + "gen_cyl_display_cross_disks.wrl";

  // read the spline axis
  std::ifstream istr(axis_path3.c_str());
   if(!istr){
     std::cout << "Bad axis file path " << axis_path3 << '\n';
     return;
   }
   bvgl_gen_cylinder gc;
   gc.read_axis_spline(istr);

   // read cross section points
   std::ifstream cistr(cross_path.c_str());
   if(!cistr){
     std::cout << "Bad cross file path " << cross_path << '\n';
     return;
   }
   gc.load_cross_section_pointsets(cistr, 100.0);
   vgl_point_3d<double> p(0.0, 0.0, 8.0);
   vgl_point_3d<double> pc;
   bool good = gc.closest_point(p, pc);
   double d = gc.surface_distance(p);
   // display functions
   // display the gc axis
   std::ofstream vostr(axis_display_path.c_str());
   if(!vostr){
     std::cout << "Bad axis display file path " << axis_display_path << '\n';
     return;
   }
   gc.display_axis_spline(vostr);

   // display the cross section plane normals
   std::ofstream cnostr(cross_normal_display_path.c_str());
   if(!cnostr){
     std::cout << "Bad cross section normal  display file path " << cross_normal_display_path << '\n';
     return;
   }
   gc.display_cross_section_planes(cnostr);

   // display the gcible cross section pointsets
   std::ofstream costr(cross_display_path.c_str());
   if(!costr){
     std::cout << "Bad cross section display file path " << cross_display_path << '\n';
     return;
   }
   gc.display_cross_section_pointsets(costr);

   std::ofstream cdostr(cross_disks_display_path.c_str());
   if(!cdostr){
     std::cout << "Bad cross section display file path " << cross_disks_display_path << '\n';
     return;
   }
   gc.display_surface_disks(cdostr);

   vgl_pointset_3d<double> agg_ptset = gc.aggregate_pointset();
   std::ofstream agg_pstr(aggregate_cross_path.c_str());
   agg_pstr << agg_ptset;
   agg_pstr.close();
#endif
}

TESTMAIN( test_gen_cylinder );
