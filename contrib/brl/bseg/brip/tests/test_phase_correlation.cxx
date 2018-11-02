// This is brl/bseg/brip/tests/test_fourier.cxx
#include <iostream>
#include <string>
#include <vector>
#include <vil/vil_image_view.h>
//#define DEBUG
#ifdef DEBUG
#include <vil/vil_save.h>
#endif
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_phase_correlation.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vnl/algo/vnl_fft.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_distance.h>
static void test_phase_correlation_ortho()
{
#ifdef DEBUG // form your own paths to test data
  std::string base_path = "c:/Users/mundy/VisionSystems/Finder/PhaseCorr/Testing/";
  std::string path0 = base_path + "orth0.jpg";
  std::string path1 = base_path + "orth1.jpg";
  std::string path2 = base_path + "orth2.jpg";
  std::string path3 = base_path + "orth3.jpg";
  std::string path4 = base_path + "orth4.jpg";

  std::string orth_corr_path_01 = base_path + "ortho_corr_01.tiff";
  std::string orth_corr_path_02 = base_path + "ortho_corr_02.tiff";
  std::string orth_corr_path_03 = base_path + "ortho_corr_03.tiff";
  std::string orth_corr_path_04 = base_path + "ortho_corr_04.tiff";
  bool good;

  std::vector<std::string> img_paths, out_paths;
  img_paths.push_back(path1);
  img_paths.push_back(path2);
  img_paths.push_back(path3);
  img_paths.push_back(path4);
  out_paths.push_back(orth_corr_path_01);
  out_paths.push_back(orth_corr_path_02);
  out_paths.push_back(orth_corr_path_03);
  out_paths.push_back(orth_corr_path_04);
  vil_image_view_base_sptr oimg0_ptr = vil_load(path0.c_str());
  vil_image_view<float> ofimg0 = vil_convert_cast(float(), oimg0_ptr);
  for(unsigned i = 0; i<4; ++i){
    vil_image_view_base_sptr oimgi_ptr = vil_load(img_paths[i].c_str());
    vil_image_view<float> ofimgi = vil_convert_cast(float(), oimgi_ptr);
    brip_phase_correlation bpco(ofimg0, ofimgi);
    good = bpco.compute();
    if(good){
      vil_image_view<float> ortho_corr = bpco.correlation_array();
      vil_save(ortho_corr,out_paths[i].c_str());
      float tu , tv, conf;
      if(bpco.translation(tu, tv, conf))
        std::cout << "t[" << i << "](" << tu << ' ' << tv << "): " << conf << '\n';
      else
        std::cout << "registration failed\n";
      }
    }
#endif
}
static void test_phase_correlation_homography(){
#ifdef DEBUG // form your own paths to test data
        vgl_homg_point_2d<double> mar03_p0(149.117615,60.867294);
        vgl_homg_point_2d<double> mar03_p1(48.173683,1079.628906);
        vgl_homg_point_2d<double> mar03_p2(826.857788,1195.778564);
        vgl_homg_point_2d<double> mar03_p3(1274.003296,1171.474487);
        vgl_homg_point_2d<double> mar03_p4(1479.977905,200.576233);
        vgl_homg_point_2d<double> mar03_p5(1602.992188,905.751099);
        vgl_homg_point_2d<double> mar03_p6(1651.440674,1119.098389);
        vgl_homg_point_2d<double> mar03_p7(1673.796875,292.729279);
        vgl_homg_point_2d<double> mar03_p8(1742.859009,548.979309);
        vgl_homg_point_2d<double> mar03_p9(1777.024414,152.905304);
        vgl_homg_point_2d<double> mar03_p10(1796.042969,1019.816528);
        std::vector<vgl_homg_point_2d<double> > to_pts;
        to_pts.push_back(mar03_p0);to_pts.push_back(mar03_p1);to_pts.push_back(mar03_p2);
        to_pts.push_back(mar03_p3);to_pts.push_back(mar03_p4);to_pts.push_back(mar03_p5);
        to_pts.push_back(mar03_p6);to_pts.push_back(mar03_p7);to_pts.push_back(mar03_p8);
        to_pts.push_back(mar03_p9);to_pts.push_back(mar03_p10);
        vgl_homg_point_2d<double> oct04_p0(193.306061,65.392708);
        vgl_homg_point_2d<double> oct04_p1(65.200920,1226.331665);
        vgl_homg_point_2d<double> oct04_p2(1063.875488,1386.002808);
        vgl_homg_point_2d<double> oct04_p3(1633.005371,1371.978638);
        vgl_homg_point_2d<double> oct04_p4(1899.592773,270.906830);
        vgl_homg_point_2d<double> oct04_p5(2057.141846,1080.180420);
        vgl_homg_point_2d<double> oct04_p6(2121.226318,1325.007568);
        vgl_homg_point_2d<double> oct04_p7(2148.175537,383.201691);
        vgl_homg_point_2d<double> oct04_p8(2237.742188,677.563232);
        vgl_homg_point_2d<double> oct04_p9(2277.867188,226.158157);
        vgl_homg_point_2d<double> oct04_p10(2305.153564,1216.641846);
        std::vector<vgl_homg_point_2d<double> > frm_pts;
        frm_pts.push_back(oct04_p0);frm_pts.push_back(oct04_p1);frm_pts.push_back(oct04_p2);
        frm_pts.push_back(oct04_p3);frm_pts.push_back(oct04_p4);frm_pts.push_back(oct04_p5);
        frm_pts.push_back(oct04_p6);frm_pts.push_back(oct04_p7);frm_pts.push_back(oct04_p8);
        frm_pts.push_back(oct04_p9);frm_pts.push_back(oct04_p10);
        vgl_h_matrix_2d_compute_linear hc;
        vgl_h_matrix_2d<double> H;
        bool good = hc.compute(frm_pts, to_pts, H);
        if(!good)
          return;
        // test projection
        for(unsigned i = 0; i<11; ++i){
          vgl_homg_point_2d<double>& pfrm = frm_pts[i];//oct04
          vgl_homg_point_2d<double>& pto = to_pts[i];//mar03
          vgl_homg_point_2d<double> t_frm  = H*pfrm;//oct04->mar03
          double d = vgl_distance(pto, t_frm);
          std::cout << i << ' ' << d << '\n';
        }
        std::string base_path = "c:/Users/mundy/VisionSystems/Finder/PhaseCorr/TestHomg/";
        std::string mar03 = base_path + "MAR03Pc.tiff";
        std::string oct04 = base_path + "OCT04P.tiff";
        std::string o04_m03 = base_path + "o04_to_m03.tiff";
        std::string corr_array = base_path + "corr.tiff";
        vil_image_view_base_sptr mar03_ptr = vil_load(mar03.c_str());
        if(!mar03_ptr) return ;
        vil_image_view<float> mar03f = vil_convert_cast(float(), mar03_ptr);
            vil_image_view_base_sptr oct04_ptr = vil_load(oct04.c_str());
        if(!oct04_ptr) return ;
        vil_image_view<float> oct04f = vil_convert_cast(float(), oct04_ptr);
        unsigned ni_mar03 = mar03f.ni(), nj_mar03 = mar03f.nj();
        vil_image_view<float> o04_to_m03(ni_mar03, nj_mar03);
        good = brip_vil_float_ops::homography(oct04f, H, o04_to_m03, true);
        if(!good) return;
        vil_save(o04_to_m03, o04_m03.c_str());
        brip_phase_correlation bpco(mar03f, o04_to_m03);
        good = bpco.compute();
        if(!good) return;
        vil_image_view<float> ortho_corr = bpco.correlation_array();
        vil_save(ortho_corr, corr_array.c_str());
        float tu , tv, conf;
        if(bpco.translation(tu, tv, conf))
          std::cout << "t(" << tu << ' ' << tv << "): " << conf << '\n';
        vgl_point_2d<double> rmar03_p0(166.434158,89.906898);
    vgl_point_2d<double> rmar03_p1(465.086426, 615.901733);
    vgl_point_2d<double> rmar03_p2(1596.039063,896.974426);
    vgl_point_2d<double> rmar03_p3(1383.876709,198.216309);
        std::vector<vgl_point_2d<double> > rmar03_pts;
        rmar03_pts.push_back(rmar03_p0);rmar03_pts.push_back(rmar03_p1);rmar03_pts.push_back(rmar03_p2);
        rmar03_pts.push_back(rmar03_p3);
        vgl_point_2d<double> to_mar03_p0(172.311478,96.282639);
    vgl_point_2d<double> to_mar03_p1(469.937897,623.132629);
    vgl_point_2d<double> to_mar03_p2(1602.076172,905.014099);
    vgl_point_2d<double> to_mar03_p3(1389.970215,206.118942);
        std::vector<vgl_point_2d<double> > to_mar03_pts;
        to_mar03_pts.push_back(to_mar03_p0);to_mar03_pts.push_back(to_mar03_p1);to_mar03_pts.push_back(to_mar03_p2);
        to_mar03_pts.push_back(to_mar03_p3);
        double sumu=0.0, sumv = 0.0;
        for(unsigned i = 0; i<4; ++i){
                vgl_point_2d<double>& rp = rmar03_pts[i];
                vgl_point_2d<double>& tp = to_mar03_pts[i];
                double du = tp.x()-rp.x();
                double dv = tp.y()-rp.y();
                std::cout << "act_t(" << du << ' ' << dv << '\n';
                sumu += du; sumv += dv;
        }
        double act_tu = sumu/4.0, act_tv = sumv/4.0;
        std::cout << "act_t_avg(" << act_tu<< ' ' << act_tv << '\n';
#endif
}
static void test_phase_correlation(){
        test_phase_correlation_ortho();
        test_phase_correlation_homography();
}
TESTMAIN(test_phase_correlation);
