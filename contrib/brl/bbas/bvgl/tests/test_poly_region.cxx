//:
// \file
#include <iostream>
#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_poly_region_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#define TEST_POLY_REGION 0
//: Test changes
static void test_poly_region()
{
#if TEST_POLY_REGION
  std::string base_path = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/fat_pocket/";
  std::string ptset_name = "random_fat_pocket_pts.txt";
  std::string ptset_path = base_path + ptset_name;
  vgl_vector_3d<double> normal(-0.73911, 0.100747, -0.666008);
  vgl_point_3d<double> origin(31.4641,0.0,89.3596);
  double ucf[5] = {2.41, 498.764, -1048.39, 517.0861, 32.544};
  vnl_vector<double> u_coefs(ucf, 5);
  double vcf[6] = {6.418, 252.6395, -1436.3, 886.4, 1469.5457, -1172.42};
  vnl_vector<double> v_coefs(vcf, 6);
  bvgl_poly_region_3d<double> pr(u_coefs, v_coefs, normal, origin, 0.5);
  vgl_point_3d<double> p(40.352008819580,4.0,85.958847045898);
  bool good = pr.in(p);
  unsigned npts = 10000;
  vgl_pointset_3d<double> ptset = pr.random_pointset(npts);
  std::ofstream ostr(ptset_path.c_str());
  ostr << ptset;
  ostr.close();
#endif
}

TESTMAIN( test_poly_region );
