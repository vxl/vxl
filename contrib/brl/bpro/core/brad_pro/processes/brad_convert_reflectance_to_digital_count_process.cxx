//This is brl/bpro/core/brad_pro/processes/brad_convert_reflectance_to_digital_count_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vnl/vnl_math.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>

//:sets input and output types
bool brad_convert_reflectance_to_digital_count_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: image with pixel values corresponding to reflectance values
  //1: image metadata
  //2: mean_reflectance value
  //3: normalize values? If TRUE, output image will floating point with input[4] mapped to 1.0
  //4: maximum digital count value (default 2047)
  //5: average airlight? If TRUE, will use all visible band to compute an average airlight value

  std::vector<std::string> input_types_(6);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "brad_image_metadata_sptr";
  input_types_[2] = "float";
  input_types_[3] = "bool";
  input_types_[4] = "unsigned";
  input_types_[5] = "bool";

  if (!pro.set_input_types(input_types_))
    return false;

  //output: digital count of original image - normalized to (0,1) if input 3 is set to TRUE
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  if (!pro.set_output_types(output_types_))
     return false;

  // set default normalization constant
  pro.set_input(4, new brdb_value_t<unsigned>(2047));

  return true;
}

bool brad_convert_reflectance_to_digital_count_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr reflectance_img_base = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
  auto mean_reflectance = pro.get_input<float>(2);
  bool do_normalization = pro.get_input<bool>(3);
  auto max_digital_count = pro.get_input<unsigned>(4);
  bool average_airlight = pro.get_input<bool>(5);

  //check inputs validity
  if (!reflectance_img_base) {
    std::cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  if (reflectance_img_base->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     std::cerr << "ERROR: brad_convert_reflectance_to_digital_count: expecting floating point image\n";
     return false;
  }
  auto* reflectance_img = dynamic_cast<vil_image_view<float>*>(reflectance_img_base.ptr());
  if (!reflectance_img) {
     std::cerr << "ERROR: brad_convert_reflectance_to_digital_count: error casting to float image\n";
     return false;
  }

  unsigned int ni = reflectance_img->ni();
  unsigned int nj = reflectance_img->nj();
  unsigned int np = reflectance_img->nplanes();

  vil_image_view<float> toa_radiance_img(ni, nj, np);
  toa_radiance_img.fill(0.0f);
  bool is_normalization = true;
  if (mean_reflectance <= 0.0)
    is_normalization = false;

  // convert surface reflectance to ToA reflectance
  brad_undo_reflectance_estimate(*reflectance_img, *mdata, mean_reflectance, toa_radiance_img, average_airlight, is_normalization);

  // convert ToA reflecatance to ToA radiance
  std::vector<double> solar_irradiance_val = mdata->normal_sun_irradiance_values_;
  if (np != solar_irradiance_val.size()) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane numebr to the length of solar irradiance.  "
                            << "Image plane number: " << np
                            << ", solar irradiance value length: " << solar_irradiance_val.size() << "!!!\n";
    return false;
  }
  double sun_dot_norm = std::sin(mdata->sun_elevation_ * vnl_math::pi_over_180) / vnl_math::pi;
  for (unsigned ii = 0; ii < np; ii++)
  {
    double band_norm = solar_irradiance_val[ii] * sun_dot_norm;
    vil_image_view<float> band = vil_plane(toa_radiance_img, ii);
    vil_math_scale_values(band, band_norm);
  }
  // convert ToA radiance to DG
  std::vector<double> gain = mdata->gains_;
  std::vector<double> offset = mdata->offsets_;
  std::vector<double> abscal = mdata->abscal_;
  std::vector<double> effect_band = mdata->effect_band_width_;
  if (np != abscal.size() || np != effect_band.size() ) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane number to the length of band dependent AbsCalFactor/EffectBandWidth.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << abscal.size() << ", offset length: " << effect_band.size() << "!!!\n";
    return false;
  }
  if (np != gain.size() || np != offset.size()) {
    std::cerr << pro.name() << "ERROR: Mismatch of image plane number to the length of band dependent gain/offset.  "
                            << "Image plane numebr: " << np
                            << ", gain length: " << gain.size() << ", offset length: " << offset.size() << "!!!\n";
    return false;
  }
  for (unsigned ii = 0; ii < np; ii++)
  {
    double abs_cal_factor = abscal[ii] / effect_band[ii];
    double band_norm = 1.0 / (gain[ii] * abs_cal_factor);
    vil_image_view<float> band = vil_plane(toa_radiance_img, ii);
    vil_math_scale_and_offset_values(band, band_norm, -offset[ii]);
  }

  vil_image_view_base_sptr output_img = nullptr;
  if (do_normalization) {
    auto *output_img_float = new vil_image_view<float>(ni,nj);
    vil_convert_stretch_range_limited(toa_radiance_img,*output_img_float,0.0f,(float)max_digital_count,0.0f,1.0f);
    output_img = output_img_float;
  }
  else {
    auto *output_img_uint16 = new vil_image_view<vxl_uint_16>(ni,nj);
    vil_convert_stretch_range_limited(toa_radiance_img,*output_img_uint16, 0.0f, (float)max_digital_count,(vxl_uint_16)0, (vxl_uint_16)max_digital_count);
    output_img = output_img_uint16;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, output_img);

  return true;
}
