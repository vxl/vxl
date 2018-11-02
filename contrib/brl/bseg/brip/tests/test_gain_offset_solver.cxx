// This is brl/bseg/brip/tests/test_gain_offset_solver.cxx

#include <iostream>
#include <cstdlib>
#include <vil/vil_image_view.h>
#include <brip/brip_gain_offset_solver.h>
#include <brip/brip_vil_float_ops.h>
#include <testlib/testlib_test.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_gain_offset_solver()
{
  // it is assumed the input i
  unsigned ni = 500, nj = 500;
  vil_image_view<float> test(ni, nj), model(ni, nj);
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i){
      // range 0-> 1
      auto v = static_cast<float>((std::rand()/(RAND_MAX+1.0)));
      model(i,j) = v;
      float noise = (0.01f)*(static_cast<float>((std::rand()/(RAND_MAX+1.0))));
      test(i,j) = v + noise;
    }
  brip_gain_offset_solver gos(model, test);
  bool good = gos.solve();
  float gain = gos.gain();
  float offset = gos.offset();
  std::cout << "gain = " << gain << "  offset = " << offset << '\n';
  float er = std::fabs(gain-1.0f);
  er += std::fabs(offset);
  if(!good) er += 1.0f;
  TEST_NEAR("gain, offset test", er, 0.0f, 0.01f);
  //
  //===========================    actual experiment =================
  //
#if 0
  std::string exp_path = "e:/mundy/Dropbox/deliveries/exp_imgs/view_00_exp.png";
  vil_image_view<unsigned char> exp = vil_load(exp_path.c_str());
   ni = exp.ni(); nj = exp.nj();
  vil_image_view<float> test_image =
    brip_vil_float_ops::convert_to_float(exp);
  vil_math_scale_values(test_image,1.0/255.0);
  vil_image_view<unsigned char> test_mask(ni, nj);
  test_mask.fill(false);
  for(unsigned j = 86; j<724; ++j)
    for(unsigned i = 47; i<771; ++i)
      test_mask(i,j) = static_cast<unsigned char>(255);

  std::string mod_path = "e:/mundy/Dropbox/deliveries/cropped_imgs_raw_pixel_values_11_bit_stretched/view_00_cropped_stretched.tiff";
  vil_image_view<float> in_img = vil_load(mod_path.c_str());
  //map the pixels by translation
  unsigned tu = 422, tv = 421;
  vil_image_view<float> model_image(ni, nj);
  for(unsigned j = 0; j<nj; ++j){
    unsigned jp = j + tv;
    for(unsigned i = 0; i<ni; ++i){
      unsigned ip = i + tu;
      model_image(i, j) = in_img(ip, jp);
    }
  }
  vil_image_view<unsigned char> model_mask;
  brip_gain_offset_solver gos2(model_image, test_image, model_mask, test_mask);
  good = gos2.solve();
  gain = gos2.gain();
  offset = gos2.offset();
  std::cout << "gain = " << gain << "  offset = " << offset << '\n';

  // ==== output resulting mapped image
  std::string map_path = "e:/images/TextureTraining/mapped_images/map_00.tiff";
  vil_image_view<float> mapped_img = gos2.mapped_test_image();
  vil_save(mapped_img, map_path.c_str());
  mod_path = "e:/images/TextureTraining/mapped_images/model_00.tiff";
  vil_save(model_image, mod_path.c_str());

  std::string mask_path = "e:/images/TextureTraining/mapped_images/test_mask_00.tiff";
  vil_save(test_mask, mask_path.c_str());
#endif
}
TESTMAIN(test_gain_offset_solver);
