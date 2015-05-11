// This is brl/bseg/brip/tests/test_fourier.cxx
#include <vil/vil_image_view.h>
//#define DEBUG
#ifdef DEBUG
#include <vil/vil_save.h>
#endif
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_phase_correlation.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vnl/algo/vnl_fft.h>
static void test_phase_correlation()
{
#ifdef DEBUG // form your own paths to test data
  std::string base_path = "c:/Users/mundy/VisionSystems/Finder/PhaseCorr/";
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

  vcl_vector<vcl_string> img_paths, out_paths;
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
        vcl_cout << "t[" << i << "](" << tu << ' ' << tv << "): " << conf << '\n';
      else
        vcl_cout << "registration failed\n";
      }
    }
#endif
}

TESTMAIN(test_phase_correlation);
