// This is brl/bseg/brip/tests/test_filter_bank.cxx

#include <iostream>
#include <vil/vil_image_view.h>
#include <brip/brip_filter_bank.h>
#include <brip/brip_vil_float_ops.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
static void test_filter_bank()
{
  unsigned ni = 500, nj = 500;
  vil_image_view<float> view(ni, nj);
  view.fill(0.0f);
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
      if(i>=(10+j)&&i<=(20+j))
        view(i,j) = 1.0f;
  unsigned n_levels = 3;
  double scale_range = 32.0;
  float lambda0 = 3.0f, lambda1 = 1.2f, theta_interval = 30.0f;
  float cutoff_ratio = 0.01f;
  brip_filter_bank bnk(n_levels, scale_range, lambda0, lambda1, theta_interval,
                       cutoff_ratio, view);
  std::cout << bnk;
  vil_image_view<float>& resp = bnk.response(n_levels-1);
  float v = resp(ni/2, nj/2);
  TEST_NEAR("filter_bank response", v, -0.016099010f, 0.001f);
  unsigned band = bnk.invalid_border();
  std::cout << " Interval:" << band << '\n';
  TEST("invalid_border", band, 259);
  //======= temporary processing :: REMOVE =======//
  std::string rgb_path = "e:/images/TextureTraining/eo_ir_images/eo_app/exp_000.png";
  std::string ir_path = "e:/images/TextureTraining/eo_ir_images/ir_app/exp_000.png";
  std::string out_path = "e:/images/TextureTraining/eo_ir_images/sdvi_byte.tiff";
  vil_image_view<unsigned char> rgb = vil_load(rgb_path.c_str());
  vil_image_view<unsigned char> ir_img = vil_load(ir_path.c_str());
   ni = rgb.ni(); nj = rgb.nj();
  vil_image_view<unsigned char> out(ni, nj, 3);
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i){
      auto r = static_cast<float>(rgb(i,j,0));
      auto g = static_cast<float>(rgb(i,j,1));
      auto b = static_cast<float>(rgb(i,j,2));
      auto ir = static_cast<float>(ir_img(i,j,0));
      float NDVI = (ir-r)/(ir+r);
      float theta = static_cast<float>(vnl_math::pi/2.0) + std::atan(NDVI);
      float clr = 81.169f*theta;
      clr = clr>255.0f ? 255.0f : clr;
      out(i,j,0) =  static_cast<unsigned char>(clr);
      out(i,j,1) =  static_cast<unsigned char>(g);
      out(i,j,2) =  static_cast<unsigned char>(b);
    }
  vil_save(out, out_path.c_str());
}
TESTMAIN(test_filter_bank);
