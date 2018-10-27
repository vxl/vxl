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
#include <brad/brad_worldview3_functions.h>

int main(int argc, char * argv[])
{
  if (argc != 5) {
    for (int i = 0; i < argc; i++) {
      std::cout << argv[i] << "\n";
    }
    std::cerr << "Usage: brad_create_sam_app [image_file] [aster_dir] [keyword] [out_file]\n";
    return 1;
  }

  //get our arguments
  std::string image_file, aster_dir, keyword, out_file;
  image_file = argv[1];
  aster_dir = argv[2];
  keyword = argv[3];
  out_file = argv[4];

  // Setup WV3 bands
  std::vector<float> bands_min, bands_max;
  brad_wv3_bands(bands_min, bands_max, 8);
  // create aster objects
  brad_spectral_angle_mapper aster(bands_min, bands_max);
  aster.add_aster_dir(aster_dir);
  std::cerr << "done with aster initialization\n";

  // Load the images
  vil_image_resource_sptr mul_img = vil_load_image_resource(image_file.c_str());
  std::cerr << "Loaded a " << mul_img->ni() << 'x' << mul_img->nj()
    << " image with " << mul_img->nplanes() << " channels\n";

  // Load metadata
  brad_image_metadata meta(vul_file::strip_extension(image_file) + ".IMD");

  // Calibrate image
  vil_image_view<vxl_uint_16> mul_uint16;
  mul_uint16 = mul_img->get_view();
  vil_image_view<float> mul_f;
  brad_calibrate_wv3_img(meta, mul_uint16, mul_f, false);

  // Correct for atmospherics
  float mean_albedo = 0.3;
  vil_image_view<float> cal_img;
  if (!brad_estimate_reflectance_image_multi(mul_f, mean_albedo, cal_img)) {
    std::cerr << "error estimating reflectance image\n";
    return 1;
  }
  std::cerr << "done with image corrections\n";

  vil_image_view<float> spectral_angle;
  if (!aster.compute_sam_img(cal_img, keyword, spectral_angle)) {
    std::cerr << "error computing spectral angle map";
  }
  vil_image_view<vxl_byte> to_save;
  vil_convert_stretch_range_limited(spectral_angle, to_save, (float) 0.0, (float) 1.0, (vxl_byte)0, (vxl_byte)255);
  if (!vil_save(to_save, out_file.c_str())) {
    std::cerr << "error saving spectral angle map";
    return 1;
  }
  return 0;
};
