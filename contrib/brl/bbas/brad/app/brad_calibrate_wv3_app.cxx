// brad_calibrate_wv3_app
// takes in a multispectral NTF image file, performs calibratation and atmospheric corrections
// saved as band##.tif in specified out directory
#include <string>

#include <vul/vul_file.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>
#include <brad/brad_spectral_angle_mapper.h>
#include <brad/brad_multispectral_functions.h>
#include <brad_worldview3_functions.h>

int main(int argc, char * argv[])
{

  if (argc != 3) {
    for (int i = 0; i < argc; i++) {
      std::cout << argv[i] << "\n";
    }
    std::cerr << "Usage: brad_calibrate_wv3_app [mul_file] [out_dir]\n";
    return 1;
  }

  std::string mul_file = argv[1];
  std::string out_dir = argv[2];

  // Note: expecting meta data and rpc files to be in same file location with the
  //       same file name as mul_file, but with IMD and RPB extensions, respectively
  //       mul_file should have NTF extension

  // Load metadata
  brad_image_metadata mul_meta(vul_file::strip_extension(mul_file) + ".IMD");

  // Load cameras
  std::ifstream mul_ifs((vul_file::strip_extension(mul_file) + ".RPB").c_str());
//UNUSED  vpgl_rational_camera<double>* mul_rpc =
    read_rational_camera<double>(mul_ifs);

  // Load the image, calibrate
  vil_image_resource_sptr mul_rsc = vil_load_image_resource(mul_file.c_str());
  vil_image_view<float> mul_img;
  vil_image_view<vxl_uint_16> raw = mul_rsc->get_view();
  brad_calibrate_wv3_img(mul_meta, raw, mul_img, false);

   // Correct for atmospherics
   float mean_albedo = 0.3;
   vil_image_view<float> cal_img;
   brad_estimate_reflectance_image_multi(mul_img, mean_albedo, cal_img);

   // save
   save_corrected_wv3(cal_img, out_dir);

   return 0;
};
