// file: create an RGB image from an MUL image after TOA adjustment and atmospheric normalization

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>
#include <vul/vul_awk.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>
#include <brad/brad_spectral_angle_mapper.h>
#include <brad/brad_multispectral_functions.h>
#include <brad_worldview3_functions.h>

int main(int  /*argc*/, char *  /*argv*/[])
{

  //---------------------------with SWIR----------------------
  // Set arguments
  std::string mul_file(
    //"C:/Users/sca0161/Documents/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
    ////"D:/data/core3d/sevastopol2/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
    //"/home/ozge/Dropbox_VSI/projects/GeoMetriX/sevastopol/sevastopol_WV03_imgs/2016_07_21_WV03/104001001E0AA000_P005_MUL/16JUL21091539-M1BS-056339611010_01_P005.NTF");
    "/home/ozge/Dropbox_VSI/projects/GeoMetriX/sevastopol/sevastopol_WV03_imgs/2016_08_15_WV03/056247235010_01_003/056247235010_01/056247235010_01_P001_MUL/16AUG15092138-M1BS-056247235010_01_P001.NTF");

  std::string out_dir(
    //"C:/Users/sca0161/Documents/sevastopol2/cpp result/");
    ////"D:/results/ms/");
    //"/home/ozge/projects/GeoMetriX/sevastopol/DG_normalized/2016_07_21_WV03/");
    "/home/ozge/projects/GeoMetriX/sevastopol/DG_normalized/2016_08_15_WV03/");


  brad_image_metadata mul_meta(vul_file::strip_extension(mul_file) + ".IMD");
  vil_image_resource_sptr mul_rsc = vil_load_image_resource(mul_file.c_str());
  vil_image_view<vxl_uint_16> raw = mul_rsc->get_view();
  vil_image_view<float> mul_img;
  brad_calibrate_wv3_img(mul_meta, raw, mul_img, false);

  // Correct for atmospherics
  float mean_albedo = 0.3;
  vil_image_view<float> cal_img;
  brad_estimate_reflectance_image_multi(mul_img, mean_albedo, cal_img);
  std::cerr << "done with image corrections\n";

  // visible byte image to be used from saving
  vil_image_view<vxl_byte> vis;

  //// Create an RGB image for visualization
  std::cerr << "Saving RGB image\n";
  vil_image_view<float> rgb_img(cal_img.ni(), cal_img.nj(), 3);
  for (int y = 0; y < cal_img.nj(); y++)
    for (int x = 0; x < cal_img.ni(); x++) {
      rgb_img(x, y, 0) = cal_img(x, y, 4);
      rgb_img(x, y, 1) = cal_img(x, y, 2);
      rgb_img(x, y, 2) = cal_img(x, y, 1);
    }

  vil_math_truncate_range(rgb_img, 0.0f, 1.0f);
  vil_convert_stretch_range_limited(rgb_img, vis, 0.0f, 1.0f);
  vil_save(vis, (out_dir + "rgb.png").c_str());

  ////Write bands
  std::cerr << "Saving band images\n";
  //for (int p = 0; p < 16; p++) {
  for (int p = 0; p < 8; p++) {
    vil_image_view<float> plane = vil_plane(cal_img, p);
    std::stringstream ss;
    ss << out_dir << p << ".png";
    vil_math_truncate_range(plane, 0.0f, 1.0f);
    vil_image_view<vxl_byte> vis;
    vil_convert_stretch_range_limited(
      plane, vis, 0.0f, 1.0f);
    vil_save(vis, ss.str().c_str());
  }
  return 0;
};
