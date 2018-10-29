//:
// \file
// \author J.L. Mundy
// \date 02/25/16


#include <string>
#include <fstream>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include "../boxm2_vecf_middle_fat_pocket_params.h"
#include "../boxm2_vecf_fit_fat_pocket.h"

//#define BUILD_FIT_FAT_POCKET
void test_fit_fat_pocket()
{
#ifdef BUILD_FIT_FAT_POCKET
  bool good = false;
#if 0
  vgl_box_3d<double> coupling_box;
  vgl_point_3d<double> fp0(58.9,-3.52, 52.65); vgl_point_3d<double> fp1(31.63,-39.0, 44.62);
  vgl_point_3d<double> fp2(69.68, 23.3, -7.1); vgl_point_3d<double> fp3(59.74, -24.98, -33.65);
  vgl_point_3d<double> fp4(41.34, -9.6, 87.54);vgl_point_3d<double> fp5(42.2, -56.64, 69.57);
  vgl_point_3d<double> fp6(86.76, 11.98, 25.09);vgl_point_3d<double> fp7(75.9, -51.5, 15.36);
  coupling_box.add(fp0);   coupling_box.add(fp1);
  coupling_box.add(fp2);   coupling_box.add(fp3);
  coupling_box.add(fp4);   coupling_box.add(fp5);
  coupling_box.add(fp6);   coupling_box.add(fp7);
  // neutral face bounding box
  vgl_box_3d<double> neu_bounding_box;
vgl_point_3d<double> pn0(30.623199462891,-0.656731009483,110.0);
vgl_point_3d<double> pn1(77.281097412109,9.051010131836,80.0);
vgl_point_3d<double> pn2(69.050697326660,-54.185100555420,70.0);
vgl_point_3d<double> pn3(30.946899414063,-61.403499603271,110.0);
vgl_point_3d<double> pn4(30.623199462891,-0.656731009483,60.0);
vgl_point_3d<double> pn5(77.281097412109,9.051010131836,30.0);
vgl_point_3d<double> pn6(69.050697326660,-54.185100555420,20.0);
vgl_point_3d<double> pn7(30.946899414063,-61.403499603271,60.0);
  neu_bounding_box.add(pn0);   neu_bounding_box.add(pn1);   neu_bounding_box.add(pn2);
  neu_bounding_box.add(pn3);   neu_bounding_box.add(pn4);   neu_bounding_box.add(pn5);
  neu_bounding_box.add(pn6);   neu_bounding_box.add(pn7);
  std::string base_path = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/";
#if 0 //extract skin
  std::string skin_path = base_path + "skull/skin_orig_no-appearance_sampled-r10-s1.05-t10.txt";
  vgl_pointset_3d<double> skin_pc, coupled_skin_pc;
  std::ifstream istr(skin_path.c_str());
  if(!istr)
   return;
  istr >> skin_pc;
  istr.close();
  for(unsigned i = 0; i<skin_pc.npts(); ++i){
    vgl_point_3d<double> pi = skin_pc.p(i);
    if(coupling_box.contains(pi))
      coupled_skin_pc.add_point(pi);
  }
  std::string coupled_skin_pc_path = base_path + "fat_pocket/coupled_skin_pts.txt";
  std::ofstream ostr(coupled_skin_pc_path.c_str());
  if(!ostr) return;
  ostr << coupled_skin_pc;
  ostr.close();
#else // extract neutral
  vgl_pointset_3d<double> neutral_pc, coupled_neutral_pc;
  //std::string neu_pc_path = base_path + "bs000_N_N_1/bs000_N_N_1_inv_trans.txt";
  std::string neu_pc_path = base_path + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_inv_trans.txt";
  std::ifstream istr(neu_pc_path.c_str());
  if(!istr)
   return;
  istr >> neutral_pc;
  istr.close();
  for(unsigned i = 0; i<neutral_pc.npts(); ++i){
    vgl_point_3d<double> pi = neutral_pc.p(i);
    if(neu_bounding_box.contains(pi))
      coupled_neutral_pc.add_point(pi);
  }
  //std::string coupled_neutral_pc_path = base_path + "bs000_N_N_1/bs000_N_N_1_coupled_inv_pc.txt";
  std::string coupled_neutral_pc_path = base_path + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_coupled_inv_pc.txt";
  std::ofstream ostr(coupled_neutral_pc_path.c_str());
  if(!ostr) return;
  ostr << coupled_neutral_pc;
  ostr.close();

#endif
#else
  std::string base_path = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/";
  std::string neutral_face_pc_path = base_path + "bs000_N_N_1/bs000_N_N_1_coupled_inv_pc.txt";
  std::string deformed_face_pc_path = base_path + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_coupled_inv_pc.txt";
  std::string skin_pc_path = base_path + "fat_pocket/coupled_skin_pts.txt";
  std::string scene_dir = "d:/VisionSystems/Janus/experiments/vector_flow/fat_pocket/";
  std::string fat_pocket_geo_path = scene_dir + "middle_fat_pocket_geo.txt";
#if 1
  boxm2_vecf_fit_fat_pocket ffp(neutral_face_pc_path, skin_pc_path, fat_pocket_geo_path);
  boxm2_vecf_middle_fat_pocket_params params;
  double err = ffp.fit_neutral(&std::cout, true);
  std::cout << "Returned error value " << err << '\n';
  std::string fit_error_path = base_path + "bs000_N_N_1/bs000_N_N_1_fit_error_plot.txt";
  std::ofstream err_ostr(fit_error_path.c_str());
  if(!err_ostr) return;
  ffp.plot_middle_fat_pocket_fit(err_ostr);
  err_ostr.close();
#else
  boxm2_vecf_fit_fat_pocket ffp(neutral_face_pc_path, deformed_face_pc_path, skin_pc_path, fat_pocket_geo_path);
  boxm2_vecf_middle_fat_pocket_params params;
  double err = ffp.fit_deformed(&std::cout, true);
  std::cout << "Returned error value " << err << '\n';
  std::string fit_error_path = base_path + "bs000_E_HAPPY_0/bs000_E_HAPPY_0_fit_error_plot.txt";
  std::ofstream err_ostr(fit_error_path.c_str());
  if(!err_ostr) return;
  ffp.plot_middle_fat_pocket_fit(err_ostr);
  err_ostr.close();
#endif
#endif
#endif //BUILD_FIT_FAT_POCKET
}
TESTMAIN( test_fit_fat_pocket );
